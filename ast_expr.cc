/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */

#include <string.h>
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "symtable.h"
#include "errors.h"


void Expr::CheckStmt()   {
    this->CheckExpr();
}

/*
inline ExprError* ee(Expr* e)
{
  return dynamic_cast<ExprError*>(e); 
}

inline EmptyExpr* eme(Expr* e)
{
  return dynamic_cast<EmptyExpr*>(e); 
}

inline IntConstant* ic(Expr* e)
{
  return dynamic_cast<IntConstant*>(e);
}

inline FloatConstant* fc(Expr* e)
{
  return dynamic_cast<FloatConstant*>(e);
}

inline BoolConstant* bc(Expr* e)
{
  return dynamic_cast<BoolConstant*>(e); 
}

inline VarExpr* ve(Expr* e)
{
  return dynamic_cast<VarExpr*>(e);
}

inline CompoundExpr* ce(Expr* e)
{
  return dynamic_cast<CompoundExpr*>(e); 
}

inline ConditionalExpr* conde(Expr* e)
{
  return dynamic_cast<ConditionalExpr*>(e); 
}

inline LValue* lv(Expr* e)
{
  return dynamic_cast<LValue*>(e);
}

inline Call* call(Expr* e)
{
  return dynamic_cast<Call*>(e);
}

inline Expr::exprcast(Expr* e)
{
  if(ExprError *exprerror = ee(e))
  {
    return exprerror; 
  }

  else if(EmptyExpr *emptyexpr = eme(e))
  {
    return emptyexpr; 
  }
  else if(IntConstant *intconstant = ic(e))
  {
    return intconstant; 
  }
  else if(FloatConstant *floatconstant = fc(e))
  {
    return floatconstant; 
  }
  else if(BoolConstant *boolconstant = bc(e))
  {
    return boolconstant; 
  }
  else if(VarExpr* varexpr = ve(e))
  {
    return varexpr; 
  }
  else if(CompoundExpr* compoundexpr = ce(e))
  {
    return compoundexpr; 
  }
  else if(ConditionalExpr* condexpr = conde(e))
  {
    return condexpr; 
  }
  else if(LValue* lvalue = lv(e))
  {
    return lvalue; 
  }
  else if(Call* tcall = call(e))
  {
    return tcall; 
  } 

  return NULL; 
}

void Expr::CheckExpr(SymbolTable *st)
{
  exprcast(this) -> CheckExpr(st); 
}


Type* ExprError::CheckExpr()
{
  return Type::errorType;
}*/ 


IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}
void IntConstant::PrintChildren(int indentLevel) { 
    printf("%d", value);
}

FloatConstant::FloatConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}
void FloatConstant::PrintChildren(int indentLevel) { 
    printf("%g", value);
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}
void BoolConstant::PrintChildren(int indentLevel) { 
    printf("%s", value ? "true" : "false");
}

VarExpr::VarExpr(yyltype loc, Identifier *ident) : Expr(loc) {
    Assert(ident != NULL);
    this->id = ident;
}

void VarExpr::PrintChildren(int indentLevel) {
    id->Print(indentLevel+1);
}


void VarExpr::CheckExpr()   {
    vector<Decl*> matches = Node::symtab->findInAnyScope(this->id->GetName());
    for(vector<Decl*>::const_iterator it = matches.begin(); it != matches.end(); it++)    {
        if (VarDecl* vd = dynamic_cast<VarDecl*>(*it))  {
            this->type = vd->GetType();
        }
    }
    if (this->type == NULL) {
        ReportError::IdentifierNotDeclared(this->id, LookingForVariable);
        this->type = Type::errorType;
    }
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}

void Operator::PrintChildren(int indentLevel) {
    printf("%s",tokenString);
}

bool Operator::IsOp(const char *op) const {
    return strcmp(tokenString, op) == 0;
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r) 
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this); 
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r) 
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL; 
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o) 
  : Expr(Join(l->GetLocation(), o->GetLocation())) {
    Assert(l != NULL && o != NULL);
    (left=l)->SetParent(this);
    (op=o)->SetParent(this);
}

void CompoundExpr::PrintChildren(int indentLevel) {
   if (left) left->Print(indentLevel+1);
   op->Print(indentLevel+1);
   if (right) right->Print(indentLevel+1);
}

void ArithmeticExpr::CheckExpr() {
    //Post-order traversal
    left->CheckExpr();
    right->CheckExpr();

    Type * ltype = left->getType();
    Type * rtype = right->getType();

    if (!(ltype->IsNumeric() || ltype->IsError()))  {
        ReportError::IncompatibleOperand(op, ltype);
        left->setType(Type::errorType);
    }

    if (!(rtype->IsNumeric() || rtype->IsError()))  {
        ReportError::IncompatibleOperand(op, rtype);
        right->setType(Type::errorType);
    }
    //If we have two matching valid types, we know we're error free
    if (ltype->IsEquivalentTo(rtype))   {
        this->type = ltype;
        return;
    }
    //Otherwise we know this expr will be an errortype
    if(!(ltype->IsConvertibleTo(rtype) || rtype->IsConvertibleTo(ltype)))    {
        ReportError::IncompatibleOperands(op, ltype, rtype);
    }

    this->type = Type::errorType;
}

