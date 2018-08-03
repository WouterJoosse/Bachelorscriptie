#include "vectortree.h"
#include <sys/resource.h>

void initCharId(map<char,int>& charid){
	for(char ch=97;ch<=122;ch++){
		charid.insert(make_pair(ch,ch-97));
	}
	charid.insert(make_pair(' ',26));
	charid.insert(make_pair(',',27));
	charid.insert(make_pair('_',28));
}
// ,3 # ,,1 # a,1 # b,7 # c,5 # d,5 # e,0 # f,5 # g,0 # h,1 # i,4 # j,1 # k,7 # l,1 # m,5 # n,3 # o,7 # p,2 # q,1 # r,2 # s,6 # t,6 # u,4 # v,2 # w,4 # x,1 # y,6 # z,5 # 
/**
* Posities van de characters in de bitvector. Zie H4.5 voor details
*/
void initCharId2(map<char,int>& charid){
	charid.insert(make_pair('e',0));
	charid.insert(make_pair('j',1));
	charid.insert(make_pair('g',0));
	charid.insert(make_pair('x',1));
	
	charid.insert(make_pair('a',1));
	charid.insert(make_pair('l',1));
	charid.insert(make_pair('c',5));
	charid.insert(make_pair('q',1));
	
	charid.insert(make_pair('r',2));
	charid.insert(make_pair('h',1));
	charid.insert(make_pair('p',2));
	
	charid.insert(make_pair('n',3));
	charid.insert(make_pair('m',5));
	charid.insert(make_pair('w',4));
	charid.insert(make_pair(' ',3));
	
	charid.insert(make_pair('i',4));
	charid.insert(make_pair('k',7));
	charid.insert(make_pair('v',2));
	charid.insert(make_pair(',',1));
	
	charid.insert(make_pair('s',6));
	charid.insert(make_pair('d',5));
	charid.insert(make_pair('f',5));
	
	charid.insert(make_pair('t',6));
	charid.insert(make_pair('u',4));
	charid.insert(make_pair('z',5));
	
	charid.insert(make_pair('o',7));
	charid.insert(make_pair('b',7));
	charid.insert(make_pair('y',6));
}

/**
* Leest een regel uit een invoerbestand in en slaat het certificate id op in gid en 
* de originele regel in vecdata[0], en in vecdata[1] t/m vecdata[4] de 4 verschillende 
* namen op de akte. De splitsing gebeurd op basis van de seperator '|'
*/
void splits(string& regel, int& gid, vector<string>& vecdata, char sep){
	
	string::size_type f=0,f0=0;
	vecdata.clear();
	f=regel.find(sep,f0);

	// het registratienummer
	gid=atoi(regel.substr(0,f).c_str()); 	// atoi: maakt van een string een int

	string strdata="";
	vecdata.push_back(strdata);				// queue() --> Voeg iets aan het einde toe
	
	while(f != string::npos){
		f0=f+1;
		f=regel.find(sep,f0);

		vecdata.push_back(regel.substr(f0,f-f0));
		strdata+=regel.substr(f0,f-f0)+"|";
	}
	vecdata[0]=strdata;

}

/**
* Maakt van een string een bitvector. 
* - charid: de charid map is een map van welke letter op welke positie in * de bitvector wordt gezet.
* - data: De vector met verschillende namen (van positie 1 tot 4)
* - bs: Een array met daarin de bitvector per naam
*/
void string2bs(map<char,int>& charid, vector<string>& data, bitset<BITSETSZ>& bs){
	map<char,int>::iterator it;
	bs.reset();

	// Loop over de namen in de vecdata vector
	for(unsigned int i=1;i<data.size();i++){
		unsigned int len=data[i].size();

		// check de characters in de naam
		for(unsigned int j=0;j<len;j++){
			it=charid.find(data[i][j]);
			if(it==charid.end()){
				bs[i*BSPART-1]=1; // Als een letter niet in de charmap zit, dan is de eerste positie in de bitvector voor die naam 'aan'
			}else{
				bs[(i-1)*BSPART+(*it).second]=1; // en anders de positie die de charmap aangeeft
			}
		}
	}
}

/**
* Zet een string om naar alleen maar kleine letters
*/
void toLower(string& in){
		 int len=in.length();
		 for(int i=0;i<len;i++){
				if(in[i]>=65 && in[i] <=90){
					 in[i]+=32;
				}
		 }
}

/**
	Maakt van de namen in de input file een map met de bitvector van de namen voor elke registratie
	en slaat ook de regels in de input file op in een aparte record map.

	Een bitvector bestaat uit 8 bits, met voor elke positie een set van letters. De positie
	wordt getriggerd als een van de letters voorkomt in de naam.

	- bestand: de bestandsnaam van het inputbestand
	- gidmap: de map die wordt gevuld met registratie id's en de bijbehorden bitvectoren
	- recordmap: de map die wordt gevuld met registratie id's en de bijbehorende namen in strings
	- charid: de map met de posities van letters in een bitvector.
*/
void assignVec(string bestand, map<int,bitset<BITSETSZ> >& gidmap, map<int, vector<string> >& recordmap, map<char,int>& charid){
	ifstream fin(bestand.c_str());
	string regel;
	int gid;

	vector<string> vecdata;
	bitset<BITSETSZ> bstmp;

	int n=0;
	set<int> tmpset;
	map<string,set<int> >::iterator it1; 	// een iterator over een map van strings met sets van ints


	while(getline(fin,regel)){
		if(n%500000==0){
			cout << n << endl;
		}
		n++;
		toLower(regel);
		splits(regel,gid,vecdata,'|'); // gid wordt de id van de registratie, vecdata is een vector met daarin op positie 0 de originele namen en daarna per positie de 4 verschillende namen
		string2bs(charid,vecdata,bstmp);

		// gidmap is de map van ids en hun bijbehorende bitvector
		gidmap.insert(make_pair(gid,bstmp));

		// recordmap is de map van ids en hun bijbehorende namen in strings
		recordmap.insert(make_pair(gid,vecdata)); 

	}
	fin.close();
}

