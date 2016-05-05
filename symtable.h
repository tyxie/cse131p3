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
#include <string>
#include "ast.h"

using namespace std;
class Decl;
class SymbolTable  {
    private:
        vector <map <string, Decl*> > scope_vector;
    public:
        SymbolTable();
        void push();
        map<string, Decl*> pop();
        map<string, Decl*> top() {return scope_vector.back();};
        void addsym(string id, Decl* node);
        vector<Decl*> findInCurrScope(string key);
        friend ostream& operator<<(ostream& out, SymbolTable *st) {
            out << endl << "-- Base Scope --" << endl << endl;
            for (vector<map <string, Decl*> >::const_iterator it = st->scope_vector.begin(); it != st->scope_vector.end(); ++it)  {
                map<string, Decl*> curr_st = *it;
                for (map<string, Decl*>::const_iterator st_iter = curr_st.begin(); st_iter != curr_st.end(); st_iter++) {
                    out << "Key: " << st_iter->first << "\t\t" << "Value: " << st_iter->second << endl;
                }
            out << endl << "-- Next Scope --" << endl << endl;
            }
            return out << endl << "END SYMBOL TABLE" << endl;
        }
};

#endif
