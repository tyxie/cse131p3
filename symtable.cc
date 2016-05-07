/*
 * Symbol table implementation
 *
 */
#include "symtable.h"

using namespace std;

  SymbolTable::SymbolTable()    {
      this->push();
  }
  
  void SymbolTable::push()
  {
    map<string, Decl*> newscope;
    scope_vector.push_back(newscope); 
  }

  map<string, Decl*> SymbolTable::pop()
  {
    map<string, Decl*> tempmap = scope_vector.back();

    scope_vector.pop_back(); 

    return tempmap; 
  }

  void SymbolTable::addsym(string id, Decl* node)
  {
    scope_vector.back().insert(std::pair<string,Decl*>(id,node)); 
  }

  vector<Decl*> SymbolTable::findInCurrScope(string key) {
      map<string, Decl*>::const_iterator it = scope_vector.back().find(key);
      vector<Decl*> ret;
      while (it != scope_vector.back().end())   {
          ret.push_back(it->second);
          it++;
      }
      return ret;
  }

    vector<Decl*> SymbolTable::findInAnyScope(string key) {
      vector<Decl*> ret;
      for (vector<map<string, Decl*> >::const_iterator i = scope_vector.begin(); i != scope_vector.end(); i++)  {
        map<string, Decl*>::const_iterator it = i->find(key);
        while (it != i->end())   {
          ret.push_back(it->second);
          it++;
        }
      }
      return ret;
  }

