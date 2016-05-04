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
    map<char*, Decl*> newscope;
    scope_vector.push_back(newscope); 
  }

  map<char*, Decl*> SymbolTable::pop()
  {
    map<char*, Decl*> tempmap = scope_vector.back(); 

    scope_vector.pop_back(); 

    return tempmap; 
  }

  void SymbolTable::addsym(char* id, Decl* node)
  {
    scope_vector.back().insert(std::pair<char*,Decl*>(id,node)); 
  }
