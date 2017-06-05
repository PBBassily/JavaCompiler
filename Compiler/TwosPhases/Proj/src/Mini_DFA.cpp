#include "../include/Mini_DFA.h"


namespace std
{

/**
 * sets the initial groups
 */
void Mini_DFA::set_initial_grp()
{

    int group_count = 0;
    for(int i=0; i < states.size(); i++)
    {
        string accept =string(states[i]->acceptance);
        if(group_setter.find(accept) != group_setter.end())
        {
            // if same group
            int group=group_setter.find(accept)->second;
            ith_state_group.push_back(group);
        }
        else
        {
            // if new group
            group_setter.insert(pair<string,int>(accept,group_count));
            ith_state_group.push_back(group_count);
            group_count++;

        }
    }
    for(int i=0; i<group_count; i++)
    {
        vector<int>dummy_vector;
        groups.push_back(dummy_vector);
    }
    for(int i=0; i<states.size(); i++)
    {
        int group=ith_state_group[i];
        groups[group].push_back(i);
    }
}

/**
 *  minimize the created groups
 */
void Mini_DFA::minimize()
{
    do
    {
        need_more_grps=false;
        for(int i=0; i<states.size(); i++)
        {
            generate_nxt_group(i);
        }
        re_group();
    }
    while(need_more_grps);
}

/**
  *   generate the next groups
  */
void Mini_DFA:: generate_nxt_group(int i)
{
    vector<int> dummy_vector;
    for(int j=0; j<INPUT_SIZE; j++)
    {
        int nxt_state=subset_construction[i][j];
        if(nxt_state==-2)
        {
            dummy_vector.push_back(groups.size());
        }
        else
        {
            dummy_vector.push_back(ith_state_group[nxt_state]);
        }

    }
    next_grp_ident.push_back(dummy_vector);
    is_taken.push_back(false);

}

/**
  *   create new groups for the ones that
  *   need to be split
  */
void Mini_DFA::re_group()
{
    int nxt_group_count=0;
    for(int grp_index=0; grp_index<groups.size(); grp_index++)
    {

        for(int i=0; i<groups[grp_index].size(); i++)
        {
            int current_state=groups[grp_index][i];
            if(is_taken[current_state] ==true)
            {
                continue;
            }
            // create new group
            vector<int> new_grp;
            new_grp.push_back(current_state);
            nxt_groups.push_back(new_grp);
            // update it in the current group
            ith_state_group[current_state]=nxt_group_count;

            // for each group
            for(int j=i+1; j<groups[grp_index].size(); j++)
            {

                int sibling_state=groups[grp_index][j];
                // is it omitted before ?
                if(is_taken[sibling_state] ==true)
                {
                    continue;
                }
                bool flag=compare_nxt_grp_idents(current_state,sibling_state);

                if(flag)
                {
                    nxt_groups[nxt_group_count].push_back(sibling_state);
                    ith_state_group[sibling_state]=nxt_group_count;
                    // omit this state from subset table
                    is_taken[sibling_state] =true;
                }
                else
                {
                    need_more_grps=true;
                }

            }
            // increment group
            nxt_group_count++;
        }

    }
    groups=nxt_groups;
    if(need_more_grps)
    {

        nxt_groups.clear();
        next_grp_ident.clear();
        is_taken.clear();
    }
}

/**
  *   compares the next states of two states st1 and st2
  *   for all inputs
  */

bool Mini_DFA::compare_nxt_grp_idents(int st1,int st2)
{
    for(int i=0; i<INPUT_SIZE; i++)
    {
        if(next_grp_ident[st1][i]!=next_grp_ident[st2][i])
        {
            return false;
        }
    }
    return true;
}

/**
  *  generate the minimum DFA
  */
void  Mini_DFA::get_mini_dfa()
{
    set_initial_grp();
    minimize();

    map<int,int> delegate;
    for(int i=0; i<states.size(); i++)
    {
        int ith_group = ith_state_group[i];
        int delegate_state=groups[ith_group][0];
        delegate.insert(pair<int,int>(i,delegate_state));

    }
    for(int i=0; i<states.size(); i++)
    {
        for(int j=0; j<INPUT_SIZE; j++)
        {
            char input=INPUT[j];
            int nxt_state_index=subset_construction[i][j];
            if (nxt_state_index==-2)
                continue;
            nxt_state_index=delegate.find(nxt_state_index)->second;
            DFA::DFA_STATE nxt_state=*states[nxt_state_index];
            pair<int,char> p(i,input);
            dfa_network.insert(pair<pair<int,char>,DFA::DFA_STATE>(p,nxt_state));
        }
    }

}

Mini_DFA::Mini_DFA(vector< int * > subset_construction,vector<DFA::DFA_STATE*> states)
{
    this->subset_construction=subset_construction;
    this->states=states;
}

/**
*    Writes the Transition table file
*/
void Mini_DFA::write_trans_file()
{
    ofstream trans_file;
    trans_file.open("trans_table.txt");
    for (map<pair<int,char>,DFA::DFA_STATE>::iterator it=dfa_network.begin(); it!=dfa_network.end(); ++it)
    {
        struct DFA::DFA_STATE s = it->second;
        pair <int,char> temp=it->first;
        int from_state =get<0> (temp);
        char input = get<1> (temp);
        int to_state = it->second.number;
        trans_file << "From "<<from_state<< " (input= " << input << "): " << to_state << s.acceptance<<endl;

    }
    trans_file.close();
}

Mini_DFA::~Mini_DFA()
{
    //dtor
}
}
