#include "ParsingTable.h"

using namespace std;


/**
* Constructor
*/
ParsingTable::ParsingTable(vector<struct PR_ELEM> all_elem,map<int, vector<vector<int>>> prod_rules )
{
    this->prod_rules = prod_rules;
    this->elements = all_elem;

    for(int k=0 ; k< this->elements.size(); k++)
    {
        struct PR_ELEM elem = elements[k];
        //cout << "\n ===>  "<< elem.st << " <===\n";
    }
}

/**
*  Destructor
*/
ParsingTable::~ParsingTable()
{
    //dtor
}


/**
* Construct Parsing table , by computing first & follow
*/
void ParsingTable::construct_parsing_table()
{
    construct_first();
    fill_owners();
    construct_follow();
    print_first_or_follow("first");
    print_first_or_follow("follow");
    print_table();

}

/**
 *  iterate over all nonterminals to compute the first list
 *  @return void
 */
void ParsingTable::construct_first()
{
    for(int i=0 ;  i<elements.size() ; i++)
    {
        if((!elements[i].terminal) && elements[i].first.empty())
            get_first(i);
    }
}

/**
 *  params : index of the NT in all elements list
 *  finds the first for a specific nonterminal , given it's index
 *  @return void
 */
void ParsingTable::get_first(int NT_index)
{
    int frst_index;
    int frst_elem;
    bool has_epsilon;

    struct PR_ELEM cur_elem=elements[NT_index];
    struct PR_ELEM term;

    vector<vector<int>> pr;
    vector <int> single_pr;

    pr = prod_rules.find(NT_index)->second ;

    /* iterates over all the production rule's parts of the current nonterminal */
    for(int j = 0; j<pr.size() ; j++)
    {
        single_pr=pr[j];
        frst_index = 0 ;
        has_epsilon= true;

        /* iterates over the single production rule's elements */
        while(frst_index < single_pr.size() && has_epsilon)
        {
            has_epsilon=false;

            /* if the first was an epsilon */
            if(single_pr[frst_index]==EPS_INDEX)
            {
                cur_elem.first.insert(EPS_INDEX);
                cur_elem.first_has_epsilon=true;
                frst_index++;
                has_epsilon=true;
                continue; // break ?
            }

            term =elements[single_pr[frst_index]]; // first element in a single production
            /* if the first was a terminal */
            if(term.terminal)
            {
                cur_elem.first.insert(single_pr[frst_index]);
                parsing_table.insert (std::make_pair(std::make_pair(NT_index,term.st), single_pr) );
            }
            /* if first was a non terminal -NT- */
            else
            {
                /* if the first NT's first wasn't computed */
                if(elements[single_pr[frst_index]].first.empty())
                    get_first(single_pr[frst_index]);
                term =elements[single_pr[frst_index]]; // hangibo ba3d mat3'yar  IMPPPPPP

                /* iterate over the first NT's first list and add to to the cur NT's first list*/
                for (set<int>::iterator k = term.first.begin(); k != term.first.end(); k++)
                {
                    frst_elem= *k; // the first in the NT
                    cur_elem.first.insert(frst_elem);

                    /* if the first has epsilon  */
                    if(frst_elem == EPS_INDEX)
                    {
                        has_epsilon=true;
                        cur_elem.first_has_epsilon=true;
                        frst_index ++;
                        continue ;
                    }
                    parsing_table.insert (std::make_pair(std::make_pair(NT_index,elements[frst_elem].st), single_pr) );
                }
            }
        }
    }
    elements[NT_index]=cur_elem;
}



/**
 *  iterate over all the productions rules
 *  to find the owners of each non terminal
 *  @return void
 */
