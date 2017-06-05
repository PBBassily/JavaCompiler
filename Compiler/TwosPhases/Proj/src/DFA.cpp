#include "DFA.h"

using namespace std;

/**
 *  params : pointer to dfa_state , nfa_node that belongs to this dfa_node
 *              , pointer to the set of all next nfa_nodes that can be
 *               reached by epsilon
 *  checks all nodes that can be reached by epsilon and add it to the new_next set
 *  @return void
 */
void DFA::epsilon_combine(DFA_STATE *dfa_state,State cur_node,set<int> *new_next)
{
    if(cur_node.input!=' ')
        return;
    for(int i=0; i<cur_node.next.size(); i++)
    {
        new_next->insert(cur_node.next[i]);
        epsilon_combine(dfa_state,nfa_nodes[cur_node.next[i]],new_next);
    }
}


/**
 *  params : nfa_start_node_number , vector of nfa_nodes
 * fill the subset construction table and create the dfa nodes
 *  @return void
 */

void DFA::fill_subset_construction(int start_node_num, vector<State> all_nfa_nodes)
{
    int *input_match ;
    int element ;
    int dfa_state_num=0;
    bool is_input;
    bool exist;
    char nfa_input_char;

    string delimiter = "-" ;
    string start_char ;
    string end_char ;

    set<int> *the_combined;
    set <int> *temp_next;

    struct DFA_STATE* existing_state;
    struct DFA_STATE* new_state;
    struct DFA_STATE* cur_dfa_state;


    /* Start with nfa nodes */
    nfa_nodes=all_nfa_nodes;
    State node=nfa_nodes[start_node_num];

    /* Create first DFA node*/

    the_combined= new set<int>();
    the_combined->insert(node.num);
    cur_dfa_state = (struct DFA_STATE*) malloc(sizeof(struct DFA_STATE));
    cur_dfa_state->acceptance=new char();
    epsilon_combine(cur_dfa_state,node,the_combined);

    cur_dfa_state->number=dfa_state_num;
    cur_dfa_state->combined_states=the_combined;

    /* if it was acceptance*/
    dfa_mapping.push_back(cur_dfa_state);
    dfa_state_num++;


    /* iterate over all DFA states */
    for (int dfa_index =0 ; dfa_index < dfa_mapping.size() ; dfa_index++)// el size lma yt3'yar :/
    {
        input_match =new int[INPUT.length()];

        cur_dfa_state = dfa_mapping[dfa_index];

        /* iterate over all possible input characters */
        for(int input_num=0; input_num<INPUT.length(); input_num++)
        {
            the_combined =cur_dfa_state->combined_states;
            temp_next=new set<int>() ; //next state for specific input + epsilons

            /* iterate on NFA states on specific input */
            for (set<int>::iterator j = the_combined->begin(); j != the_combined->end(); j++)
            {
                element = *j;
                State temp_nfa_state =nfa_nodes[element];

                /* check if nfa_node was acceptance , set DFA nodes's acceptance */
                if( temp_nfa_state.acceptance.length()>0)
                {
                    strcpy(cur_dfa_state->acceptance,temp_nfa_state.acceptance.c_str());
                }


                is_input=false;
                nfa_input_char=temp_nfa_state.input;
                /* Check if nfa_node's input belong to one of special chars indicating ranges */
                if(sp_char_map.find(nfa_input_char)!=sp_char_map.end())
                {
                    string s=sp_char_map.find(nfa_input_char)->second;
                    s.erase(remove(s.begin(),s.end(),' '),s.end());
                    start_char = s.substr(0, s.find(delimiter));
                    end_char = s.substr( s.find(delimiter)+1,s.length());
                    if( ((int)start_char[0]<= (int)INPUT[input_num])&&((int)INPUT[input_num] <=(int)end_char[0]) )
                    {
                        is_input=true;
                    }
                }
                /* Check if the nfa_node's is the current input indexed */
                else if(nfa_input_char==INPUT[input_num])
                {
                    is_input=true;
                }

                /* Check if the nfa node has next , for this input */
                if(is_input)
                {
                    temp_next->insert(temp_nfa_state.next[0]);
                    epsilon_combine(cur_dfa_state,nfa_nodes[temp_nfa_state.next[0]],temp_next);
                }
            }

            /* New Dfa state has no next for this input */
            if(temp_next->empty())
            {
                input_match[input_num]=-2;
                continue;
            }
            exist=false;

            /* Check if DFA state already exist */
            for (int e =0 ; e < dfa_mapping.size() ; e++)
            {
                existing_state = dfa_mapping[e];
                if(*(existing_state->combined_states)==*temp_next)
                {
                    exist=true;
                    input_match[input_num]=e;
                    break;
                }
            }

            /* Create new Dfa state */
            if(!exist)
            {
                new_state = (struct DFA_STATE*) malloc(sizeof(struct DFA_STATE));
                new_state->acceptance=new char();
                new_state->number=dfa_state_num;
                new_state->combined_states=temp_next;
                input_match[input_num]= dfa_state_num;
                dfa_state_num++;
                dfa_mapping.push_back(new_state);

            }
        }
        /* Add the array of the dfa_state's next*/
        subset_construction.push_back(input_match);
    }

}
DFA::DFA(map<char,string> sp_map)
{
    sp_char_map=sp_map;
}

DFA::~DFA()
{
    //dtor
}