/**
	Slaat van alle records in recordmap de strings op zoals die in de input file staan 
	in exactset.
*/
void mexactset(map<int,vector<string> >& recordmap, set<string>& exactset){
	map<int,vector<string> >::iterator it1;
	for(it1=recordmap.begin();it1!=recordmap.end();++it1){
		// De exactset is een set van alle input strings
		exactset.insert((*it1).second[0]);
	}
}

/**
* gidmap: een map van certificate ids' en hun bijbehorende bitvector van de namen
* vt: de vector tree met in de bladeren de ids van de certificaten
* Pseudo code: Algorthm 4.1 
*/
void vectree(map<int,bitset<BITSETSZ> >& gidmap, vector<vector<int> >& vt){
	
	int vtn=0; // current node
	int vtid,vtid2;
	
	vector<int> lknoop(2,-1); // linkerknoop is een vector van 2 posities met waarden -1
	vt.push_back(lknoop);
	map<int,bitset<BITSETSZ> >::iterator it1;

	int i;
	int i2=0; 

	cout << "totaal aantal gids: " << gidmap.size() << endl;

	// for all target records t do:
	for(it1=gidmap.begin();it1!=gidmap.end();++it1){
		// print af en toe wat om te laten zien dat er wat gebeurd
		if(i2%500000==0) cout << i2 << " (knopen: " << vt.size() << ")" << endl;

		i2++;
		vtid=0;

		// BITSETSZ = 4 * BSPART = 4 * 8
		// for i = 0 to n do:
		for(i=0;i<BITSETSZ;i++){

			// vtid2 is ofwel 0 of 1, of -1 als die niet bestaat....
			vtid2=vt[vtid][(*it1).second[i]];

			// if not EdgeExists(c, b_i, c') then
			if ( vtid2 == -1 ){

				// AddEdge(c, b_i, c_new)
				vt.push_back(lknoop); 
				vt[vtid][(*it1).second[i]]=++vtn;

				// c <- c_new
				vtid2=vtn;
				
			}
			// c <- c'
			vtid=vtid2;
		}
		// if LastBit(b_i) then
		vt[vtid].push_back((*it1).first); // AddRecord(c,t)
	}
	cout << "totaal aantal knopen: " << vt.size() << endl;
}

void vulnk(vector<vector<int> >& vt, map<string,set<string> >& naammap, string& naam, map<string,int>& naamindex, set<int>& tmpset, vector<bitset<300> >& knoopbs, int& dubbel, int& enkel){
	map<string,set<string> >::iterator it1;
	set<string>::iterator it2;
	set<int>::iterator it3;
	int nid,bid;
	it1=naammap.find(naam);
	if(it1!=naammap.end()){
		for(it2=(*it1).second.begin();it2!=(*it1).second.end();++it2){
			nid=naamindex[(*it2)];
			for(it3=tmpset.begin();it3!=tmpset.end();++it3){
				if(vt[(*it3)].size()>2){
					dubbel++;
					knoopbs[vt[(*it3)][2]][nid]=1;
				}else{
					enkel++;
					knoopbs.push_back(bitset<300>());
					bid=knoopbs.size()-1;
					knoopbs[bid][nid]=1;
					vt[(*it3)].push_back(bid);
				}
			}
		}
	}
}

void vn1knopen(vector<vector<int> >& vt, map<int,bitset<BITSETSZ> >& gidmap, vector<map<string,set<string> > >& lvmap, map<int,vector<string> >& recordmap, vector<map<string,int> >& naamindex, vector<bitset<300> >& knoopbs, int imin, int imax, int istep){
	int vtid;
	map<int,bitset<BITSETSZ> >::iterator it1;
	string vn1,vn2,fn1,fn2;
	set<int> tmpset;
	int i;
	int i2=0;
	cout << "totaal aantal gids: " << gidmap.size() << endl;
	int dubbel=0,enkel=0;
	for(it1=gidmap.begin();it1!=gidmap.end();++it1){
		if(i2%100000==0) cout << i2 << endl;
		i2++;
		vtid=0;

		vn1=recordmap[(*it1).first][1];
		fn1=recordmap[(*it1).first][2];
		vn2=recordmap[(*it1).first][3];
		fn2=recordmap[(*it1).first][4];
		tmpset.clear();
		for(i=0;i<BITSETSZ;i++){
			if(i>=imin && i<=imax && i%istep==0){
				tmpset.insert(vtid);
			}
			vtid=vt[vtid][(*it1).second[i]];
		}
		vulnk(vt,lvmap[0],vn1,naamindex[0],tmpset,knoopbs,dubbel,enkel);
		vulnk(vt,lvmap[2],fn1,naamindex[2],tmpset,knoopbs,dubbel,enkel);
		vulnk(vt,lvmap[1],vn2,naamindex[1],tmpset,knoopbs,dubbel,enkel);
		vulnk(vt,lvmap[2],fn2,naamindex[2],tmpset,knoopbs,dubbel,enkel);
	}
	cout << "aantal keer knopen dubbel gebruikt: " << dubbel << endl;
	cout << "aantal gebruikte knopen: " << enkel << endl;
}

