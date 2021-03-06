/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "symtable.h"
#include "errors.h"

Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
}

inline VarDecl* varcast(Decl* d)    {
    return dynamic_cast<VarDecl*>(d);
}

inline FnDecl* fncast(Decl* d)    {
    return dynamic_cast<FnDecl*>(d);
}



void Decl::Check()  {

    this->CheckDecl(); 
    vector<Decl*> matches = Node::symtab->findInCurrScope(this->id->GetName());
    this->CheckDecl(matches);
}

void VarDecl::CheckDecl()
{
/*  if(type != NULL)
  {
  }
  if(typeq != NULL)
  {
  } */ 

  if(assignTo != NULL)
  {
    assignTo -> CheckExpr();
    if (!(assignTo->getType()->IsConvertibleTo(this->GetType())))    {
        ReportError::InvalidInitialization(this->id, this->GetType(), assignTo->getType());
    }
  }
}

void FnDecl::CheckDecl()
{

  //vector<Expr*> returnExprs; 
 // bool hasReturn = false; 

  Node::symtab->push(); 
 
  if(formals != NULL)
  {
    for(int numFormals = 0; numFormals < (formals->NumElements()); numFormals++)
    {
      formals->Nth(numFormals)-> CheckDecl(); 

      if(!formals->Nth(numFormals)->GetType()->IsEquivalentTo(Type::errorType))
      {
        Node::symtab-> addsym(formals->Nth(numFormals)->GetIdentifier()->GetName(), formals->Nth(numFormals));
      } 
    }
  }

  if(returnType != NULL)
  {
    if(!returnType->IsEquivalentTo(Type::voidType))
    {
      Node::symtab->needReturn = true; 
      Node::returnType = this; 
     /* //signal a return is needed
      if(!hasReturn)
      {
        ReportError::ReturnMissing(this); 
      }*/
    }
    else
    {
      Node::symtab->needReturn = false; 
      Node::returnType = this; 
      //IT IS A VOID FUNCTION, CAN't HAVE RETURN VALUES; 
    }
  }

  if(body != NULL)
  {
//    Node::symtab->push();
    body -> Check();
//    Node::symtab->pop();
  /*  if(StmtBlock *block = dynamic_cast<StmtBlock*>(body))
    {
      if(block->stmts!=NULL)
      {
        for(int rts = 0; rts < (block->stmts->NumElements()); rts++)
        {
          if(ReturnStmt *restmt = dynamic_cast<ReturnStmt*>((block->stmts)->Nth(rts)))
          {
            hasReturn = true; 
            //returnExprs.push_back(restmt->expr);
          }
        }
      }
    }*/
  }  

  Node::symtab->pop(); 
}

VarDecl::VarDecl(Identifier *n, Type *t, Expr *e) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
    if (e) (assignTo=e)->SetParent(this);
    typeq = NULL;
}

VarDecl::VarDecl(Identifier *n, TypeQualifier *tq, Expr *e) : Decl(n) {
    Assert(n != NULL && tq != NULL);
    (typeq=tq)->SetParent(this);
    if (e) (assignTo=e)->SetParent(this);
    type = NULL;
}

VarDecl::VarDecl(Identifier *n, Type *t, TypeQualifier *tq, Expr *e) : Decl(n) {
    Assert(n != NULL && t != NULL && tq != NULL);
    (type=t)->SetParent(this);
    (typeq=tq)->SetParent(this);
    if (e) (assignTo=e)->SetParent(this);
}
  
void VarDecl::PrintChildren(int indentLevel) { 
   if (typeq) typeq->Print(indentLevel+1);
   if (type) type->Print(indentLevel+1);
   if (id) id->Print(indentLevel+1);
   if (assignTo) assignTo->Print(indentLevel+1, "(initializer) ");
}

void VarDecl::CheckDecl(vector<Decl*> matches)   {
    for(vector<Decl*>::const_iterator it = matches.begin(); it != matches.end(); it++)    {
         if (varcast(*it) || fncast(*it))   {
            ReportError::DeclConflict(this, *it);
            return;
        }
    }
 //   cout<< "added "<< this->GetType() << this->id->GetName() << this->GetLocation()->first_line << endl; 
    Node::symtab->addsym(this->id->GetName(), this);
}

FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
    returnTypeq = NULL;
}

FnDecl::FnDecl(Identifier *n, Type *r, TypeQualifier *rq, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r != NULL && rq != NULL&& d != NULL);
    (returnType=r)->SetParent(this);
    (returnTypeq=rq)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
}

void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
}

void FnDecl::PrintChildren(int indentLevel) {
    if (returnType) returnType->Print(indentLevel+1, "(return type) ");
    if (id) id->Print(indentLevel+1);
    if (formals) formals->PrintAll(indentLevel+1, "(formals) ");
    if (body) body->Print(indentLevel+1, "(body) ");
}

void FnDecl::CheckDecl(vector<Decl*> matches)    {
    for(vector<Decl*>::const_iterator it = matches.begin(); it != matches.end(); it++)    {

        if (FnDecl *fd = dynamic_cast<FnDecl*>(*it))
	{
            ReportError::DeclConflict(this, *it);
            return;
        }
        else if(VarDecl *varde = dynamic_cast<VarDecl*>(*it))
        {
            ReportError::DeclConflict(this, *it);
            return;
        }
    }
    Node::symtab->addsym(this->id->GetName(), this);
/*    if (body)   {
        Node::symtab->push();
        body->Check();
        Node::symtab->pop();
    }*/
}
