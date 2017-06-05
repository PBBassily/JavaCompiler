#include"Parser.h"
using namespace std;

bool Parser::compare (char s )
{
    if (s == '+' || s == '*' || s == '^'|| s == '|'|| s == ')'|| s == '(')
    {
        return false ;
    }
    return true;
}

/**
* input char s
* return int  express on precedence
*/
int Parser::getPrecedence (char s )
{
    int Precedence ;
    switch (s)
    {
    case '|':
        Precedence = 1;
        break;
    case '^':
        Precedence = 2;
        break;
    case '*':
        Precedence = 3;
        break;
    case '+':
        Precedence = 4;
        break;
    case '(':
        Precedence = 5;
        break;
    default:
        Precedence = 0;
    }
    return Precedence ;
}
/**
* input string input , name of pattern
* return string express on postfix expression
*/
string  Parser::postfix_expression (string input )
{
    stack <char>  exp_stack;
    char  output [input.length()];
    int counter =0;
    for (int i =0 ; i< input.length() ; i++)
    {
        if((input[i] == '\\'))
        {
            if(input[i+1] == 'L')
            {
                output[counter++] =' ';
            }
            else
            {
                /*if any (\ then character)*/
                output[counter++]=input[i];
                output[counter++] = input[i+1];
            }
            i++;
        }
        else if (compare(input[i]))
        {
            /* if any letter of digits */
            output[counter++] = input[i];
        }
        else if (exp_stack.empty())
        {
            /* if operator and stack empty*/
            exp_stack.push(input[i]);
        }
        else if (input[i] ==')')
        {
            char value =exp_stack.top() ;
            while( value != '(')
            {
                output[counter++] =exp_stack.top();
                exp_stack.pop();
                value =exp_stack.top() ;
            }
            exp_stack.pop(); // to pop (
        }
        else
        {
            /* if stack is not empty*/
            int num_X = getPrecedence(input[i]);
            int num_Y =getPrecedence(exp_stack.top());
            if(num_Y == 5)
            {
                /* if ( then push */
                exp_stack.push(input[i]);
            }
            else
            {
                if(num_X > num_Y)
                {
                    exp_stack.push(input[i]);
                }
                else
                {
                    while ((!exp_stack.empty())&& ( num_X < num_Y) )
                    {
                        output[counter++] = exp_stack.top();
                        exp_stack.pop();
                        if(!exp_stack.empty()){
                           num_Y =getPrecedence(exp_stack.top());
                        }
                    }
                    exp_stack.push(input[i]);
                }
            }
        }

    }
    /* pop to empty stack*/
    while (!exp_stack.empty())
    {
        output[counter++]=exp_stack.top();
        exp_stack.pop();
    }
    output[counter++]= '\0';
    return string(output);
}


/**
* input string s from file
* if string s matched a definition  , add it to definition list
* return true if string s matches a definition
*/
bool Parser::match_def(string s)
{

    regex def_gnl_rgx("^[[:s:]]*([[:w:]]+)[[:s:]]*= (.*)");
    smatch def_gnl_match;

    if(regex_search(s, def_gnl_match, def_gnl_rgx))
    {
        // Add label to definition list
        def_array[def_count] =(char*)malloc(sizeof(char));
        sprintf(def_array[def_count], def_gnl_match.str(1).c_str());

        //Add the label's definition
        string reg_def = def_gnl_match.str(2);
        string def;
        def+="(";
        regex reg_real_def_rgx("[[:s:]]*([[:w:]]+[[:s:]]*-[[:s:]]*[[:w:]]+)|([[:w:]]+[[:s:]]*\\+)[[:s:]]*\\|?");
        sregex_iterator def_prt(reg_def.cbegin(), reg_def.cend(),reg_real_def_rgx);
        sregex_iterator end;
        for(; def_prt!=end; def_prt++)
        {
            if(!def_prt->str(1).empty())
            {
                def += special_char[special_char_count];
                sp_map.insert(pair<char,string>(special_char[special_char_count],def_prt->str(1).c_str()));
                special_char_count++;
            }
            else if(!def_prt->str(2).empty())
            {
                string sps = def_prt->str(2).substr(0, def_prt->str(2).size()-1);
                for(int i=0; i<=def_count ; i++)  // Can't be defined by itself
                {
                    if(def_prt->str(2).substr(0, def_prt->str(2).size()-1)==def_array[i])
                    {
                        sps= def_array_str[i];
                        break;
                    }
                }
                def+=sps+"+";
            }
            def += "|";

        }

        def = def.substr(0,def.size()-1);
        def+=")";
        def_array_str[def_count] =(char*)malloc(sizeof(char));
        sprintf(def_array_str[def_count], def.c_str());
        def_count++;
        return true;
    }
    return false;
}