void pbs(int start, map<int,int>& bw0, map<int,int>& bw1){
	int pos=start;
	bool vorig=true;
	map<int,int>::iterator itbw0,itbw1;
	while(vorig){
		itbw0 = bw0.find(pos);
		itbw1 = bw1.find(pos);
		if(itbw0==bw0.end()){
			if(itbw1==bw1.end()){
				vorig=false;
			}else{
				cout << "1";
				pos = (*itbw1).second;
			}
		}else{
			cout << "0";
			pos = (*itbw0).second;
		}
	}
	cout << endl;
}

int treedown(vector<knoop>& vt, int knoop, set<int>& checked, map<int,string>& recordmap, int level){
	cout << "treedown, level " << level << endl;
	cout << "knoop " << knoop << endl;
	int res=0;
	int res1=0,res2=0;
	int ktmp1,ktmp2;
	ktmp1=vt[knoop].volgend[0];
	ktmp2=vt[knoop].volgend[1];
	if(ktmp1==-1 && ktmp2==-1){
		for(set<int>::iterator itr=vt[knoop].records.begin(); itr!=vt[knoop].records.end();++itr){
			cout << (*itr) << " # " << recordmap[(*itr)] << endl;
			res++;
		}
		return(res);
	}else{
		if(ktmp1>-1){
			if(checked.find(vt[knoop].volgend[0])==checked.end()){
				res1=(treedown(vt,vt[knoop].volgend[0],checked,recordmap,level+1));
			}
		}
		if(ktmp2>-1){
			if(checked.find(vt[knoop].volgend[1])==checked.end()){
				res2=(treedown(vt,vt[knoop].volgend[1],checked,recordmap,level+1));
			}
		}
		return(res1+res2);
	}
}
		

void treeadd(vector<knoop>& vt, int vtpos, set<int>& checked, int gevonden, int th, map<int,int>& bw0, map<int,int>& bw1, map<int,string>& recordmap, int level){
	cout << "treeadd, knoop: " << vtpos << endl;
	cout << "niveau: " << level << endl;
	cout << "th: " << th << " gevonden: " << gevonden << endl;
	cin.get();
	checked.insert(vtpos);
	int gevonden2;
	map<int,int>::iterator itbw0,itbw1;
	itbw0 = bw0.find(vtpos);
	itbw1 = bw1.find(vtpos);
	int kntmp;
	if(itbw0!=bw0.end()){
		kntmp=(*itbw0).second;
	}else{
		if(itbw1!=bw1.end()){
			kntmp=(*itbw1).second;
		}else{
			kntmp=-1;
		}
	}
	if(kntmp!=-1){
		pbs(kntmp,bw0,bw1);
		gevonden2=treedown(vt,kntmp,checked,recordmap,0);
			//1 knoop omhoog
		//recursief naar beneden->tel aantal nieuwe records
		if(gevonden+gevonden2<th){
			//treeadd vanaf de knoop omhoog
			treeadd(vt,kntmp,checked,gevonden+gevonden2,th,bw0,bw1,recordmap,level+1);
		}
	}
}
	

void treeview(vector<knoop>& vt, map<int,string>& recordmap){
	map<int,int> bw0,bw1;
	for(unsigned int itbw=0;itbw<vt.size();itbw++){
		if(vt[itbw].volgend[0]!=-1){
			bw0.insert(make_pair(vt[itbw].volgend[0],itbw));
		}
		if(vt[itbw].volgend[1]!=-1){
			bw1.insert(make_pair(vt[itbw].volgend[1],itbw));
		}
	}
	set<int> checked;
	unsigned int nsub;
	unsigned int th=25;
	//set<int> rset;
	for(unsigned int vkit=0;vkit<vt.size();vkit++){
		if(checked.find(vkit)==checked.end()){
			nsub=0;
			if(vt[vkit].volgend[0]==-1 && vt[vkit].volgend[1]==-1){   //eindknoop
				checked.insert(vkit);
				nsub+=vt[vkit].records.size();
				pbs(vkit,bw0,bw1);
				
				for(set<int>::iterator itr=vt[vkit].records.begin(); itr!=vt[vkit].records.end();++itr){
					cout << (*itr) << " # " << recordmap[(*itr)] << endl;
				}
				treeadd(vt,vkit,checked,nsub,th,bw0,bw1,recordmap,0);
				cin.get();
			}
		}
	}
}
				
				
void treesearchvn1(vector<vector<int> >& vt, int vtpos, bitset<BSPART>& bsvn1orig, int bspos, int err0, int err1, int th, vector<vtp>& res, bitset<BSPART>& bsvn1tmp){  
	if(vtpos!=-1){
		if(bspos==BSPART){

			vtp vtposerr={vtpos,max(err0,err1)};
			res.push_back(vtposerr);
		}else{
			int c1=bsvn1orig[bspos];
			int c2=1-c1;
			int vtpos1=vt[vtpos][c1];
			int vtpos2=vt[vtpos][c2];
			if(err0+c2<=th && err1+c1<=th){
				bsvn1tmp.flip(bspos);
				treesearchvn1(vt,vtpos2,bsvn1orig,bspos+1,err0+c2,err1+c1,th,res,bsvn1tmp);
				bsvn1tmp.flip(bspos);
			}
			treesearchvn1(vt,vtpos1,bsvn1orig,bspos+1,err0,err1,th,res,bsvn1tmp);
		}
	}
}

