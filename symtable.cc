/*
 * Symbol table implementation
 *
 */
#include "symtable.h"

using namespace std;

  int SymbolTable::loops = 0;
  int SymbolTable::switches = 0; 
  bool SymbolTable::needReturn = false;

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

  inline vector<Decl*> find(map<string, Decl*> m, string key)   {
      vector<Decl*> ret;
      for (map<string, Decl*>::const_iterator it = m.begin(); it != m.end(); it++)  {
          if (!(key.compare(it->first)))    {
              ret.insert(ret.begin(), it->second);
          }
      }
      return ret;
  }

  vector<Decl*> SymbolTable::findInCurrScope(string key) {
      return find(scope_vector.back(), key);
  }
  /*
      map<string, Decl*>::const_iterator it = scope_vector.back().find(key);
      vector<Decl*> ret;
      while (it != scope_vector.back().end())   {
          ret.push_back(it->second);
          it++;
      }
      return ret;
  }
*/
    vector<Decl*> SymbolTable::findInAnyScope(string key) {
      vector<Decl*> ret;
      for (vector<map<string, Decl*> >::const_iterator i = scope_vector.begin(); i != scope_vector.end(); i++)  {
          vector<Decl*> results = find(*i, key);
          vector<Decl*> temp (ret);
          ret.clear();
          ret.reserve(temp.size() + results.size());
          ret.insert(ret.begin(), temp.begin(), temp.end());
          ret.insert(ret.end(), results.begin(), results.end());
      }
      return ret;
    }
    /*
        map<string, Decl*>::const_iterator it = i->find(key);
        while (it != i->end())   {
          ret.push_back(it->second);
          cout << "Found " << it->second << " whose key is " << it->first << endl;
          it++;
        }
      }
      return ret;
  }
*/
