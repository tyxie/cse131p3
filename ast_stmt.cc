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
    cout << Node::symtab << endl;
    }
}

/*
inline BreakStmt* b(Stmt* s)
{
  return dynamic_cast<BreakStmt*>(s); 
}

inline SwitchLabel* sl(Stmt* s)
{
  return dynamic_cast<SwitchLabel*>(s); 
}

inline ContinueStmt* cs(Stmt* s)
{
  return dynamic_cast<ContinueStmt*>(s); 
}

inline Stmt* cast()
{
  if(BreakStmt *breakstmt = b(this))
  {
    return breakstmt; 
  }
  else if(SwitchLabel *switchlabel = sl(this))
  {
    return switchlabel; 
  }
  else if(ContinueStmt *continuestmt = cs(this))
  {
    return continuestmt; 
  }

}
*/ 

void Stmt::CheckStmt(SymbolTable *st) {

  if(BreakStmt *breakstmt = dynamic_cast<BreakStmt*>(this))
  {
    breakstmt -> CheckStmt(); 
  }
  else if(SwitchLabel *switchlabel = dynamic_cast<SwitchLabel*>(this))
  {
    switchlabel -> CheckStmt(st); 
  }
  else if(ContinueStmt *continuestmt = dynamic_cast<ContinueStmt*>(this))
  {
    continuestmt -> CheckStmt(); 
  }
  else if(ReturnStmt *returnstmt = dynamic_cast<ReturnStmt*>(this))
  {
    returnstmt -> CheckStmt(); 
  }
  else if(IfStmt *ifstmt = dynamic_cast<IfStmt*>(this))
  {
    ifstmt -> CheckStmt(); 
  }
}

void ReturnStmt::CheckStmt()
{
  if(expr != NULL)
  {
   // expr -> CheckStmt(); 
  }
}

void IfStmt::CheckStmt()
{

}

void ContinueStmt::CheckStmt()
{
  //check if inside the scope of a loop, if not throw outsideloop error
}

void BreakStmt::CheckStmt()
{
  //check if inside the scope of a loop, if not throw outsideloop error	
}

void SwitchLabel::CheckStmt(SymbolTable *st)
{
  if(Case *c = dynamic_cast<Case*>(this))
  {
    c -> CheckStmt(st); 
  }
  else if(Default *d = dynamic_cast<Default*>(this))
  {
    d -> CheckStmt(st); 
  }
}

void Case::CheckStmt(SymbolTable *st)
{
  if(label != NULL && stmt != NULL)
  {
    label -> CheckStmt(st);
    stmt -> CheckStmt(st); 
  }
}

void Default::CheckStmt(SymbolTable *st)
{
  if(stmt != NULL)
  {
    stmt -> CheckStmt(st); 
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

void ConditionalStmt::CheckStmt() {}

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

void SwitchStmt::CheckStmt() {}