void treesearchp(vector<vector<int> >& vt, int vtpos, bitset<BITSETSZ>& bstmp, int bspos, int err0, int err1, int th, set<int>& res, vector<int>& nid, vector<bitset<300> >& bsknoop, int& pogingen){
	pogingen++;

	if(vtpos!=-1){
		if(bspos==BITSETSZ){

			res.insert(vt[vtpos].begin()+2,vt[vtpos].end());

		}else{
			if(bspos%BSPART==0){
				err0 = max(err0,err1);
				err1 = max(err0,err1);
			}
			int c1=bstmp[bspos];
			int c2=1-c1;
			int vtpos1=vt[vtpos][c1];
			int vtpos2=vt[vtpos][c2];
			bool tcheck=true;
			if(err0+c2<=th && err1+c1<=th){
				if((bspos+1)>=16 && (bspos+1)<=24 && (bspos+1)%4==0 && vtpos2>-1){
					//cout << "record: " << j << " vtpos: " << vtpos2 << endl;
					//cout << "andere bitset. bspos: " << bspos << endl;
					int i=0;
					while(i<=3 && tcheck){
						if(nid[i]!=-1){
							//if(vt[vtpos2].records.find(nid[i])==vt[vtpos2].records.end()){}
							if(vt[vtpos2].size()==2){
								tcheck=false;
							}else{
								if(!bsknoop[vt[vtpos2][2]][nid[i]]){
									tcheck=false;
									//cout << "naam " << i << " niet in knoop" << endl;
								}
							}
						}
						i++;
					}
					/*if(vb && bspos>=24){
					cout << "bspos: " << bspos << endl;
					cout << "err++" << endl;
					cout << "naamid: " << nid << " knoop: " << vtpos2 << endl;
					cout << "aantal namen in knoop: " << vt[vtpos2].records.size() << endl;
					}*/
					//cin.get();
				}
				if(tcheck){
					//cout << "verkeerde tak" << endl;
					treesearchp(vt,vtpos2,bstmp,bspos+1,err0+c2,err1+c1,th,res,nid,bsknoop,pogingen);
				}
			}//else{
			//  cout << "error te groot" << endl;
			//}
			if((bspos+1)>=16 && (bspos+1)<=24 && (bspos+1)%4==0 && vtpos1>-1){
				tcheck=true;
				//cout << "record: " << j << " vtpos: " << vtpos1 << endl;
				//cout << "standaard. bspos: " << bspos << endl;
				int i=0;
				while(i<=3 && tcheck){
					if(nid[i]!=-1){
						//if(vt[vtpos1].records.find(nid[i])==vt[vtpos1].records.end()){  }
						if(vt[vtpos1].size()==2){
							tcheck=false;
						}else{
							if(!bsknoop[vt[vtpos1][2]][nid[i]]){  
								tcheck=false;
								//cout << "naam " << i << " niet in knoop" << endl;
							}
						}
					}
					i++;
				}
			}
			if(tcheck){
				//cout << "goede tak" << endl;
				treesearchp(vt,vtpos1,bstmp,bspos+1,err0,err1,th,res,nid,bsknoop,pogingen);
			}
		}
	}//else{
	//  cout << "geen pad" << endl;
	//}
}

void treesearchgp(vector<vector<int> >& vt, int vtpos, bitset<BITSETSZ>& bstmp, int bspos, int err0, int err1, int th, set<int>& res, int& pogingen){
	pogingen++;
 	if(vtpos!=-1){ // als die wel -1 is dan bestaat de edge niet

	 	// In dit geval zijn we aan het einde van de vector, en dus bij een leafnode
		if(bspos==BITSETSZ){
			res.insert(vt[vtpos].begin()+2,vt[vtpos].end());
		}
		
		// zoek verder
		else{

			// De error is het maximum van de twee errors
			if(bspos%BSPART==0){
				err0 = max(err0,err1);
				err1 = max(err0,err1);
			}

			int c1=bstmp[bspos]; 		// b
			int c2=1-c1;				// 1 - b
			int vtpos1=vt[vtpos][c1];	// correct node
			int vtpos2=vt[vtpos][c2];	// incorrect node

			if(err0+c2<=th && err1+c1<=th){
				treesearchgp(vt,vtpos2,bstmp,bspos+1,err0+c2,err1+c1,th,res,pogingen);
			}      
		treesearchgp(vt,vtpos1,bstmp,bspos+1,err0,err1,th,res,pogingen);
		}
	}
}

void flatsearch(map<string,set<int> >& vecmap, set<int>& res, bitset<BITSETSZ>& bs, int pos, int nv){
	map<string,set<int> >::iterator it1;
	if(nv>=1){
		for(int i=pos;i<BITSETSZ;i++){
			bs.flip(i);
			it1=vecmap.find(bs.to_string());
			if(it1!=vecmap.end()){
				res.insert((*it1).second.begin(),(*it1).second.end());
			}
			if(nv>=2){
				flatsearch(vecmap,res,bs,i+1,nv-1);
			}
			bs.flip(i);
		}
	}
}

