//
// Created by Paula B. Bassily on 24/03/2017.
//

#include<string>
#include <map>
#include <vector>
#include "DFA.h"

#ifndef COMPILERS_PHASE1_LEXICAL_ANALYZER_H
#define COMPILERS_PHASE1_LEXICAL_ANALYZER_H



namespace std
{
class Lexical_Analyzer
{
public :

    Lexical_Analyzer(map<pair<int,char>,DFA::DFA_STATE> dfa_network);

    void analyze(DFA::DFA_STATE start);

private:

    const string ERROR = "invalid expression";

    vector<string> code_strs;
    vector<pair<string,string> > tokens;
    map <string,string> symbol_table;
    map<pair<int,char>,DFA::DFA_STATE> dfa_network;

    void init_lexems();
    void write_output_file();


};
}


#endif //COMPILERS_PHASE1_LEXICAL_ANALYZER_H
