//modified from: http://www.merriampark.com/ldcpp.htm

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <map>
#include <fstream>
#include "levenshtein.h"
using namespace std;

int levenshtein(const string& source, const string& target, const int th) {
  int res;
  //cout << source << "," << target << endl;
  // Step 1
  const int n = source.length();
  const int m = target.length();
  int i,j,th2n,th2m,minj,maxj;
  int left,above,diag;
  int th1 = 2*th+1;
  if(th==-1){
    th1=2*(n+m)+1;
  }
  if (source==target){
    return 0;
  }  
  if (n == 0) {
    return m;
  }
  if (m == 0) {
    return n;
  }
  if(abs(n-m) > th){
    return -1;
  }

  Tmatrix matrix(n+1);

  for (i = 0; i <= n; i++) {
    matrix[i].resize(m+1);
  }

  // Step 2
  th2n = min(n,th1);
  for (i = 0; i <= th2n; i++) {
    matrix[i][0]=i;
  }
  
  th2m = min(m,th1);
  for (j = 0; j <= th2m; j++) {
    matrix[0][j]=j;
  }

  // Step 3
  for (i = 1; i <= n; i++) {
    
    const char s_i = source[i-1];
    // Step 4
    // bereken benodigd j-interval
    // let op: bij grenzen minimumwaarde beredeneren
    minj = max(1,i-(th1-1));
    maxj = min(m,i+(th1-1));
    for (j = minj; j <= maxj; j++) {
      
      const char t_j = target[j-1];
      // Step 5

      int cost;
      if (s_i == t_j) {
        cost = 0;
      }
      else {
        cost = 1;
      }
      // Step 6
      above = matrix[i-1][j];
      left = matrix[i][j-1];
      diag = matrix[i-1][j-1];
            
      if(j-i>=th1-1){		//linkercel niet ingevuld
	above = min(left+1,diag+1);
      }
      if(i-j>=th1-1){		//bovencel niet ingevuld
	left = min(above+1,diag+1);
      }
      const int cell = min( above + 1, min(left + 1, diag + cost));
      // Step 6A: Cover transposition, in addition to deletion,
      // insertion and substitution. This step is taken from:
      // Berghel, Hal ; Roach, David : "An Extension of Ukkonen's 
      // Enhanced Dynamic Programming ASM Algorithm"
      // (http://www.acm.org/~hlb/publications/asm/asm.html)
      /*
      if (i>2 && j>2) {
        int trans=matrix[i-2][j-2]+1;
        if (source[i-2]!=t_j) trans++;
        if (s_i!=target[j-2]) trans++;
        if (cell>trans) cell=trans;
      }
      */
      if(cell>th && (n-m)==(i-j)){	//cell is op einddiagonaal en >th
	return -1;
      }

      matrix[i][j]=cell;
    }
  }

  // Step 7

  //matrix gevuld
  //terugrekenen
  //neem minimum van links,boven, diagonaal
  //als gelijke minimumwaarde, kies diagonaal
  //anders: ga naar minimum
  /*i=n;
  j=m;
  string transtype;
  vector<string> tmptrans;
  //tmptrans.push_back("#");
  while(i+j>0){
    if(i==0){
      tmptrans.push_back("ins_" + target[j-1]);
      j--;
    }
    if(j==0){
      tmptrans.push_back("del_" + source[i-1]);
      i--;
    }
    if(i>0 && j>0){
      min_ij = min(matrix[i-1][j],min(matrix[i][j-1],matrix[i-1][j-1]));
      if(matrix[i-1][j-1]==min_ij){
	if(matrix[i-1][j-1]!=matrix[i][j]){
	  transtype="sub_";
	  char rest[4] = {source[i-1],'_',target[j-1],'\0'};
	  tmptrans.push_back(transtype+rest);
	}else{
	  tmptrans.push_back(source.substr(i-1,1));
	}
	i--;
	j--;
      }else{
	if(matrix[i-1][j]==min_ij){
	  transtype="del_";
	  transtype.push_back(source[i-1]);
	  tmptrans.push_back(transtype);
	  i--;
	}
	if(matrix[i][j-1]==min_ij){
	  transtype="ins_";
	  transtype.push_back(target[j-1]);
	  tmptrans.push_back(transtype);
	  j--;
	}
      }
    }
  }
  //tmptrans.push_back("#");
  int k1,k2,ts,li,re;
  vector<string> tmpwi;
  string tmpflat;
  ws--;
  ts=tmptrans.size()-1;
  for(k1=ts;k1>=0;k1--){
    if(tmptrans[k1].length()>1){
      li=k1+ceil((float) ws/2);
      if(li>ts){ li=ts; }
      re=li-ws;
      if(re<0){
        re=0;
        li=((re+ws)<ts) ? re+ws : ts;
      }
      for(k2=li;k2>=re;k2--){
        tmpwi.push_back(tmptrans[k2]);
        tmpflat += tmptrans[k2];
      }
      res.trans.push_back(tmpwi);
      res.flat.push_back(tmpflat);
      tmpwi.clear();
      tmpflat.clear();
    } 
  }
  */
  res=matrix[n][m];
  return res;
}