/**
	Print wat statistieken over de grootte van de eindknopen in de boom...
*/		
void eindknopen(vector<vector<int> >& vt){
	int eindknopen=0;
	unsigned int aantalmax=0,aantaltotaal=0;
	unsigned int tmpsize;
	map<unsigned int,unsigned int> sizemap;
	for(unsigned int vkit=0;vkit<vt.size();vkit++){
		if(vt[vkit][0]==-1 && vt[vkit][1]==-1){
			eindknopen++;
			tmpsize=vt[vkit].size()-2;
			sizemap[tmpsize]++;
			if(tmpsize>aantalmax){
				aantalmax=tmpsize;
				cout << "maximum aantal records: " << aantalmax << endl;
			}
			aantaltotaal+=tmpsize;
		}
	}
	cout << "aantal eindknopen: " << eindknopen << endl;
	cout << "totaal aantal records in eindknopen: " << aantaltotaal << endl;
	cout << "gemiddeld: " << (float)aantaltotaal/(float)eindknopen << endl;
	for(map<unsigned int,unsigned int>::iterator itsize=sizemap.begin();itsize!=sizemap.end();++itsize){
		cout << (*itsize).first << " # " << (*itsize).second << endl;
	}
}

/**
	hwin: De filenaam van het input bestand (in dit geval de kandiaat set)
	vecmap: Een map van bit string en alle registratie id's die die bitstring hebben. LET OP: dit is dus andersom dan bij de target registraties. Daar is vecmap een map van ids naar bijbehorende bitstrings...
	altmap: Een map van registratie ids en hun alternatieve ids
	recordmap2: De set van registratie ids en de bijbehorende namen (in strings)
	charid: De sets van letters en hun positie in de bit vector
	exactset: De namen in de registraties in de target set
*/
void assignVec2(ifstream& in, map<string,set<int> >& vecmap, map<int,int>& altmap, map<int,vector<string> >& recordmap2, map<char,int>& charid, set<string>& exactset){//, int th){

	string regel;
	int i=0,exact=0,altid=0;
	int gid=-1,gidoud=-1;
	bool altgid;

	bitset<BITSETSZ> bstmp;
	bitset<BSPART> bsvn1tmp;
	map<string,set<int> >::iterator it1;
	set<int> tmpset;
	vector<string> vecdata;

	while(in.is_open()){

		// lees een regel in 
		if(getline(in,regel)){
			if(i%250000==0){
				cout << i << endl;
			}
			if(regel.find("||")==string::npos){

				// splits die 
				toLower(regel);
				splits(regel,gid,vecdata,'|');

				if(exactset.find(vecdata[0])!=exactset.end()){
					// tel het aantal exacte matches
					exact++;
				}
				// zet de namen om in de bitvector
				string2bs(charid,vecdata,bstmp);

				// Als de huidige registratie id ook in de target set zit maken we een 
				// tijdelijke alternatieve id aan.
				altgid=false;
				while(recordmap2.find(gid)!=recordmap2.end()){
					gidoud=gid;
					gid=altid++;
					altgid=true;
				}
				if(altgid){
					altmap.insert(make_pair(gid,gidoud));
				}

				// Sla de registratie id en de namen op in recordmap2
				recordmap2.insert(make_pair(gid,vecdata));

				// Sla de bitstring op en voeg de registratie id toe aan de set van id's met diezelfde bitstring.
				// LET OP: dit is dus andersom dan bij de target registraties. 
				it1=vecmap.find(bstmp.to_string());
				if(it1==vecmap.end()){
					tmpset.insert(gid);
					vecmap.insert(make_pair(bstmp.to_string(),tmpset));
					tmpset.clear();
				}else{
					(*it1).second.insert(gid);
				}
			}
			i++;
		}else{
			in.close();
		}
	}
	cout << "exact: " << exact << endl;
}

void namenlv(vector<vector<int> >& vt, string bestand, map<int,set<string> >& inlv){
	ifstream bsnamen(bestand.c_str());
	string regel;
	bitset<BSPART> bsvn1;
	int vn1pos;
	set<string> tmpnamen;
	map<int,set<string> >::iterator it1;
	pair<map<int,set<string> >::iterator,bool> itbool;
	size_t pos,pos2;  
	while(getline(bsnamen,regel)){
		pos=regel.find('$');
		bsvn1 = bitset<BSPART>(regel.substr(0,pos));
		vn1pos=0;
		for(int i=0;i<BSPART;i++){
			if(vn1pos!=-1){     
				vn1pos = vt[vn1pos][bsvn1[i]];
			}else{
				cout << "bitset " << bsvn1 << " niet in boom" << endl;
			}
		}

		itbool=inlv.insert(make_pair(vn1pos,tmpnamen));   
		it1=itbool.first;
		while(pos!=string::npos){
			pos2=regel.find('$',pos+1);

			(*it1).second.insert(regel.substr(pos+1,pos2-pos-1));
			pos=pos2;

		}
	}
}