void ParsingTable::fill_owners()
{
    struct PR_ELEM temp_element;
    vector<vector<int>> temp_prod_rule;
    vector<int> temp_single_pr;
    //search all elements
    for(int i=0; i<elements.size(); i++)
    {
        temp_element=elements[i];
        if(!temp_element.terminal)
        {
            //cout<< " ------> " << temp_element.st <<"  :  " ;
            for (std::map<int, vector<vector<int>>>::iterator it=prod_rules.begin(); it!=prod_rules.end(); ++it)
            {
                int curr_owner=it->first;
                temp_prod_rule=it->second;
                for(int j=0; j<temp_prod_rule.size(); j++)
                {
                    temp_single_pr=temp_prod_rule[j];
                    vector<int>::iterator it;
                    it = find (temp_single_pr.begin(), temp_single_pr.end(), i);
                    if (it != temp_single_pr.end())
                    {
                        //std::cout << "found ownerrrrrrrrrrrrrrrrrrr" << '\n';
                        //cout << elements[curr_owner].st << "  ";
                        temp_element.owners.push_back(curr_owner);
                    }
                }
            }
            cout << endl;
            elements[i]=temp_element;
        }
    }
}


/**
 *  iterate over all nonterminals to compute the follow list
 *  @return void
 */
void ParsingTable::construct_follow()
{
    for(int i=0 ;  i<elements.size() ; i++)
    {
        if((!elements[i].terminal) && elements[i].follow.empty())
            get_follow(i);
    }
}

/**
 *  params : index of the NT in all elements list
 *  creates the follow list  for each non terminal
 *  @return void
 */

void ParsingTable::get_follow(int NT_index)
{
    struct PR_ELEM curr_non_ter=elements[NT_index];
    set<int> follow;
    vector<vector<int>> temp_prod_rule;
    vector<int> temp_single_pr;
    struct PR_ELEM curr_owner;
    set <int> needed_first;
    set<int> needed_follow;
    //-----------------------------------here
    if(curr_non_ter.terminal)
    {
        curr_non_ter.follow.insert(NT_index);
        return;
    }

    /* if start symbol put $ */
    if(NT_index==0)
    {
        follow.insert(DOLLAR_SIGN);
        insert_to_map(NT_index,DOLLAR_SIGN); //----------->>>>
    }
    int owner_to_find;
    int pos ;
    int follow_ind;
    bool is_epsilon;
    int next;
    struct PR_ELEM next_struct;

    /* search for the symbol */
    for(int i=0; i<curr_non_ter.owners.size(); i++)
    {
        owner_to_find=(curr_non_ter.owners[i]);

        //i am the owner of my self
        if(owner_to_find==NT_index)
        {
            continue;
        }

        if(prod_rules.find(owner_to_find) == prod_rules.end())
            cout << "no in prod rules"<<endl;
        temp_prod_rule=prod_rules.find(owner_to_find)->second;

        // iterate inside each rule of owner
        for(int j=0; j<temp_prod_rule.size(); j++)
        {
            //element in the current rule
            temp_single_pr=temp_prod_rule[j];
            pos = find(temp_single_pr.begin(), temp_single_pr.end(), NT_index) - temp_single_pr.begin();
            //if not found in this production
            if(pos==temp_single_pr.size())
            {
                continue;
            }
            //case nothing after it
            if(pos==(temp_single_pr.size()-1))
            {
                curr_owner=elements[owner_to_find];

                //follow not computed yet
                if(curr_owner.follow.empty())
                {
                    get_follow(owner_to_find);
                }
                needed_follow=curr_owner.follow;
                for (set<int>::iterator f = needed_follow.begin(); f != needed_follow.end(); f++)
                {
                    follow_ind= *f;
                    follow.insert(follow_ind);
                    insert_to_map(NT_index,follow_ind);
                }

            }
            //case something after it
            else if(pos<temp_single_pr.size())
            {
                is_epsilon=false;
                //get first of next element
                next=temp_single_pr[pos+1];
                next_struct=elements[next];
                //terminal
                if(next_struct.terminal)
                {
                    follow.insert(next);
                    insert_to_map(NT_index,next);
                    continue;
                }
                needed_first=next_struct.first;
                //add the next first to my follow
                for (set<int>::iterator f = needed_first.begin(); f != needed_first.end(); f++)
                {
                    follow_ind= *f;
                    if(follow_ind==EPS_INDEX)
                    {
                        is_epsilon=true;
                    }
                    else
                    {
                        follow.insert(follow_ind);
                        insert_to_map(NT_index,follow_ind);
                    }
                }
                //the first list contains epsilon
                if(is_epsilon)
                {
                    //follow not computed yet
                    if(next_struct.follow.empty())
                        get_follow(next);
                    needed_follow=next_struct.follow;
                    for (set<int>::iterator f = needed_follow.begin(); f != needed_follow.end(); f++)
                    {
                        follow_ind= *f;
                        follow.insert(follow_ind);
                        insert_to_map(NT_index,follow_ind);
                    }
                }
            }
        }
    }
    curr_non_ter.follow = follow;
    elements[NT_index]=curr_non_ter;
}




