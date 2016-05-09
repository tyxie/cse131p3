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

    Type * ltype;
    Type * rtype;
    //Non-Unary Exprs
    if(left != NULL && right != NULL)
    {
        left->CheckExpr();
        right->CheckExpr();

        ltype = left->getType();
        rtype = right->getType();
    
        if(!(ltype->IsConvertibleTo(rtype) || rtype->IsConvertibleTo(ltype)))    {
            ReportError::IncompatibleOperands(op, ltype, rtype);
            this->type = Type::errorType;
            return;
        }
        
        if (ltype->IsError() || rtype->IsError())   {
            this->type = Type::errorType;
            return;
        }

        if (!(ltype->IsNumeric() || ltype->IsVector() || ltype->IsMatrix()))  {
            ReportError::IncompatibleOperand(op, ltype);
            this->setType(Type::errorType);
        }
        
        if (!(rtype->IsNumeric() || rtype->IsVector() || rtype->IsMatrix()))  {
            ReportError::IncompatibleOperand(op, rtype);
            this->setType(Type::errorType);
        }

        //If we have two matching valid types, we know we're error free
        if (ltype->IsEquivalentTo(rtype))   {
            this->type = ltype;
        }
    }

    //Prefix Exprs
    else if (left == NULL)  {

        right->CheckExpr();

        rtype = right->getType();

        if (!(rtype->IsNumeric() || rtype->IsVector() || rtype->IsMatrix() || rtype->IsError()))    {
            ReportError::IncompatibleOperand(op, rtype);
            this->type = Type::errorType;
        }
        else    {
            this->type = rtype;
        }
    }
    
    else    {

        left->CheckExpr();

        ltype = left->getType();

        if (!(ltype->IsNumeric() || ltype->IsVector() || ltype->IsMatrix() || ltype->IsError()))     {
            ReportError::IncompatibleOperand(op, ltype);
            this->type = Type::errorType;
        }
        else    {
            this->type = ltype;
        }
    }
}