void namenvt(vector<vector<int> >& vt, map<string,bitset<BSPART> >& vn1map, map<char,int>& charid, map<int,set<string> >& inlv, map<string,vector<vtp> >& vn1knopen, int th){
	map<string,bitset<BSPART> >::iterator it1;
	vector<vtp> resvn1;
	vector<vtp>::iterator it2;
	vector<vtp> tmpvec;
	pair<map<string,vector<vtp> >::iterator,bool> itbool;
	map<string,vector<vtp> >::iterator it3;
	for(it1=vn1map.begin();it1!=vn1map.end();++it1){
		resvn1.clear();   

		treesearchvn1(vt,0,(*it1).second,0,0,0,th,resvn1,(*it1).second);
		itbool=vn1knopen.insert(make_pair((*it1).first,tmpvec));
		it3=itbool.first;
		int il=0,nil=0;
		for(it2=resvn1.begin();it2!=resvn1.end();++it2){

			if(inlv[(*it2).pos].find((*it1).first)!=inlv[(*it2).pos].end()){
				(*it3).second.push_back((*it2));          
				il++;         
			}else{
				nil++;
			}
		}

	}
}

void recheck(string bestand, vector<vector<int> >& vt, map<string,set<int> >& vecmap, vector<bitset<300> >& knoopbs, map<char,int>& charid, int th){
	map<string,set<int> >::iterator it1;
	ifstream fin(bestand.c_str());
	string regel;
	vector<string> vecdata;
	int i=0,nrecheck=0;
	set<int> res;
	vector<int> nid;
	int gid;
	bitset<BITSETSZ> bstmp;
	map<string,set<int> > recheck;
	while(getline(fin,regel)){
		if(i%100000==0){
			cout << i << endl;
			cout << "recheck: " << nrecheck << endl;
		}
		res.clear();
		toLower(regel);
		splits(regel,gid,vecdata,'|');
		string2bs(charid,vecdata,bstmp);

		it1=vecmap.find(bstmp.to_string());
		if(it1!=vecmap.end()){
			res.insert((*it1).second.begin(),(*it1).second.end());
		}

		int pogingen=0;
		if(recheck.find(bstmp.to_string())==recheck.end()){
			treesearchp(vt,0,bstmp,0,0,0,th,res,nid,knoopbs,pogingen);
			recheck.insert(make_pair(bstmp.to_string(),res));
		}else{
			nrecheck++;
		}
		i++;
	}
}

/**
	Loopt voor elke registratie in de kandidaten set de vectortree af en maakt een set van potentieel matchende 
	target registraties op basis van de bitvector. 
	Zie algoritme 4.2 op bladzijde 54.

	Vervolgens gaat het van alle potentiele matches de levenhstein afstand bepalen.
*/
void checkr2(vector<vector<int> >& vt, map<string,set<int> >& vecmap, map<int,vector<string> >& recordmap, map<int,vector<string> >& recordmap2, map<int,int>& altmap, int th, int lvth, string bestand, vector<map<string,int> >& naamid,vector<bitset<300> >& knoopbs, bool pruning, bool verbose){
	time_t tstart,tstart2,tend;
	ofstream fout(bestand.c_str()); //open outputbestand
	
	// en schrijf de header
	fout << "dist|id_1|lname_hw_m|fname_hw_m|lname_hw_f|fname_hw_f|id_2|lname_p_m|fname_p_m|lname_p_f|fname_p_f" << endl;
	
	map<string,set<int> >::iterator it1; 	// iterator voor de vecmap --> dus de set van bitstrings en bijbehorende kandidaat registratie id's
	set<int>::iterator it2; 				// iterator voor alle kandidaat registraties id's die horen bij de bitvector in iterator 1: de bitvector die we aan het bekijken zijn.
	//map<string,vector<vtp> >::iterator it3;
	map<string,set<int> >::iterator it3;	// iterator voor
	vector<vtp>::iterator it4;				// iterator voor
	set<int>::iterator it5;					// iterator voor het doorlopen van de resultaten set per vector
	map<int,int>::iterator it6;				// iterator voor het zoeken van de alternatieve id's in altmap
	vector<vtp>::iterator it7;				// iterator voor
	map<string,int>::iterator it8;			// iterator voor
	
	bitset<BITSETSZ> bs2;					// Hierin word de string van de huidige bitvector die we aan het matchen zijn opgeslagen
	set<int> res;
	vector<string> recdata;					// de namen van de registratie van iterator 2

	string r1,r2,vn1;	// r1 is de string van namen van iterator 2, 
	int cdn=0;
	int gid_ouders;  	// de registratie id van de target registraties
	int j=0;
	int j2=10000;		// Om de j2 aantal iteraties: schrijf wat statistieken naar stdout
	int lv;				// de edit distance tussen de huidige registraties
	int pogingen=0;
	int recordn=0;
	
	vector<int> nid(4,0);
	vector<int> plvlm (8,0);
	vector<int> plvlp (8,0);

	time(&tstart);
	time(&tstart2);
	
	cout << "grootte vecmap: " << vecmap.size() << endl;
	
	for(it1=vecmap.begin();it1!=vecmap.end();++it1){
		
		// Wat statistieken over de afgelopen 10000 records
		if(j%j2==0){
			time(&tend);
			cout << j << endl;
			if(j>0){

				cout << "kandidaten per bs: " << cdn/recordn << endl;

				cdn=0;
				recordn=0;

				cout << "tijd vanaf tstart: " << difftime(tend,tstart) << endl;
				cout << "tijd vanaf tstart2: " << difftime(tend,tstart2) << endl;
				time(&tstart2);
				cout << "gemiddeld aantal pogingen: " << pogingen/j2 << endl;
				pogingen=0;
			}
		}
		
		bs2=bitset<BITSETSZ>(string((*it1).first)); // De huidige bitvector

		// Loop over alle registratie id's die bij de huidige bitvector horen
		for(it2=(*it1).second.begin();it2!=(*it1).second.end();++it2){
			// maak de resultaatset leeg...
			res.clear();

			if (verbose) {
				cout << *it2 << endl;
			}

			// haal de strings op van het registratie id in iterator 2
			recdata=recordmap2[(*it2)];
			r1=recdata[0]; // de hele string


			if (verbose) {
				cout << "r1: " << r1 << endl;
			}
			
			if(pruning){
				it8=naamid[0].find(recdata[1]);
				nid[0] = (it8!=naamid[0].end()) ? (*it8).second : -1;
				if(verbose) cout << "naamid[" << recdata[1] << "]=" << nid[0] << endl;
				it8=naamid[2].find(recdata[2]);
				nid[1] = (it8!=naamid[2].end()) ? (*it8).second : -1;
				if(verbose) cout << "naamid[" << recdata[2] << "]=" << nid[1] << endl;
				it8=naamid[1].find(recdata[3]);
				nid[2] = (it8!=naamid[1].end()) ? (*it8).second : -1;
				if(verbose) cout << "naamid[" << recdata[3] << "]=" << nid[2] << endl;
				it8=naamid[2].find(recdata[4]);
				nid[3] = (it8!=naamid[2].end()) ? (*it8).second : -1;
			}

			if(pruning){
				treesearchp(vt,0,bs2,0,0,0,th,res,nid,knoopbs,pogingen);
			}else{
				/**
					vt: de vectortree
					0: de beginknoop
					bs2: de bitvector die we gaan matchen
					0: de positie in de bitvector
					0: error 0
					0: error 1
					th: de threshold
					res: de set van resultaten --> Dit zijn dus alle potentiele kandidaten
					pogingen: het aantal pogingen

					Zie Algorithm 4.2 blz 54
				*/
				treesearchgp(vt,0,bs2,0,0,0,th,res,pogingen);
			}

			if(verbose) cout << "aantal kandidaten: " << res.size() << endl;

			cdn+=res.size();
			recordn++;      
			//cin.get();      
			int i2=1;

			for(it5=res.begin();it5!=res.end();it5++){

				 r2=recordmap[(*it5)][0]; // de string van de target registratie
				 lv=levenshtein(r1,r2,lvth);

				 i2++;

				 if (lv>-1 && lv<=lvth) {
					// een match!
					 it6=altmap.find((*it2));  // zoek de alternatieve id
					 if(it6!=altmap.end()){
						 gid_ouders=(*it6).second;
					 }else{
						 gid_ouders=(*it2);
					 }

					 // Hier de output naar het output-bestand...
					 fout << lv << "|" << (*it5) << "|" << r2 << gid_ouders << "|" << r1 << endl;
				 }
			}   
		}
		j++;
	}

}

