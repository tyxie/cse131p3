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
class Node;
class SymbolTable  {
    private:
        vector <map <char*, Node*> > scope_vector;
    public:
        void push(map<char*, Node*> newscope);
        map<char*, Node*> pop();
        void addsym(char* id, Node* node);
};

#endif
