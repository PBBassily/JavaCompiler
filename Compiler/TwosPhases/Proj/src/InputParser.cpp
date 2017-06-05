#include "InputParser.h"



InputParser::InputParser()
{
    cout<<"-------Parsing Phase-------"<<endl;
    elem_count=0;

}

void InputParser:: parse_rules(){
    read_file();
    init_prod();
    check();
}
/**
    read word by word
    init struct
    set production rule
*/

void InputParser::read_file()
{
    ifstream file;
    file.open (LL_PRODUCTIONS_FILE_PATH);
    string in_word,last_word,key_elem;
    vector<string> anding;
    vector<vector<string>> oring;
    char x ;
    bool first_time=true;
    in_word.clear();
    last_word.clear();

    while ( file>>in_word )
    {
        x = file.get();
        // escape white spaces
        while ( x != ' '&& x != '\t'&& x != '\n')
        {
            in_word = in_word + x;
            x = file.get();
        }

        // if T or NT create struct for it
        if(in_word!="#"&&in_word!="|"&&in_word!="=")
        {
            init_elem(in_word);
        }
        // if elem in NT key for a production rule
        if(last_word=="#")
        {
            key_elem=in_word;
        }
        // start new rule
        else if(in_word=="#"&&!first_time)
        {
            // push last patch to the ORs
            oring.push_back(anding);
            // add it to the rules map
            prod_rules.insert(pair<string,vector<vector<string>>> (key_elem,oring));
            // clear all
            anding.clear();
            oring.clear();
            key_elem.clear();
        }
        else if(in_word=="|")
        {
            // one patch is ended
            oring.push_back(anding);
            anding.clear();
        }
        else if(in_word!="="&&last_word.size()>0)
        {
            // add elem to same patch
            anding.push_back(in_word);
        }

        first_time=false;
        last_word=in_word;
        in_word.clear();

    }
    // add last patch , as loop ended before it has been added
    oring.push_back(anding);
    prod_rules.insert(pair<string,vector<vector<string>>> (key_elem,oring));
    anding.clear();
    oring.clear();
    key_elem.clear();

    file.close();
}
/**
    gives new index to each new elem
    creates struct for the new one
    adds it to the all elements struct vector
*/
void InputParser::init_elem(string str)
{
    // if epsilon return
    if(str==EPS)
        return;
    // remove single quoations if terminal elem
    string temp="";
    if(!isalpha(str[0]))
        temp=str.substr(3,str.size()-6);
    else
        temp=str;
    // search for it in the index map
    // if fnot found
    if(elem_index_map.find(temp)==elem_index_map.end())
    {
        // add it to map
        elem_index_map.insert(pair<string,int>(temp,elem_count));
        // create new struct
        struct PR_ELEM elem;
        // if terminal set true in struct
        if(!isalpha(str[0]))
        {
            elem.terminal=true;
            // to remove single qouations
            elem.st=str.substr(3,str.size()-6);
        }
        // else set false
        else
        {
            elem.terminal=false;
            elem.st=str;
        }
        elements.push_back(elem);
        elem_count++;

    }


}
/**
    convert from production to its indexies
*/
void InputParser::init_prod()
{
    vector <int> anding_ind;
    vector<vector<int>> oring_ind;
    vector<string> anding;
    vector<vector<string>> oring;
    int key;
    for (map<string,vector<vector<string>>>::iterator it=prod_rules.begin(); it!=prod_rules.end(); ++it)
    {
        key=elem_index_map.find(it->first)->second;
        oring=it->second;
        for(int i=0; i<oring.size(); i++)
        {
            anding = oring[i];
            for(int j=0; j<anding.size(); j++)
            {
                if(anding[j]==EPS)
                {
                    anding_ind.push_back(EPS_INDEX);
                }
                else if (!isalpha(anding[j][0]))
                {
                    string temp=anding[j].substr(3,anding[j].size()-6);
                    anding_ind.push_back(elem_index_map.find(temp)->second);
                }
                else
                {
                    anding_ind.push_back(elem_index_map.find(anding[j])->second);
                }

            }
            oring_ind.push_back(anding_ind);
            anding_ind.clear();
        }
        prod_rules_indexes.insert(pair<int,vector<vector<int>>>(key,oring_ind));
        oring_ind.clear();
    }
}
void InputParser::check()
{
    cout<<"elements count = "<<elem_count<<endl;
    for(unsigned int i=0; i<elements.size(); i++)
    {
        struct PR_ELEM x = elements[i];
        cout<<i<<" ) "<<x.st<<" "<<x.terminal<<endl;
    }
    cout<<endl;
    cout<<"-----------Productions-----------"<<endl;
    for (map<string,vector<vector<string>>>::iterator it=prod_rules.begin(); it!=prod_rules.end(); ++it)
    {
        vector<string> anding;
        vector<vector<string>> oring;
        cout<<it->first<<" ";
        oring=it->second;
        for(int i=0; i<oring.size(); i++)
        {
            cout<<"[ ";
            anding = oring[i];
            for(int j=0; j<anding.size(); j++)
            {
                cout<<anding[j]<<" ";
            }
            cout<<"] ";
        }
        cout<<endl;
    }
    cout<<endl;
//    cout<<"------- elments index------"<<endl;
//    for (map<string,int>::iterator it=elem_index_map.begin(); it!=elem_index_map.end(); ++it)
//    {
//        cout<<it->first<<" --> "<<it->second<<endl;
//    }
//    cout<<endl;
    cout<<"-------Productions index------"<<endl;
    for (map<int,vector<vector<int>>>::iterator it=prod_rules_indexes.begin(); it!=prod_rules_indexes.end(); ++it)
    {
        vector<int> anding;
        vector<vector<int>> oring;
        cout<<it->first<<" ";
        oring=it->second;
        for(int i=0; i<oring.size(); i++)
        {
            cout<<"[ ";
            anding = oring[i];
            for(int j=0; j<anding.size(); j++)
            {
                cout<<anding[j]<<" ";
            }
            cout<<"] ";
        }
        cout<<endl;
    }
}
InputParser::~InputParser()
{
    //dtor
}