/**
	Maakt een map met per naam een set van namen met een afstand van 3
*/
void lvset(string bestand, map<string,set<string> >& lvmap){
	ifstream lv3(bestand.c_str());
	map<string,set<string> >::iterator it1;
	string regel1,naam1,naam2;  
	size_t pos; 
	set<string> tmpset1;  
	int i1=0; 
	while(getline(lv3,regel1)){
		toLower(regel1);
		if(i1%500000==0) cout << i1 << endl;
		i1++;   
		pos=regel1.find('_');
		naam1=regel1.substr(0,pos);
		naam2=regel1.substr(pos+1);
		it1=lvmap.find(naam1);    
		if(it1==lvmap.end()){
			tmpset1.insert(naam2);
			lvmap.insert(make_pair(naam1,tmpset1));
			tmpset1.clear();
		}else{
			(*it1).second.insert(naam2);
		}
	}
	lv3.close();
	cout << "bestand: " << bestand << endl;
	cout << "aantal namen: " << lvmap.size() << endl;
}

/**
	...
*/
void naamids(string bestand, map<string,int>& naamid, int th, int mp){
	ifstream fin(bestand.c_str());
	int i=0;
	string regel;
	while(abs(i)<th){
		getline(fin,regel);
		toLower(regel);
		naamid.insert(make_pair(regel,i+(mp*th)));
		i++;
	}
	fin.close();
	cout << "bestand: " << bestand << endl;
	cout << "aantal namen: " << naamid.size() << endl;
}


