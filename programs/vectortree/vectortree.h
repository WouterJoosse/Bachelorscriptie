#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <bitset>
#include <stdlib.h>
#include <algorithm>
#include <time.h>
#include "levenshtein.h"

using namespace std;

const int BSPART=8;
const int BITSETSZ=4*BSPART;

struct knoop{
  int volgend[2];
  set<int> records;
  knoop(){
    volgend[0]=-1;
    volgend[1]=-1;
  }
};

struct vtp{
	int pos;
	int err;
	//int er1;
	//vtp(int a, int b, int c):pos(a),er0(b),er1(c){}
};

void initCharId(map<char,int>& charid);
void initCharId2(map<char,int>& charid);
void splits(string& regel, int& gid, vector<string>& vecdata, string& strdata, char sep);
void string2bs(map<char,int>& charid, vector<string>& data, bitset<BITSETSZ>& bs, int bspart);
void toLower(string& in);
//void assignVec(string bestand,map<int,bitset<BITSETSZ> >& gidmap,map<int,pair<string,string> >& recordmap, map<char,int>& charid, int bspart, set<string>& exactset);
void vectree(map<int,bitset<BITSETSZ> >& gidmap, vector<knoop>& vt);
//void treesearch(vector<knoop>& vt, int vtpos, bitset<BITSETSZ>& bstmp, int bspos, int err0, int err1, int th, set<int>& res, int bspart);
