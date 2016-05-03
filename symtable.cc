/*
 * Symbol table implementation
 *
 */
#include "symtable.h"

using namespace std;

  void SymTable::push(map<char*, Node*> newscope)
  {
    scope_vector.push_back(newscope); 
  }

  map<char*, Node*> SymTable::pop()
  {
    map<char*, Node*> tempmap = scope_vector.back(); 

    scope_vector.pop_back(); 

    return tempmap; 
  }

  void SymTable::addsym(char* id, Node* node)
  {
    scope_vector.back().insert(std::pair<char*,Node*>(id,node)); 
  }
