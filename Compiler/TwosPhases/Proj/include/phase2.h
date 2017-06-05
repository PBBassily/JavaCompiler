#ifndef PHASE2_H_INCLUDED
#define PHASE2_H_INCLUDED

#include<map>
#include<list>

#include<malloc.h>
#include<string.h>


#include <stdlib.h>
#include <stdio.h>
#include <regex>

#include <iostream>
#include <vector>

#include <set>

#include <fstream>

#include "Element.h"
#include "ParsingTable.h"
#include "InputParser.h"
#include "LL_productions.h"
#include "Matching.h"


namespace std
{
const string EPS = "\\L";
const int EPS_INDEX = -2; //-------------->
const int DOLLAR_SIGN = -1;

const vector <int> EPS_PR = {-2};            /* vector indicating epsilon production*/
const vector <int> SYNC_PR = {-3};           /* vector indicating "SYNC" */


// file pathes
// main file path of CFG
const string PRODUCTIONS_FILE_PATH="productions.txt";
// created file path of LL CFG
const string LL_PRODUCTIONS_FILE_PATH="productions_ll_ours.txt";
}


#endif // PHASE2_H_INCLUDED
