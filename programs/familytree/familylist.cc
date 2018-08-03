#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "levenshtein.h"

using namespace std;

struct prec{
	int naam;
	int gid;
	short int jaar;
	int latlon;
	char match;
	prec(int n1, int gd1, short int jr1, char mt1, int lt1):naam(n1),gid(gd1),jaar(jr1),latlon(lt1),match(mt1){}
	prec(){
		naam=-1;
		gid=-1;
		jaar=-1;
		latlon=0;
		match=0;
	}
};

int ilatlon(string& lat, string& lon){
	int res=0;
	size_t pos=lat.find('.');
	int n,f;
	n=atoi(lat.substr(0,pos).c_str());
	f=atoi(lat.substr(pos+1,2).c_str());
	res |= n;
	res <<= 8;
	res |= f;
	res <<= 8;
	pos=lon.find('.');
	n=atoi(lon.substr(0,pos).c_str());
	f=atoi(lon.substr(pos+1,2).c_str());
	res |= n;
	res <<= 8;
	res |= f;
	return(res);
}

void split(string rg,vector<string>& velden, const char SEP){
	velden.clear();
	if(!rg.empty() && *rg.rbegin() == '\r') {			// Windows-bestanden gelezen in Linux
    rg.erase( rg.length()-1, 1);
	}
	size_t pos1=0;
	size_t pos2=rg.find(SEP,pos1);
	while(pos2!=string::npos){
		velden.push_back(rg.substr(pos1,pos2-pos1));
		pos1=pos2+1;
		pos2=rg.find(SEP,pos1);
	}
	velden.push_back(rg.substr(pos1));
}

template <class T>
void openfile(T& fs, string fnaam, bool& bo){
	fs.open(fnaam.c_str());
	if(!fs.is_open()){
		cout << "The file \"" << fnaam << "\" cannot be opened." << endl;
		bo=false;
	}
}

