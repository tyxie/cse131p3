/**
 * File: symtable.h
 * ----------- 
 *  Header file for Symbol table implementation.
 */

#ifndef SYM_H_
#define SYM_H_

#include <map>
#include <iostream>
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
        friend ostream& operator<<(ostream& out, SymbolTable *st) {
            for (vector<map <char*, Decl*> >::const_iterator it = st->scope_vector.begin(); it != st->scope_vector.end(); ++it)  {
                out << endl << "-- Next Scope --" << endl << endl;
                map<char*, Decl*> curr_st = *it;
                for (map<char*, Decl*>::const_iterator st_iter = curr_st.begin(); st_iter != curr_st.end(); st_iter++) {
                    out << "Key: " << st_iter->first << "\t\t" << "Value: " << st_iter->second << endl;
                }
            }
            return out;
        }
};

#endif
