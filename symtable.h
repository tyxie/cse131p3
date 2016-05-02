/**
 * File: symtable.h
 * ----------- 
 *  Header file for Symbol table implementation.
 */

#include <map>
#include <stack>

using namespace std;

class SymTab    {
    private:
        stack <map <char*, Node*> > scope_stack;
    public:
        void push(map<char*, Node*> newscope);
        map<char*, Node*> pop();
        void addsym(char* id, Node* node);
};
