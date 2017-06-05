#include <iostream>
#include "phase2.h"
#include "InputParser.h"

#include"Parser.h"
#include"NFA.h"
#include"DFA.h"
#include"Mini_DFA.h"
#include"Lexical_Analyzer.h"
#include"Phase_one.h"


using namespace std;

int main()
{
    cout << INPUT.length()<< endl;
    Parser p;
    NFA nfa;


    vector<pair<string,string> >lpp = p.parse_file();
    for(int i =0; i<lpp.size(); i++)
    {
        cout<< lpp[i].first << "   " <<  lpp[i].second << " \n" ;
        nfa.build_nfa(lpp[i].first,lpp[i].second);
    }

    int c=nfa.build_combined_nfa();
    cout<<"----->"<< c <<endl ;
    DFA dfa(p.sp_map);
    //cout<< nfa.all_nodes<<endl ;
    dfa.fill_subset_construction(c,nfa.all_nodes);

    cout<<"-----> DFA subset done \n\n " ;
    Mini_DFA m_dfa(dfa.subset_construction,dfa.dfa_mapping);
    cout<<"-----> Mini dfa construction done \n\n " ;
    m_dfa.get_mini_dfa();
    cout<<"-----> mini dfa is done \n\n " ;
    //------------------------------
    m_dfa.write_trans_file();
    cout << "tran written"<<endl;

    //-------------------------------------------

    Lexical_Analyzer lex(m_dfa.dfa_network);
    cout<<"-----> lexial analyzer construction is done \n\n " ;
    lex.analyze(*m_dfa.states[0]);
    cout<<"-----> DONE DONE DONE \n\n " ;

    //-------------------------------------end lex------------------------------------------------
    //--------------------------------------------------------------------------------------------


    cout << "Hello Team el 7azina el gamed gidnnnn:D!" << endl;
    /*  */
    LL_Productions p1;
    p1.get_bonus();
    InputParser ip;
    ip.parse_rules();

    /**/
    ParsingTable t(ip.elements, ip.prod_rules_indexes) ;
    t.construct_parsing_table();



    /* */
    Matching m(t.parsing_table,ip.elements);

    m.match_input();

    m.write_file();



    return 0;
}