/**
 *  params : index of a NT and a T
 *  insert to the parsing table either an epsilon or a sync
 *  @return void
 */
void ParsingTable::insert_to_map(int NT_index,int ter_index)
{
    string str = "-1";// $
    if(ter_index!=DOLLAR_SIGN)
        str= elements[ter_index].st;

    /*if the first contains epsilon*/
    if(elements [NT_index].first_has_epsilon)
    {
        parsing_table.insert (std::make_pair(std::make_pair(NT_index,str), EPS_PR) );
        return;
    }
    parsing_table.insert (std::make_pair(std::make_pair(NT_index,str), SYNC_PR) );
}




/**
 *  iterate over all nonterminals to print the first/follow list
 *  @return void
 */
void ParsingTable::print_first_or_follow(string choice)
{
    struct PR_ELEM elem;
    int elem_indx;
    set<int> chosen_list;

    cout << " \n---------" << choice << " -------- \n";
    for(int i=0 ;  i<elements.size() ; i++)
    {
        elem = elements[i];
        if(elem.terminal)
            continue;
        cout << "==> struct: " << elem.st << "  :  " ;
        if(choice=="first")
            chosen_list = elem.first;
        else
            chosen_list = elem.follow;

        if((!elem.terminal) )
        {
            for (set<int>::iterator k = chosen_list.begin(); k !=  chosen_list.end(); k++)
            {
                elem_indx= *k;

                if(elem_indx==EPS_INDEX)
                {
                    cout << "EP  ";
                    continue;
                }
                if(elem_indx==DOLLAR_SIGN)
                {
                    cout << "$  ";
                    continue;
                }
                cout << elements[elem_indx].st<< "  ";
            }
        }
        cout << "\n" ;
    }
}

/**
 *  print the parsing table
 *  @return void
 */
void ParsingTable::print_table()
{
    map<pair<int,string>, vector<int> > ::iterator it;
    vector<int> v;
    ofstream outfile;
    outfile.open("parsing_table.txt");

    for (int i=0 ; i<elements.size() ; i++)
    {
        if(elements[i].terminal)
            continue;

        for(int j=-1; (j<elements.size() || j==-1) ; j++)
        {
            string s = "-1";
            if(j!=-1)
            {
                if(!elements[j].terminal)
                    continue;
                s=elements[j].st ;
            }

            outfile << elements[i].st << " -->" << s << " ===> " ;
            it = parsing_table.find( std::make_pair(i, s));
            if(it==parsing_table.end())
                outfile << "error";
            else if(it->second[0]== -3)
                outfile << "sync";
            else if(it->second[0]== -2)
                outfile<< " EPS";
            else
            {
                v= it->second;
                for(int k=0; k<v.size() ; k++)
                {
                    outfile << elements[v[k]].st << "-";
                }
            }
            outfile << "\n";
        }
        outfile<<"---------------------------   \n" ;
    }
    outfile.close();
}

