#include "Matching.h"
#define Epsilon_index -2
#define sync -3

using namespace std ;

// read from file from lexical
void Matching::get_next_token()
{
    string line ;
    ifstream infile("tokens.txt");
    while(infile >> line)
    {
        if (line.compare("invalid expression") !=0)
        {
            if(line == "assign")
            {
                tokens.push_back("=");
            }
            tokens.push_back(line);
        }
    }
    // -1 mean $
    tokens.push_back("-1");
}

void Matching ::match_input()
{
    get_next_token();

    int i=0;
    vector<int> emp;
    rules.push(-1); // push $
    rules.push(0); // push start non_terminal
    while(true)
    {
        int top_stack = rules.top();
        if (top_stack == -1)
        {
            if(tokens[i]== "-1")
            {
                // stop and accept
                derivatins_steps.push_back(std::make_pair("Accept",emp));
                break;
            }
            else
            {
                // stack is finshed but not input
                derivatins_steps.push_back(std::make_pair("Error still $ : stack finished ",emp));
                break;
            }
        }
        struct PR_ELEM LHS = elements [top_stack] ;
        if (LHS.st == tokens[i])   // terminal
        {
            derivatins_steps.push_back(std::make_pair("match :: " + LHS.st,emp));
            rules.pop();// pop from stack
            i++;// move to next token
        }
        else if (!(LHS.terminal))  // not terminal
        {
            pair <int,string> index_table= std::make_pair(top_stack,tokens[i]);
            if(parsing_table.find(index_table)!= parsing_table.end())
            {
                vector <int> prod_entry =parsing_table.find(index_table)->second;
                if (prod_entry[0]==Epsilon_index)
                {
                    // pop non_terminal from stack
                    derivatins_steps.push_back(std::make_pair(LHS.st,prod_entry));
                    rules.pop();
                }
                else if (prod_entry[0]==sync)
                {
                    // sync and output error
                    rules.pop();
                    derivatins_steps.push_back(std::make_pair("Error:follow missing "+LHS.st,prod_entry));
                }
                else
                {
                    // pop non_terminal from stack and put production in stack
                    rules.pop();
                    for(int i=prod_entry.size()-1; i>=0; i--)
                    {
                        rules.push(prod_entry[i]);
                    }
                    derivatins_steps.push_back(std::make_pair(LHS.st,prod_entry));
                }
            }
            else
            {
                // error andinput is discarded
                derivatins_steps.push_back(std::make_pair("Error:(illegal "+ LHS.st +" )"+ "– discard "+ tokens[i],emp));
                i++;
            }
        }
        else if ((LHS.terminal) && (LHS.st != tokens[i]))    // error
        {
            // error
            rules.pop();
            vector<int> emp;
            derivatins_steps.push_back(std::make_pair("Error:missing "+ LHS.st,emp));
        }
    }
}
// to print content of stack
/*vector <int>Matching ::refresh_stack()
{
    vector <int> reslte_stack;
    while(!rules.empty())
    {
        reslte_stack.push_back(rules.top());
        rules.pop();
    }
    for(int i=reslte_stack.size()-1; i>=0; i--)
    {
        rules.push(reslte_stack[i]);
    }
    return reslte_stack;
}*/

void Matching ::write_file()
{
    ofstream outputFile("output.txt");
    for (int j =0 ; j< derivatins_steps.size(); j++)
    {

        vector <int>prod =derivatins_steps[j].second;
        if(!prod.empty())
        {
            if(prod[0]== sync)
            {
                outputFile << derivatins_steps[j].first << endl ;
            }
            else if(prod[0]== Epsilon_index){
                outputFile << derivatins_steps[j].first << " --> " << "Epsilon" <<endl;
            }
            else
            {
                outputFile << derivatins_steps[j].first<< " --> ";
                for(int i=0; i<prod.size(); i++)
                {
                    outputFile  << elements[prod[i]].st<< " ";
                }
                outputFile << endl;
            }
        }
        else
        {
            outputFile << derivatins_steps[j].first << endl ;
        }
    }

    outputFile.close();
}

Matching::Matching(map<pair< int, string>, vector<int> > parsing_table,vector<struct PR_ELEM> elements )
{
    this->parsing_table =parsing_table;
    this->elements=elements;
}
Matching::~Matching()
{
    //dtor
}
