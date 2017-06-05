#include "LL_productions.h"

void LL_Productions::get_bonus(){
    read_rules();
    write_rules();
}
/**
    read word by word and construct the rule
    to be checked for having left factor, left
    recursion or not
*/
void LL_Productions::read_rules()
{
    ifstream file;
    file.open (PRODUCTIONS_FILE_PATH);
    string in_word,last_word,key_elem;
    vector<vector<string>>rule;
    vector<string>patch ;
    char x ;
    bool first_time=true;
    in_word.clear();
    last_word.clear();
    patch.clear();
    while ( file>>in_word )
    {
        x = file.get();
        // escape white spaces
        while ( x != ' '&& x != '\t'&& x != '\n')
        {
            in_word = in_word + x;
            x = file.get();
        }
        // add key to the first slot in the rule list
        if(last_word=="#")
        {
            patch.push_back(in_word);
            rule.push_back(patch);
            patch.clear();
        }
        // add the last patch of the last rule, as
        // new rule is encountered
        else if(in_word=="#"&&!first_time)
        {
            rule.push_back(patch);
            // now the rule is ready for test
            handle_rule(rule);
            patch.clear();
            rule.clear();
        }
        //  add new patch to the rule
        else if(in_word=="|")
        {
            rule.push_back(patch);
            patch.clear();
        }
        // add to the same patch another elem
        else if(in_word!="="&&in_word!="#"&&last_word.size()>0)
        {
            patch.push_back(in_word);
        }
        first_time=false;
        last_word=in_word;
        in_word.clear();
    }
    // add the last patch to the rule
    rule.push_back(patch);
    handle_rule(rule);
    write_rules();
    patch.clear();
    rule.clear();
}
/**
    check if any key is similar to any start in
    any of its patches
*/
bool LL_Productions::check_left_rec(vector<vector<string>> rule)
{
    string key = rule[0][0];
    for(int i=1; i<rule.size(); i++)
    {
        if(rule[i][0]==key)
        {
            // left rec is found
            cout<< key << " --> left recursive"<<endl;
            return true;
        }
    }
    return false;
}
/**
    check if any start of any patch is equal
    to another patch start
*/
bool LL_Productions::check_left_fac(vector<vector<string>> rule)
{
    if(rule.size()<=2)
    {
        return false;
    }
    for(int i=1; i<rule.size(); i++)
    {
        string factor= rule[i][0];
        for(int j=i+1; j<rule.size(); j++)
        {
            if(rule[j][0]==factor)
            {
                // left fac is found
                // set to factor index to start
                // fixiation from it
                factor_index=i;
                cout<< rule[0][0] << " --> left factor"<<endl;
                return true;
            }
        }
    }
    return false;
}
/**
    handles flow of testing and rules
    if now left fac or rec
    then add it to all rules
*/
void LL_Productions::handle_rule(vector<vector<string>> rule)
{
    if(check_left_rec(rule))
    {
        remove_left_rec(rule);
    }
    else if(check_left_fac(rule))
    {
        remove_left_fac(rule);
    }
    else
        all_rules.push_back(rule);
}
/**
    remove left rec by same rule discussed in class
*/
void LL_Productions::remove_left_rec(vector<vector<string>> rule)
{
    vector <string> patch_dash;
    vector <string> patch_new;
    vector <string> dummy_list;
    vector<vector <string>> rule_dash;
    vector<vector <string>> rule_new;
    string key = rule[0][0];
    string key_dash= key+"_X";

    dummy_list.push_back(key);
    rule_new.push_back(dummy_list);
    dummy_list.clear();

    dummy_list.push_back(key_dash);
    rule_dash.push_back(dummy_list);
    dummy_list.clear();
    for(int i=1; i<rule.size(); i++)
    {
        if(rule[i][0]==key)
        {
            for(int j=1; j<rule[i].size(); j++)
            {
                patch_dash.push_back(rule[i][j]);
            }
            patch_dash.push_back(key_dash);
            rule_dash.push_back(patch_dash);
            dummy_list.push_back(EPS);
            rule_dash.push_back(dummy_list);
        }
        else
        {
            for(int j=0; j<rule[i].size(); j++)
            {
                patch_new.push_back(rule[i][j]);
            }
            patch_new.push_back(key_dash);
            rule_new.push_back(patch_new);
            patch_new.clear();
        }
    }
    all_rules.push_back(rule_new);
    all_rules.push_back(rule_dash);
}
/**
    remove left fac by same rule discussed in class
*/
void LL_Productions::remove_left_fac(vector<vector<string>> rule)
{
    vector <string> patch_dash;
    vector <string> patch_new;
    vector <string> dummy_list;
    vector<vector <string>> rule_dash;
    vector<vector <string>> rule_new;
    string key = rule[0][0];
    string key_dash= key+"_X";

    dummy_list.push_back(key);
    rule_new.push_back(dummy_list);
    dummy_list.clear();

    dummy_list.push_back(key_dash);
    rule_dash.push_back(dummy_list);
    dummy_list.clear();
    string factor = rule[factor_index][0];
    for(int i=factor_index; i<rule.size(); i++)
    {
        if(rule[i][0]==factor)
        {

            for(int j=1; j<rule[i].size(); j++)
            {
                patch_dash.push_back(rule[i][j]);
            }
            if(patch_dash.size()==0)
                patch_dash.push_back(EPS);
            rule_dash.push_back(patch_dash);
            patch_dash.clear();
        }
        else
        {
            for(int j=0; j<rule[i].size(); j++)
            {
                patch_new.push_back(rule[i][j]);
            }
            rule_new.push_back(patch_new);
            patch_new.clear();
        }
    }
    patch_new.push_back(factor);
    patch_new.push_back(key_dash);
    rule_new.push_back(patch_new);
    patch_new.clear();

    all_rules.push_back(rule_new);
    all_rules.push_back(rule_dash);
}
/**
    writes modified rules to the another file to be
    read by the parser
*/
void LL_Productions::write_rules()
{
    ofstream out_file(LL_PRODUCTIONS_FILE_PATH);
    for(int k=0; k<all_rules.size(); k++)
    {

        for(int i=0; i<all_rules[k].size(); i++)
        {
            for(int j=0; j<all_rules[k][i].size(); j++)
            {
                if(i==0)
                {
                    cout<<"# "<<all_rules[k][i][0]<<" = ";
                    out_file<<"# "<<all_rules[k][i][0]<<" = ";
                    continue;
                }
                cout<<all_rules[k][i][j]<<" ";
                out_file<<all_rules[k][i][j]<<" ";
            }
            if(i!=all_rules[k].size()-1&&i!=0)
            {
                cout<<" | ";
                out_file<<" | ";
            }
        }
        cout<<endl;
        out_file<<endl;
    }
}
LL_Productions::LL_Productions()
{
    //ctor

}


LL_Productions::~LL_Productions()
{
    //dtor
}