int main(){

	bool verbose = false;


	map<char,int> charid;
	initCharId2(charid); // Maakt een map aan van de letters die op dezelfde positie in de vector komen

	int th=5;
	int lvth=5;
	bool pruning=false;

	//uitvoerbestand
	string matchbestand="../../datafiles/output_vectortree/huwelijk_overlijden/matches_links_exact_lvl_5_overlijden.csv";
	
	// inputfiles
	string input_targets = "../../datafiles/links/input/targets.csv";
	string input_candidates = "../../datafiles/links/input/overlijden_ouders.csv";
	
	
	cout << "lv<=" << th << endl;
	cout << "subjectsbestand: " << input_targets << endl;
	cout << "oudersbestand:" << input_candidates << endl;
	cout << "resultaatbestand: " << matchbestand << endl;
	cout << "pruning: " << pruning << endl;
	cout << "lv<=th per naam..." << endl;

	// Maps die pruning data bevatten
	map<string,set<string> > lv3map1;
	vector<map<string,set<string> > > lv3map4 (3,lv3map1);
	
	map<string,int> naamid1;
	vector<map<string,int> > naamid4 (3,naamid1);
	
	if(pruning){
		cout << "We gebruiken pruning" << endl;
		naamids("../../datafiles/data_marijn/vnmanhw.txt",naamid4[0],100,0);
		naamids("../../datafiles/data_marijn/vnvrouwhw.txt",naamid4[1],100,1);
		naamids("../../datafiles/data_marijn/fnhw.txt",naamid4[2],100,2);
	
		lvset("../../datafiles/data_marijn/vnmanlv3top100.txt",lv3map4[0]);
		lvset("../../datafiles/data_marijn/vnvrouwlv3top100.txt",lv3map4[1]);
		lvset("../../datafiles/data_marijn/fnlv3top100.txt",lv3map4[2]);
	} 
	cout << "ok." << endl;

	// Dit zijn alleen maps voor de target registraties....!!!
	map<int,bitset<BITSETSZ> > gidmap;  // dit wordt de map van registratie ids met de bijbehorende bitvectors
	map<int,vector<string> > recordmap; // dit wordt de map van registratie ids met de bijbehorende strings van namen

	cout << "bereken bitsets voor subjects..." << endl;
	
	// Lees het subject bestand in en maak de bitvectors voor de namen in het bestand
	assignVec(input_targets, gidmap, recordmap, charid);
	cout << "ok." << endl;
	
	cout << "maak vectorboom..." << endl;

	// vt = de vectortree
	vector<vector<int> > vt;
	vectree(gidmap,vt);
	cout << "ok." << endl;
	
	int bladknopen=0, bladrecords=0;
	for(unsigned int vi=0;vi<vt.size();vi++){
		if(vt[vi].size()>2){
			bladknopen++;
			bladrecords+=vt[vi].size()-2;
		}
	}
	
	cout << "aantal bladknopen: " << bladknopen << " aantal records: " << bladrecords << endl;
	cout << "gemiddeld aantal records in bladknopen: " << (float)bladrecords/(float)bladknopen << endl;
	eindknopen(vt); // nog wat meer statistieken. Kan ook worden geskipt.
	
	//cin.get();
	cout << "exactset..." << endl;
	set<string> exactset;
	mexactset(recordmap,exactset); // exactset is de set van alle input strings --> Alle 4 de namen op een acte
	cout << "ok." << endl;
		
	// Dit is alleen relevant wanneer je pruning gebruikt... Doe ik niet.	
	cout << "bereken knopen per naam..." << endl;
	map<string,set<int> > vtlv3;
	
	vector<bitset<300> > knoopbs;
	if(pruning){
		vn1knopen(vt,gidmap,lv3map4,recordmap,naamid4,knoopbs,16,24,4);
	}
	cout << "ok." << endl;
	
	cout << "lv3map4 capacity (voor clear): " << lv3map4.capacity() << endl;
	lv3map4.clear();
	
	cout << "lv3map4 capacity (na clear): " << lv3map4.capacity() << endl;
	//vector<lv3map4.swap(lv3map4);
	
	vector<map<string,set<string> > >().swap(lv3map4);
	cout << "lv3map4 capacity (na swap): " << lv3map4.capacity() << endl;
	map<int,bitset<BITSETSZ> >().swap(gidmap);

	ifstream hwin(input_candidates.c_str());
	int hwi=1;

	cout << "bereken bitsets voor hwouders (" << hwi << ")..." << endl;
	hwi++;
	map<string,set<int> > vecmap;
	map<int,int> altmap;
	map<int,vector<string> > recordmap2;

	/**
		hwin: De filenaam van het input bestand (in dit geval de kandiaat set)
		vecmap: Een map van bit string en alle registratie id's die die bitstring hebben. LET OP: dit is dus andersom dan bij de target registraties. Daar is vecmap een map van ids naar bijbehorende bitstrings...
		altmap: Een map van registratie ids en hun alternatieve ids
		recordmap2: De set van registratie ids en de bijbehorende namen (in strings)
		charid: De sets van letters en hun positie in de bit vector
		exactset: De namen in de registraties in de target set
	*/
	assignVec2(hwin,vecmap,altmap,recordmap2,charid,exactset);//,1000000);
	cout << "ok." << endl;
	cout << "vecmap: " << vecmap.size() << endl;

	cout << "treesearch..." << endl;
	/**
		vt: De opgebouwde vectortree
		vecmap: De map van bitstrings en de bijbehorende registratie id's (van de kandidaat registraties)
		recordmap: De id's van de target registraties en de strings van namen
		recordmap2: De id's van de kandiaat registraties en de string van namen
		altmap: De map van originele kandidaat id's en de alternatieve id's
		th: De threshold voor het afkappen van zoeken in takken
		lvth: De maximum edit distance
		matchbestand: De bestandsnaam van het uitvoerbestand
		naamid4: Pruning set van namen. Gebruik ik niet
		knoopbs: Pruning set van het eoa. Geen idee wat dit is, maar ik gebruik geen pruning...
		pruning: Boolean die aangeeft of pruning wordt gebruikt.
		verbose: Boolean die aangeeft of er meer info naar stdout moet worden geprint.
	*/
	checkr2(vt,vecmap,recordmap,recordmap2,altmap,th,lvth,matchbestand,naamid4,knoopbs,pruning, verbose);
	vecmap.clear();
	recordmap2.clear();
	altmap.clear();

	return(0);
}