void RelationalExpr::CheckExpr() { 
    //Post-order traversal
    left->CheckExpr();
    right->CheckExpr();

    Type * ltype = left->getType();
    Type * rtype = right->getType();

    if(!(ltype->IsConvertibleTo(rtype) || rtype->IsConvertibleTo(ltype)))    {
        ReportError::IncompatibleOperands(op, ltype, rtype);
        this->type = Type::errorType;
        return;
    }

    if (ltype->IsError() || rtype->IsError())   {
        this->type = Type::errorType;
        return;
    }

    if (!(ltype->IsNumeric()))  {
        ReportError::IncompatibleOperand(op, ltype);
        this->setType(Type::errorType);
    }

    if (!(rtype->IsNumeric()))  {
        ReportError::IncompatibleOperand(op, rtype);
        this->setType(Type::errorType);
    }

    //If we have two matching valid types, we know we're error free
    if (ltype->IsEquivalentTo(rtype))   {
        this->type = (ltype->IsEquivalentTo(Type::errorType)) ? Type::errorType : Type::boolType;
    }
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

    if(!(ltype->IsConvertibleTo(rtype) || rtype->IsConvertibleTo(ltype)))    {
        ReportError::IncompatibleOperands(op, ltype, rtype);
        this->type = Type::errorType;
        return;
    }

    if (ltype->IsError() || rtype->IsError())   {
        this->type = Type::errorType;
        return;
    }

    if (!(ltype->IsConvertibleTo(Type::boolType)))  {
        ReportError::IncompatibleOperand(op, ltype);
    }

    if (!(rtype->IsConvertibleTo(Type::boolType)))  {
        ReportError::IncompatibleOperand(op, rtype);
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
    if (ltype != NULL && ltype->IsEquivalentTo(rtype))   {
        this->type = ltype;
        return;
    }

    if(ltype != NULL && rtype != NULL)
    {
    if(!(ltype->IsConvertibleTo(rtype) || rtype->IsConvertibleTo(ltype)))    {
        ReportError::IncompatibleOperands(op, ltype, rtype);
    }
    } 
    
    this->type = Type::errorType;
}

void PostfixExpr::CheckExpr() { 
    //Post-order traversal
    left->CheckExpr();

    Type * ltype = left->getType();

    if (!(ltype->IsNumeric() || ltype->IsVector() || ltype->IsMatrix() || ltype->IsError()))  {
        ReportError::IncompatibleOperand(op, ltype);
        this->setType(Type::errorType);
    }
    else    {
        this->type = ltype;
    }
}

void ConditionalExpr::CheckExpr() { 
    //Post-order traversal
    cond->CheckExpr();
    trueExpr->CheckExpr();
    falseExpr->CheckExpr();

    Type * ctype = cond->getType();
    Type * ttype = trueExpr->getType();
    Type * ftype = falseExpr->getType();

    if (!(ctype->IsEquivalentTo(Type::boolType)))  {
        ReportError::TestNotBoolean(cond);
    }
    //The TAs sais this would not be tested

    //If we have two matching valid types, we know we're error free
    if (ttype->IsEquivalentTo(ftype))   {
        this->type = (ttype->IsEquivalentTo(Type::errorType)) ? Type::errorType : ttype;
    }
    else    {
        this->type = Type::errorType;
    }
}

void FieldAccess::CheckExpr()   {
    base->CheckExpr();
    Type * btype = base->getType();
    bool skip = false;
    if (!(btype->IsVector() || btype->IsError()))  {
        ReportError::InaccessibleSwizzle(field, base);
        skip = true;
    }

    string swizzleset("xyzw");
    string swizzle(field->GetName());
    for (int i = 0; i < swizzle.size(); i++) {
        if (swizzleset.find(swizzle[i]) == string::npos)    {
            ReportError::InvalidSwizzle(field, base);
            break;
        }
        if (!skip)   {
            char dim = swizzle[i];
            switch (dim)    {
                case 'w':
                    if (btype->IsEquivalentTo(Type::vec3Type))  {
                        ReportError::SwizzleOutOfBound(field, base);
                        return;
                    }
                case 'z':
                    if (btype->IsEquivalentTo(Type::vec2Type))  {
                        ReportError::SwizzleOutOfBound(field, base);
                        return;
                    }
                break;

                default:
                break;
            }
        }
    }
    if (swizzle.size() > 4) {
        ReportError::OversizedVector(field, base);
        this->type = Type::errorType;
    }
}

void ArrayAccess::CheckExpr()   {
    base->CheckExpr();
    Type* btype = base->getType();
    ArrayType* at = dynamic_cast<ArrayType*>(btype);
    if (at == NULL && !(btype->IsMatrix())) {
        if (VarExpr* ve = dynamic_cast<VarExpr*>(base)) {
            ReportError::NotAnArray(ve->GetIdentifier());
            this->type = base->getType();
        }
    }
    else    {
        if (btype->IsEquivalentTo(Type::mat2Type))  {
            this->type = Type::vec2Type;
        }
        else if (btype->IsEquivalentTo(Type::mat3Type))  {
            this->type = Type::vec3Type;
        }
        else if (btype->IsEquivalentTo(Type::mat4Type))  {
            this->type = Type::vec4Type;
        }
        else    {
            this->type = at->GetElemType();
        }
    }
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

void Call::CheckExpr()
{
  if(base != NULL)
  {
    base->CheckExpr(); 
  } 
  
  if(field != NULL)
  {

    for(int y = 0; y < actuals->NumElements(); y++)
    {
      actuals->Nth(y)->CheckExpr(); 
    }

    vector<Decl*> matches = Node::symtab->findInAnyScope(field->GetName());
    vector<FnDecl*> matchingFn;  

    for(vector<Decl*>::const_iterator it = matches.begin(); it != matches.end(); it++)    
    {
         if (FnDecl *fd = dynamic_cast<FnDecl*>(*it))
	 { 
           matchingFn.push_back(fd); 
    	 }
    }

    if(matchingFn.size() == 0)
    {
      ReportError::IdentifierNotDeclared(this->field, LookingForFunction); 
      this->type = Type::errorType;
    }
    else if(matchingFn.size() == 1)
    {
        FnDecl *fd = matchingFn.back(); 

        if(fd->GetFormals()->NumElements() < this->actuals->NumElements())
        {
           ReportError::ExtraFormals(field, fd->GetFormals()->NumElements(), 
						this->actuals->NumElements()); 
        } 
        else if(fd->GetFormals()->NumElements() > this->actuals->NumElements())
        {
           ReportError::LessFormals(field, fd->GetFormals()->NumElements(), 
						this->actuals->NumElements()); 
        }
        else
        {
          for(int x = 0; x < this->actuals->NumElements(); x++)
          {
            if(!fd->GetFormals()->Nth(x)->GetType()->IsEquivalentTo(this->actuals->Nth(x)->getType()) &&
              !this->actuals->Nth(x)->getType()->IsEquivalentTo(Type::errorType))
            {
              ReportError::FormalsTypeMismatch(this->field, x + 1, fd->GetFormals()->Nth(x)->GetType(), 
						this->actuals->Nth(x)->getType()); 
              this->actuals->Nth(x)->setType(Type::errorType); 
              return;
            }
          }
       }
       this->type = fd->GetType();
    }
  }
}

