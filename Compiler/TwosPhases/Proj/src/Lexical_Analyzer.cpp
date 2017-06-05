//
// Created by Paula B. Bassily on 24/03/2017.
//

#include <set>
#include <iostream>
#include <fstream>
#include "Lexical_Analyzer.h"

namespace std
{

/**
*   params : first minimized dfa_state
*   iterates over all strings to analyze the tokens and their lexemes
*   return void
*/
void Lexical_Analyzer::analyze(DFA::DFA_STATE start)
{
    init_lexems();
    bool found;
    int current_state_index=0;
    char cur_char;

    string lexeme="";
    string cur_string;

    DFA::DFA_STATE *current_state =&start;
    vector<DFA::DFA_STATE*> prev_states;

    for(int i=0; i<code_strs.size(); i++)
    {

        cur_string=code_strs[i];

        /*iterate on chars in the code strings*/
        for( int j=0; j<cur_string.size(); j++)
        {
            cur_char=cur_string[j];
            prev_states.push_back(current_state);
            pair<int,char> p(current_state_index,cur_char);

            found=false;
            /* check if cur_state goes to another state on this transition  function(cur_char) */
            if(dfa_network.find(p) != dfa_network.end())
            {
                found=true;
                current_state=&dfa_network.find(p)->second;
                current_state_index=current_state->number;
                lexeme+=cur_char;
            }
            /* if cur_char can't be reached or the strings reached its max */
            if(!found || j== cur_string.size()-1)
            {
                DFA::DFA_STATE *dum=current_state;
                while(true)
                {
                    /* If the state was an acceptance state */
                    if(string(dum->acceptance).size()!=0)
                    {
                        pair<string,string> token(string(dum->acceptance),lexeme);
                        tokens.push_back(token);
                        if(string(dum->acceptance)=="id")
                            symbol_table.insert(pair<string,string>(lexeme,"0"));
                        if(!found)
                            j--;
                        break;
                    }
                    else
                    {
                        prev_states.erase(prev_states.end()-1,prev_states.end());
                        if(prev_states.size()==0)
                        {
                            lexeme+=cur_char;
                            pair<string,string> token(ERROR,lexeme);
                            tokens.push_back(token);
                            break;
                        }
                        dum=prev_states[prev_states.size()-1];
                        lexeme = lexeme.substr(0, lexeme.size()-1);
                        j--;
                    }
                }
                /*reset to the start state*/
                lexeme="";
                prev_states.clear();
                current_state=&start;
                current_state_index=0;
            }
        }// end of on string
    }// end of all strings
    /* write to output file */
    write_output_file();
}

/**
*   Constructor
*/
Lexical_Analyzer::Lexical_Analyzer(map<pair<int,char>,DFA::DFA_STATE> dfa_network)
{
    this->dfa_network=dfa_network;
}

/**
*     Opens the input code file and parse in into strings
*/
void Lexical_Analyzer::init_lexems()
{
    ifstream infile("test_code.txt");
    string word;
    char x ;
    word.clear();

    while (infile>>word)
    {
        x = infile.get();
        while ( x != ' '&&x != '\n'&&x != '\t' )
        {
            word = word + x;
            x = infile.get();
        }
        code_strs.push_back(word);
        word.clear();
    }
    infile.close();
}

/**
*    Writes the output file from the list of tokens
*/
void Lexical_Analyzer::write_output_file()
{
    ofstream outfile;
    outfile.open("tokens.txt");
    pair<string,string> token;
    for(int i=0; i<tokens.size(); i++)
    {
        token=tokens[i];
        outfile<<token.first<<endl;
    }
    outfile.close();
}

}