/**
* input string s from file
* if string s matched a keyword pattern, add it to <token,lexeme> vector
* return true if string s matches a keywords pattern
*/
bool Parser::match_keywords (string s)
{

    regex reg_exp_keyword("^[[:s:]]*\\{((?:[[:s:]]*[[:w:]]+)+)[[:s:]]*\\}$");
    smatch reg_exp_keyword_match;

    if(regex_search(s, reg_exp_keyword_match, reg_exp_keyword))
    {
        string keyword_items=reg_exp_keyword_match[1].str();
        regex keyword_item_match("[[:s:]]*([[:w:]]+)");
        sregex_iterator keyword_item_match_itr(keyword_items.cbegin(), keyword_items.cend(),keyword_item_match);
        sregex_iterator end;

        for(; keyword_item_match_itr!=end; keyword_item_match_itr++)
        {
            string lk="";
            for(int p=0; p < keyword_item_match_itr->str(1).length(); p++)
            {
                lk+=keyword_item_match_itr->str(1)[p];
                lk+="^";
            }
            lk = lk.substr(0,lk.size()-1);
            string exp = postfix_expression(lk);
            pair <string,string> p (keyword_item_match_itr->str(1),exp);
            result.push_back(p);
        }
        return true;
    }

    return  false;
}


/**
* input string s from file
* if string s matches punctuations' pattern, add it to <token,lexeme> vector
* return true if string s matches punctuations' pattern
*/
bool Parser::match_punc(string s)
{
    regex punc_rgx("^[[:s:]]*\\[(.*)\\]$");
    smatch punc_match;

    if(regex_search(s, punc_match, punc_rgx))
    {
        string punc_items = punc_match.str(1);
        regex punc_item_match("[[:s:]]*(\\\\?[[:punct:]])");
        sregex_iterator punc_item_match_itr(punc_items.cbegin(), punc_items.cend(),punc_item_match);
        sregex_iterator end;
        for(; punc_item_match_itr!=end; punc_item_match_itr++)
        {
            string st="";
            for(int i=0 ; i<punc_item_match_itr->str(1).length(); i++)
            {
                st=punc_item_match_itr->str(1)[i];
            }
            string exp = postfix_expression(st);
            pair <string,string> p (st,exp);
            result.push_back(p);
        }
        return true;
    }
    return  false;
}

/**
* input string s from file
* if string s matches expression pattern, add it to <token,lexeme> vector
* return true if string s matches expression pattern
*/
bool Parser::match_reg_exp(string s)
{
    regex reg_exp_rgx("^[[:s:]]*([[:w:]]+)[[:s:]]*\\:(.*)");
    smatch reg_exp_match;

    if(regex_search(s, reg_exp_match, reg_exp_rgx))
    {
        string reg_exp = reg_exp_match.str(1);
        string reg_exp_str = reg_exp_match.str(2);
        regex reg_real_exp_rgx("[[:s:]]*|([[:w:]]+|\\\\[[:punct:]]|[[:punct:]]|\\\\L)|(\\|)|(\\()|(\\)|\\*|\\+)");

        string expp="";
        sregex_iterator pos(reg_exp_str.cbegin(), reg_exp_str.cend(),reg_real_exp_rgx);
        sregex_iterator end;
        bool p=false;
        for(; pos!=end; pos++)
        {
            if(!pos->str(1).empty())
            {
                if(p)
                    expp+="^";
                p=true;

                string h=pos->str(1);
                for(int i=0; i<def_count ; i++)  // Can't be defined by itself
                {
                    if(h==def_array[i])
                    {
                        h= def_array_str[i];
                        break;
                    }
                }
                expp+=h;
            }
            else if(!pos->str(2).empty())
            {
                p=false;
                expp+=pos->str(2);
            }
            else if(!pos->str(3).empty())
            {
                if(p)
                    expp+="^";
                expp+=pos->str(3);
                p=false;
            }
            else if(!pos->str(4).empty())
            {
                p=true;
                expp+=pos->str(4);
            }
        }
        string exp = postfix_expression(expp);
        pair <string,string> item (reg_exp,exp);
        result.push_back(item);
        return true;
    }
    return false;
}

/**
* input string s from file
* pass string to different matchers to be parsed
*/
void Parser::parse(string s)
{
    if(match_def(s))
        return;
    if(match_keywords(s))
        return;
    if(match_punc(s))
        return;
    if(match_reg_exp(s))
        return;
}

/**
* read the langauge expressions and definitions file
* return vector of pair of <string token ,string lexeme>
*/

vector <pair <string,string> > Parser::parse_file()
{
    ifstream infile("lang_specifications.txt");
    string s;

    while(getline(infile,s))
    {
        parse(s);
    }
    return result;
}

Parser::Parser() {}
