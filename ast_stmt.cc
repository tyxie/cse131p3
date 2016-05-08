/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "errors.h"
#include "symtable.h"

Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    printf("\n");
}

void Program::Check() {
    /* pp3: here is where the semantic analyzer is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, examining all constructs for compliance
     *      with the semantic rules.  Each node can have its own way of
     *      checking itself, which makes for a great use of inheritance
     *      and polymorphism in the node classes.
     */

    // sample test - not the actual working code
    // replace it with your own implementation
    if ( decls->NumElements() > 0 ) {
      for ( int i = 0; i < decls->NumElements(); ++i ) {
        Decl *d = decls->Nth(i);
        d->Check();
      }
    }
}
/*
inline BreakStmt* bs(Stmt* s)
{
  return dynamic_cast<BreakStmt*>(s); 
}

inline SwitchStmt* ss(Stmt* s)
{
  return dynamic_cast<SwitchStmt*>(s); 
}

inline SwitchLabel* sl(Stmt* s)
{
  return dynamic_cast<SwitchLabel*>(s); 
}

inline ContinueStmt* cs(Stmt* s)
{
  return dynamic_cast<ContinueStmt*>(s); 
}

inline ReturnStmt* rs(Stmt* s)
{
  return dynamic_cast<ReturnStmt*>(s); 
}

inline ConditionalStmt* cos(Stmt* s)
{
  return dynamic_cast<ConditionalStmt*>(s);
}

inline StmtBlock* sb(Stmt* s)
{
  return dynamic_cast<StmtBlock*>(s);
}

inline DeclStmt* ds(Stmt* s)
{
  return dynamic_cast<DeclStmt*>(s); 
}

inline Stmt* cast(Stmt* s)
{
  if(BreakStmt *breakstmt = bs(s))
  {
    return breakstmt; 
  }
  else if(SwitchStmt *switchstmt = ss(s))
  {
    return switchstmt; 
  }
  else if(SwitchLabel *switchlabel = sl(s))
  {
    return switchlabel; 
  }
  else if(ContinueStmt *continuestmt = cs(s))
  {
    return continuestmt; 
  }
  else if(ReturnStmt *returnstmt = rs(s))
  {
    return returnstmt; 
  }
  else if(ConditionalStmt *condstmt = cos(s))
  {
    return condstmt; 
  }
  else if(StmtBlock* stmtblock = sb(s))
  {
    //return stmtblock; 
  }
  else if(DeclStmt* declstmt = ds(s))
  {
    //return declstmt; 
  }
  return NULL; 
}
*/
void Stmt::Check() 
{
  this->CheckStmt(); 
}
/*
inline LoopStmt* ls(ConditionalStmt* cons)
{
  return dynamic_cast<LoopStmt*>(cons); 
}

inline IfStmt* ifs (ConditionalStmt *cons)
{
  return dynamic_cast<IfStmt*>(cons);
}

inline ConditionalStmt* cscast(ConditionalStmt *cons)
{
  if(LoopStmt *loopstmt = ls(cons))
  {
    return loopstmt; 
  }
  else if(IfStmt *ifstmt = ifs(cons))
  {
    return ifstmt; 
  }
  return NULL; 
}

inline ForStmt* fs (LoopStmt *loop)
{
  return dynamic_cast<ForStmt*>(loop);
}

inline WhileStmt* ws (LoopStmt *loop)
{
  return dynamic_cast<WhileStmt*>(loop);
}

inline LoopStmt* loopcast(LoopStmt* ls)
{
  if(ForStmt *forstmt = fs(ls))
  {
    return forstmt; 
  }
  else if(WhileStmt *whilestmt = ws(ls))
  {
    return whilestmt; 
  }
  return NULL; 
}

void LoopStmt::CheckStmt()
{  
  loopcast(this) -> CheckStmt(); 
}
*/
void ForStmt::CheckStmt()
{
  Node::symtab->push();
  Node::loops++; 

  if(init != NULL)
  {
    init -> CheckExpr();
  }

  //TODO: test whether or not test is a boolean type if not throw error
  if(test != NULL)
  { 
    test -> CheckExpr();
    if(!test->getType()->IsEquivalentTo(Type::boolType))   
    {
      ReportError::TestNotBoolean(test); 
    } 
  }

  if(step != NULL)
  {
/*    if(!test->getType()->IsEquivalentTo(Type::boolType))   
    {
      ReportError::TestNotBoolean(test); 
    }  */

    step -> CheckExpr(); 
  }

  if(body != NULL)
  {
    body -> CheckStmt(); 
  }

  Node::loops--;
  Node::symtab->pop(); 
}

void WhileStmt::CheckStmt()
{

  Node::symtab->push();
  Node::loops++;

  //TODO: Check if test is of type boolean if not throw error
  if(test != NULL)
  {
    test -> CheckExpr(); 
    if(!test->getType()->IsEquivalentTo(Type::boolType))   
    {
      ReportError::TestNotBoolean(test); 
    } 
  }

  if(body != NULL)
  {
    body -> CheckStmt(); 
  }

  Node::loops--;
  Node::symtab->pop(); 
}
/*
void ConditionalStmt::CheckStmt()
{
  cscast(this) -> CheckStmt(); 
}
*/
void IfStmt::CheckStmt()
{
  Node::symtab->push();

  if(test != NULL)
  {  
    test -> CheckExpr(); 
    if(!test->getType()->IsEquivalentTo(Type::boolType))   
    {
      ReportError::TestNotBoolean(test); 
    }  
  }
  
  // TODO ADD the IFSTMT UNDER STMT as well  
  if(body != NULL)
  {
    body -> CheckStmt(); 
  }
  Node::symtab->pop();

  Node::symtab->push();
  if(elseBody != NULL)
  {
    elseBody -> CheckStmt(); 
  }
  Node::symtab->pop(); 
}