float lv_ordered(const string& source, const string& target, map<string,float>& orderedmap) {
  float res;
  int ws=2;
  //cout << source << "," << target << endl;
  // Step 1
  const int n = source.length();
  const int m = target.length();
  int i,j,min_ij;
  int left,above,diag;
  if (source==target){
    //res.dist=0;
    return(0.0);
  }  
  
  Tmatrix matrix(n+1);

  for (i = 0; i <= n; i++) {
    matrix[i].resize(m+1);
  }

  // Step 2
  for (int i = 0; i <= n; i++) {
    matrix[i][0]=i;
  }

  for (int j = 0; j <= m; j++) {
    matrix[0][j]=j;
  }

  // Step 3
  for (i = 1; i <= n; i++) {
    
    const char s_i = source[i-1];
    // Step 4
    // bereken benodigd j-interval
    // let op: bij grenzen minimumwaarde beredeneren
    //minj = max(1,i-(th1-1));
    //maxj = min(m,i+(th1-1));
    //for (j = minj; j <= maxj; j++) {
    for (j = 1; j <= m; j++) {  
      const char t_j = target[j-1];
      // Step 5

      int cost;
      if (s_i == t_j) {
        cost = 0;
      }
      else {
        cost = 1;
      }
      // Step 6
      above = matrix[i-1][j];
      left = matrix[i][j-1];
      diag = matrix[i-1][j-1];
            
      /*if(j-i>=th1-1){		//linkercel niet ingevuld
	above = min(left+1,diag+1);
      }
      if(i-j>=th1-1){		//bovencel niet ingevuld
	left = min(above+1,diag+1);
      }*/
      const int cell = min( above + 1, min(left + 1, diag + cost));
      // Step 6A: Cover transposition, in addition to deletion,
      // insertion and substitution. This step is taken from:
      // Berghel, Hal ; Roach, David : "An Extension of Ukkonen's 
      // Enhanced Dynamic Programming ASM Algorithm"
      // (http://www.acm.org/~hlb/publications/asm/asm.html)
      /*
      if (i>2 && j>2) {
        int trans=matrix[i-2][j-2]+1;
        if (source[i-2]!=t_j) trans++;
        if (s_i!=target[j-2]) trans++;
        if (cell>trans) cell=trans;
      }
      */
      matrix[i][j]=cell;
    }
  }

  // Step 7

  //matrix gevuld
  //terugrekenen
  //neem minimum van links,boven, diagonaal
  //als gelijke minimumwaarde, kies diagonaal
  //anders: ga naar minimum
  i=n;
  j=m;
  string transtype;
  vector<string> tmptrans;
  //tmptrans.push_back("#");
  while(i+j>0){
    if(i==0){
      tmptrans.push_back("ins_" + target[j-1]);
      j--;
    }
    if(j==0){
      tmptrans.push_back("del_" + source[i-1]);
      i--;
    }
    if(i>0 && j>0){
      min_ij = min(matrix[i-1][j],min(matrix[i][j-1],matrix[i-1][j-1]));
      if(matrix[i-1][j-1]==min_ij){
	if(matrix[i-1][j-1]!=matrix[i][j]){
	  transtype="sub_";
	  char rest[4] = {source[i-1],'_',target[j-1],'\0'};
	  tmptrans.push_back(transtype+rest);
	}else{
	  tmptrans.push_back(source.substr(i-1,1));
	}
	i--;
	j--;
      }else{
	if(matrix[i-1][j]==min_ij){
	  transtype="del_";
	  transtype.push_back(source[i-1]);
	  tmptrans.push_back(transtype);
	  i--;
	}
	if(matrix[i][j-1]==min_ij){
	  transtype="ins_";
	  transtype.push_back(target[j-1]);
	  tmptrans.push_back(transtype);
	  j--;
	}
      }
    }
  }
  //tmptrans.push_back("#");
  int k1,k2,ts,li,re;
  vector<string> transvec;
  string tmpflat;
  while(ws>0){
    ws--;
    ts=tmptrans.size()-1;
    for(k1=ts;k1>=0;k1--){
      if(tmptrans[k1].length()>1){
        li=k1+ceil((float) ws/2);
        if(li>ts){ li=ts; }
        re=li-ws;
        if(re<0){
          re=0;
          li=((re+ws)<ts) ? re+ws : ts;
        }
        for(k2=li;k2>=re;k2--){
          //tmpwi.push_back(tmptrans[k2]);
          tmpflat += tmptrans[k2];
        }
        //res.trans.push_back(tmpwi);
        transvec.push_back(tmpflat);
        //tmpwi.clear();
        tmpflat.clear();
      } 
    }
  }
  res=(float) matrix[n][m];
  
  map<string,float>::iterator it;
  for(unsigned int tr=0;tr<transvec.size();tr++){
    cout << "trans: " << transvec[tr] << endl;
    it=orderedmap.find(transvec[tr]);
    if(it!=orderedmap.end()){
      cout << res << "-" << (*it).second << "=";
      res-=(*it).second;
      cout << res << endl;
      cout << "discount: " << (*it).second << endl;
    }else{
      cout << "geen discount" << endl;
    }
  }
  cin.get();
  return res;
}