void RelationalExpr::CheckExpr() { 
    //Post-order traversal
    left->CheckExpr();
    right->CheckExpr();

    Type * ltype = left->getType();
    Type * rtype = right->getType();

    if (!(ltype->IsNumeric() || ltype->IsError()))  {
        ReportError::IncompatibleOperand(op, ltype);
        left->setType(Type::errorType);
    }

    if (!(rtype->IsNumeric() || rtype->IsError()))  {
        ReportError::IncompatibleOperand(op, rtype);
        right->setType(Type::errorType);
    }

    //If we have two matching valid types, we know we're error free
    if (ltype->IsEquivalentTo(rtype))   {
        this->type = (ltype->IsEquivalentTo(Type::errorType)) ? Type::errorType : Type::boolType;
        return;
    }

    if(!(ltype->IsConvertibleTo(rtype) || rtype->IsConvertibleTo(ltype)))    {
        ReportError::IncompatibleOperands(op, ltype, rtype);
    }
    
    this->type = Type::errorType;
}

void EqualityExpr::CheckExpr() { 
    //Post-order traversal
    left->CheckExpr();
    right->CheckExpr();

    Type * ltype = left->getType();
    Type * rtype = right->getType();

    //If we have two matching valid types, we know we're error free
    if (ltype->IsEquivalentTo(rtype))   {
        this->type = (ltype->IsEquivalentTo(Type::errorType)) ? Type::errorType : Type::boolType;
        return;
    }

    if(!(ltype->IsConvertibleTo(rtype) || rtype->IsConvertibleTo(ltype)))    {
        ReportError::IncompatibleOperands(op, ltype, rtype);
    }
    
    this->type = Type::errorType;
}

void LogicalExpr::CheckExpr() { 
    //Post-order traversal
    left->CheckExpr();
    right->CheckExpr();

    Type * ltype = left->getType();
    Type * rtype = right->getType();

    if (ltype->IsEquivalentTo(Type::boolType) && rtype->IsEquivalentTo(Type::boolType)) {
        this->type = Type::boolType;
        return;
    }

    if (!(ltype->IsConvertibleTo(Type::boolType)))  {
        ReportError::IncompatibleOperand(op, ltype);
        left->setType(Type::errorType);
    }

    if (!(rtype->IsConvertibleTo(Type::boolType)))  {
        ReportError::IncompatibleOperand(op, rtype);
        right->setType(Type::errorType);
    }

    this->type = Type::errorType;

}

void AssignExpr::CheckExpr() {
    //Post-order traversal
    left->CheckExpr();
    right->CheckExpr();

    Type * ltype = left->getType();
    Type * rtype = right->getType();

    //If we have two matching valid types, we know we're error free
    if (ltype->IsEquivalentTo(rtype))   {
        this->type = ltype;
        return;
    }

    if(!(ltype->IsConvertibleTo(rtype) || rtype->IsConvertibleTo(ltype)))    {
        ReportError::IncompatibleOperands(op, ltype, rtype);
    }
    
    this->type = Type::errorType;
}

void PostfixExpr::CheckExpr() { 

}

void ConditionalExpr::CheckExpr() { 

}

ConditionalExpr::ConditionalExpr(Expr *c, Expr *t, Expr *f)
  : Expr(Join(c->GetLocation(), f->GetLocation())) {
    Assert(c != NULL && t != NULL && f != NULL);
    (cond=c)->SetParent(this);
    (trueExpr=t)->SetParent(this);
    (falseExpr=f)->SetParent(this);
}

void ConditionalExpr::PrintChildren(int indentLevel) {
    cond->Print(indentLevel+1, "(cond) ");
    trueExpr->Print(indentLevel+1, "(true) ");
    falseExpr->Print(indentLevel+1, "(false) ");
}
ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this); 
    (subscript=s)->SetParent(this);
}

void ArrayAccess::PrintChildren(int indentLevel) {
    base->Print(indentLevel+1);
    subscript->Print(indentLevel+1, "(subscript) ");
}
     
FieldAccess::FieldAccess(Expr *b, Identifier *f) 
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
    Assert(f != NULL); // b can be be NULL (just means no explicit base)
    base = b; 
    if (base) base->SetParent(this); 
    (field=f)->SetParent(this);
}


void FieldAccess::PrintChildren(int indentLevel) {
    if (base) base->Print(indentLevel+1);
    field->Print(indentLevel+1);
}

Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) base->SetParent(this);
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
}

void Call::PrintChildren(int indentLevel) {
   if (base) base->Print(indentLevel+1);
   if (field) field->Print(indentLevel+1);
   if (actuals) actuals->PrintAll(indentLevel+1, "(actuals) ");
}