void ReturnStmt::CheckStmt()
{
  if(expr != NULL)
  {
    expr -> CheckExpr(); 

    if(expr->getType() != NULL && !expr->getType()->IsEquivalentTo(returnType->GetType()))
    {
      ReportError::ReturnMismatch(this, expr->getType(), returnType->GetType());
    }
    
  }
  else
  {
    if(Node::needReturn)
    {
       
       ReportError::ReturnMismatch(this, Type::voidType, returnType->GetType());     
      //needs a return value but return does not provide one
      /// ReportError::ReturnMismatch(); 
    }
  }
}

void ContinueStmt::CheckStmt()
{
  //check if inside the scope of a loop, if not throw outsideloop error
  if(Node::loops == 0)
  {
    ReportError::ContinueOutsideLoop(this); 
  }

}

void BreakStmt::CheckStmt()
{
  //check if inside the scope of a loop, if not throw outsideloop error	
  if(Node::loops == 0 && Node::switches == 0)
  {
    ReportError::BreakOutsideLoop(this); 
  }
}

void SwitchStmt::CheckStmt()
{
  Node::symtab->push(); 
  Node::switches++;

  if(expr != NULL)
  {
    expr -> CheckExpr(); 
  }

  if(cases != NULL)
  {
    for(int numCases = 0; numCases < (cases->NumElements()); numCases++)
    {
      cases->Nth(numCases)->CheckStmt(); 
    }
  }

  if(def != NULL)
  {
    def -> CheckStmt(); 
  }

  Node::switches--;
  Node::symtab->pop(); 
}
/*
inline Case* c(SwitchLabel* swla)
{
  return dynamic_cast<Case*>(swla); 
}

inline Default* d(SwitchLabel* swla)
{
  return dynamic_cast<Default*>(swla); 
}

inline SwitchLabel* slcast(SwitchLabel* swla)
{
  if(Case *ca = c(swla))
  {
    return ca; 
  }
  else if(Default *de = d(swla))
  {
    return de; 
  }
  return NULL; 
}

void SwitchLabel::CheckStmt()
{
  slcast(this) -> CheckStmt();  
}
*/
void Case::CheckStmt()
{
  if(label != NULL && stmt != NULL)
  {
      label -> CheckExpr(); 
      stmt -> CheckStmt(); 
  }
}

void Default::CheckStmt()
{
  if(stmt != NULL)
  {
    stmt -> CheckStmt(); 
  }
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}

void StmtBlock::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    stmts->PrintAll(indentLevel+1);
}

void StmtBlock::CheckStmt() {
    for (int i = 0; i < stmts->NumElements(); i++)  {
        Stmt* s = stmts->Nth(i);
        s->Check();
    }

    int numReturns = 0; 
    for(int x = 0; x < stmts->NumElements(); x++)
    {
      if(ReturnStmt *rest = dynamic_cast<ReturnStmt*>(stmts->Nth(x)))
      {
        numReturns++; 
      }
    }

    if(Node::needReturn && numReturns == 0)
    {
      ReportError::ReturnMissing(Node::returnType);  
    }
}

DeclStmt::DeclStmt(Decl *d) {
    Assert(d != NULL);
    (decl=d)->SetParent(this);
}

void DeclStmt::PrintChildren(int indentLevel) {
    decl->Print(indentLevel+1);
}

void DeclStmt::CheckStmt() {
    decl->Check();    
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}

//void ConditionalStmt::CheckStmt() {}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && b != NULL);
    (init=i)->SetParent(this);
    step = s;
    if ( s )
      (step=s)->SetParent(this);
}

void ForStmt::PrintChildren(int indentLevel) {
    init->Print(indentLevel+1, "(init) ");
    test->Print(indentLevel+1, "(test) ");
    if ( step )
      step->Print(indentLevel+1, "(step) ");
    body->Print(indentLevel+1, "(body) ");
}

void WhileStmt::PrintChildren(int indentLevel) {
    test->Print(indentLevel+1, "(test) ");
    body->Print(indentLevel+1, "(body) ");
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}

void IfStmt::PrintChildren(int indentLevel) {
    if (test) test->Print(indentLevel+1, "(test) ");
    if (body) body->Print(indentLevel+1, "(then) ");
    if (elseBody) elseBody->Print(indentLevel+1, "(else) ");
}


ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    expr = e;
    if (e != NULL) expr->SetParent(this);
}

void ReturnStmt::PrintChildren(int indentLevel) {
    if ( expr ) 
      expr->Print(indentLevel+1);
}


SwitchLabel::SwitchLabel(Expr *l, Stmt *s) {
    Assert(l != NULL && s != NULL);
    (label=l)->SetParent(this);
    (stmt=s)->SetParent(this);
}

SwitchLabel::SwitchLabel(Stmt *s) {
    Assert(s != NULL);
    label = NULL;
    (stmt=s)->SetParent(this);
}

void SwitchLabel::PrintChildren(int indentLevel) {
    if (label) label->Print(indentLevel+1);
    if (stmt)  stmt->Print(indentLevel+1);
}


SwitchStmt::SwitchStmt(Expr *e, List<Stmt *> *c, Default *d) {
    Assert(e != NULL && c != NULL && c->NumElements() != 0 );
    (expr=e)->SetParent(this);
    (cases=c)->SetParentAll(this);
    def = d;
    if (def) def->SetParent(this);
}

void SwitchStmt::PrintChildren(int indentLevel) {
    if (expr) expr->Print(indentLevel+1);
    if (cases) cases->PrintAll(indentLevel+1);
    if (def) def->Print(indentLevel+1);
}

