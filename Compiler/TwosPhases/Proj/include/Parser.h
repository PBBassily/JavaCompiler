#ifndef Parser_H
#define Parser_H
#include "Phase_one.h"


namespace std
{

class Parser
{

public:
    map <char,string> sp_map;
    Parser();
    vector<pair<string,string> > result;
    vector <pair <string,string> > parse_file();


private:
    /* Definitions */
    char ** def_array = (char**)malloc(5*sizeof(char*));
    int def_count=0;
    char ** def_array_str = (char**)malloc(5*sizeof(char*));
    /* Special characters */
    char special_char[5]= {'$','@','&','#','%'};
    int special_char_count=0;


    /*
    *  Parse string and match it against defintions , keywords' pattern ,
    *  punctuations' pattern and expressions
    */
    void parse(string s);
    bool match_def(string s);
    bool match_keywords (string s);
    bool match_punc(string s);
    bool match_reg_exp(string s);

    /*
    * Postfix building
    */
    bool compare (char s );
    int getPrecedence (char s );
    string postfix_expression (string input );
};
}
#endif