int main(int argc, const char* argv[]){
	const char SEP = (argc<5) ? '|' : argv[4][0];
	const int JAARTH = (argc<6) ? 20 : atoi(argv[5]);
	
	//ifstream gbin("/local/Links/data/dec11/geboorteaktenlatlon.txt");
	//ofstream gzuit("/local/Links/data/dec11/gezinnen.txt");
	//ofstream gz2uit("/local/Links/data/dec11/gezinnenu.txt");
	//ofstream urefuit("/local/Links/data/dec11/unaamref.txt");
	// resdir = /local/Links/data/dec11
	
	ifstream gbin, hwin;
	ofstream refuit, urefuit, gzuit, gz2uit;
	string resdir(argv[3]);
	bool openb = true;
	openfile<ifstream>(gbin, argv[1], openb);				//gbakten6.txt
	openfile<ifstream>(hwin, argv[2], openb);				//hwakten4.txt
	openfile<ofstream>(refuit, resdir+"/naamref.txt", openb);
	openfile<ofstream>(urefuit, resdir+"/unaamref.txt", openb);
	openfile<ofstream>(gzuit, resdir+"/gezinnen.txt", openb);
	openfile<ofstream>(gz2uit, resdir+"/gezinnenu.txt", openb);
	if(!openb){
		return(1);
	}
	
	multimap<string,int> naamref;
	map<string,int> naamrefuniek;
	map<string,int>::iterator itvu,itmu;
	multimap<string,int>::iterator itnaam1,itnaam2;
	pair<multimap<string,int>::iterator,multimap<string,int>::iterator> itrnaam1,itrnaam2;
	int naamid=1;
	multimap<pair<int,int>,prec> gezinnen;
	multimap<pair<int,int>,prec>::iterator itg;
	pair<multimap<pair<int,int>,prec>::iterator,multimap<pair<int,int>,prec>::iterator> itrgz;
	multimap<pair<int,int>,pair<int,int> > gezinnen2;
	multimap<pair<int,int>,pair<int,int> >::iterator itg2;
	string regel;
	vector<string> velden;
	int gid;
	int gbnamen[] = {6,7,9,10,11,12};		//niet leeg: namen kind, vader, moeder
	vector<int> vgbnamen(gbnamen,gbnamen+6);
	vector<int>::iterator it1;
	int n=0;
	int nleeg=0;
	bool leeg;
	string geb,vader,moeder;
	string gebdatum;
	int gebjaar;
	prec p1;
	int idgeb=-1,idvader=-1,idmoeder=-1,idvaderu=-1,idmoederu=-1;
	bool idnieuw;
	string lat,lon;
	bool gzcheck;
	pair<multimap<pair<int,int>,pair<int,int> >::iterator,multimap<pair<int,int>,pair<int,int> >::iterator> itrgz2;
	multimap<pair<int,int>,pair<int,int> >::iterator itgz2;
	cout << "reading birth certificates..." << endl;
	while(getline(gbin,regel)){
		if(n%50000==0) cout << n << endl;
		n++;
		split(regel,velden,SEP);
		
		for(it1=vgbnamen.begin();it1!=vgbnamen.end();it1++){
			if(velden[(*it1)]==""){
				leeg=true;
			}
		}
		if(!leeg){
			gid=atoi(velden[0].c_str());
		
			geb = velden[6]+"|"+velden[7];
			lat = velden[4];
			lon = velden[5];
			idgeb = naamid;
			naamref.insert(make_pair(geb,naamid));
			naamid++;
			gebdatum = (velden[8]!="") ? velden[8] : velden[1];
			if(gebdatum.length()==10){
				gebjaar = atoi(gebdatum.substr(6,4).c_str());
			}else{
				cout << "no certificate date or incomplete birth date: " << endl;
				cout << regel << endl;
				gebjaar=0;
			}
			vader = velden[9]+"|"+velden[10];
			moeder = velden[11]+"|"+velden[12];
			itrnaam1=naamref.equal_range(vader);
			itrnaam2=naamref.equal_range(moeder);
			idnieuw=true;
			for(itnaam1=itrnaam1.first;itnaam1!=itrnaam1.second;++itnaam1){
				for(itnaam2=itrnaam2.first;itnaam2!=itrnaam2.second;++itnaam2){
					itrgz=gezinnen.equal_range(make_pair((*itnaam1).second,(*itnaam2).second));
					if(itrgz.first!=itrgz.second){				//ouders gevonden in gezinnen
						for(itg=itrgz.first;itg!=itrgz.second;++itg){
							if(abs((*itg).second.jaar-gebjaar)<=JAARTH){
								idnieuw=false;
								idvader=(*itg).first.first;
								idmoeder=(*itg).first.second;
							}
						}
					}
				}
			}
			if(idnieuw){
				idvader = naamid;
				naamref.insert(make_pair(vader,naamid));
				naamid++;
				idmoeder = naamid;
				naamref.insert(make_pair(moeder,naamid));
				naamid++;						
			}
			itvu=naamrefuniek.find(vader);
			itmu=naamrefuniek.find(moeder);
			if(itvu==naamrefuniek.end()){
				naamrefuniek.insert(make_pair(vader,idvader));
				idvaderu=idvader;
			}else{
				idvaderu=(*itvu).second;
			}
			if(itmu==naamrefuniek.end()){
				naamrefuniek.insert(make_pair(moeder,idmoeder));
				idmoederu=idmoeder;
			}else{
				idmoederu=(*itmu).second;
			}
			gzcheck=false;
			itrgz2=gezinnen2.equal_range(make_pair(idvaderu,idmoederu));
			for(itgz2=itrgz2.first;itgz2!=itrgz2.second;++itgz2){
				if((*itgz2).second == make_pair(idvader,idmoeder)){
					gzcheck=true;
				}
			}
			if(!gzcheck){
				gezinnen2.insert(make_pair(make_pair(idvaderu,idmoederu),make_pair(idvader,idmoeder)));
			}
			
			p1.naam=idgeb;
			p1.gid=gid;
			p1.jaar=gebjaar;
			p1.match=0;
			p1.latlon=ilatlon(lat,lon);
			gezinnen.insert(make_pair(make_pair(idvader,idmoeder),p1));
		}else{
			nleeg++;
		}
		leeg=false;
	}
	cout << "number of birth certificates: " << n << endl;
	cout << "incomplete certificates: " << nleeg << endl;
	cout << "complete certificates: " << n-nleeg << endl;
	pair<int,int> refkey=make_pair(-1,-1);
	int ngezin=0;
	for(itg=gezinnen.begin();itg!=gezinnen.end();++itg){
		if((*itg).first!=refkey){
			ngezin++;
			refkey=(*itg).first;
		}
	}
	cout << "number of families: " << ngezin << endl;
	gbin.close();
	for(multimap<string,int>::iterator itref=naamref.begin();itref!=naamref.end();++itref){
		refuit << (*itref).first << "|" << (*itref).second << endl;
	}
	refuit.close();
	cout << "finished reading birth certificates." << endl;
	
	string bg,br;
	
	int hwnamen[] = {2,3,4,5};
	vector<int> vhwnamen(hwnamen,hwnamen+4);
	leeg=false;
	int hwmatch=0,hwgeenmatch=0,gebmatch=0,gebgeenmatch=0;
	string hwdatum;
	int hwjaar=0;
	bool gevonden=false;
	n=0;
	cout << "reading marriage certificates..." << endl;
	while(getline(hwin,regel)){
		if(n%50000==0) cout << n << endl;
		n++;
		split(regel,velden,SEP);

		for(it1=vhwnamen.begin();it1!=vhwnamen.end();it1++){
			if(velden[(*it1)]==""){
				leeg=true;
			}
		}
		if(!leeg){
			gid=atoi(velden[0].c_str());

			bg = velden[2]+"|"+velden[3];
			br = velden[4]+"|"+velden[5];
			hwdatum = velden[1];
			if(hwdatum.length()==10){
				hwjaar = atoi(hwdatum.substr(6,4).c_str());
			}else{
				cout << "incomplete or missing certificate date: " << endl;
				cout << regel << endl;
				hwjaar=0;
			}
			itrnaam1=naamref.equal_range(bg);
			itrnaam2=naamref.equal_range(br);
			gevonden=false;
			for(itnaam1=itrnaam1.first;itnaam1!=itrnaam1.second;++itnaam1){
				for(itnaam2=itrnaam2.first;itnaam2!=itrnaam2.second;++itnaam2){
					itrgz=gezinnen.equal_range(make_pair((*itnaam1).second,(*itnaam2).second));
					if(itrgz.first!=itrgz.second){				//ouders gevonden in gezinnen
						for(itg=itrgz.first;itg!=itrgz.second;++itg){
							if(abs((*itg).second.jaar-hwjaar)<=JAARTH){
								gevonden=true;
								hwmatch++;
								(*itg).second.match=1;
							}
						}
					}
				}
			}
			if(!gevonden){
				hwgeenmatch++;
			}
		}else{
			nleeg++;
		}
		leeg=false;
	}
	hwin.close();
	cout << "finished reading marriage certificates." << endl;
	cout << "writing output files..." << endl;
	for(itg=gezinnen.begin();itg!=gezinnen.end();++itg){
		gzuit << (*itg).first.first << "|" << (*itg).first.second << "|" << (*itg).second.gid << "|" << (*itg).second.naam << "|" << (*itg).second.jaar << "|" << (*itg).second.latlon << "|" << (int)(*itg).second.match << endl;
		if((*itg).second.match==1){
			gebmatch++;
		}else{
			gebgeenmatch++;
		}
	}
	for(itg2=gezinnen2.begin();itg2!=gezinnen2.end();++itg2){
		gz2uit << (*itg2).first.first << "|" << (*itg2).first.second << "|" << (*itg2).second.first << "|" << (*itg2).second.second << endl;
	}
	for(map<string,int>::iterator itref=naamrefuniek.begin();itref!=naamrefuniek.end();++itref){
		urefuit << (*itref).first << "|" << (*itref).second << endl;
	}
	gzuit.close();
	gz2uit.close();
	urefuit.close();
	cout << "finished writing output files." << endl;
	cout << "corresponding parent names (marriage): " << hwmatch << endl << "not found: " << hwgeenmatch << endl;
	cout << "corresponding parent names (birth): " << gebmatch << endl << "not found: " << gebgeenmatch << endl;

	return(0);
}
