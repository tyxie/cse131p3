/**
 * File: symtable.h
 * ----------- 
 *  Header file for Symbol table implementation.
 */

#ifndef SYM_H_
#define SYM_H_

#include <map>
#include <vector>
#include "ast.h"

using namespace std;
class Decl;
class SymbolTable  {
    private:
        vector <map <char*, Decl*> > scope_vector;
    public:
        SymbolTable();
        void push();
        map<char*, Decl*> pop();
        void addsym(char* id, Decl* node);
};

#endif
