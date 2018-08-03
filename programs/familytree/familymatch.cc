#include <map>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include "levenshtein.h"

using namespace std;

const char SEP = '|';
const int JAARTH = 20;
const int JAARTH2 = 10;
const int JAARTH3 = 10;
const int LVTH=4;
const int DTH=75;
const int OVDTH=50;

const double earthdiameter=12745.595121712;
const double pirad2=0.00872664625997164788461;
const double pirad=0.0174532925199432957692;

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

struct pr3{
	string naam;
	short int jaar;
	int latlon;
	int gid;
	char hwgb;
};

struct pr3cmp {								//ToDo: equal bij gelijke jaren (!)
  bool operator() (const pr3& lhs, const pr3& rhs) const
  {return lhs.jaar < rhs.jaar;}
};

struct ov3{
	int gid;
	int latlon;
	short int jaar;
	ov3(int gd1, int jr1, int lt1): gid(gd1), jaar(jr1), latlon(lt1){}
};

void split(string rg,vector<string>& velden){
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

int dlatlon(int latlon1,int latlon2){
	int lon1f=latlon1 & 255;
	latlon1>>=8;
	int lon1n=latlon1 & 255;
	latlon1>>=8;
	int lat1f=latlon1 & 255;
	latlon1>>=8;
	int lat1n=latlon1 & 255;
	int lon2f=latlon2 & 255;
	latlon2>>=8;
	int lon2n=latlon2 & 255;
	latlon2>>=8;
	int lat2f=latlon2 & 255;
	latlon2>>=8;
	int lat2n=latlon2 & 255;
	float lat1=lat1n+((float)lat1f/100);
	float lon1=lon1n+((float)lon1f/100);
	float lat2=lat2n+((float)lat2f/100);
	float lon2=lon2n+((float)lon2f/100);
	
	float radlat1=lat1*pirad;
	float radlat2=lat2*pirad;
	float deltaradlon=(lon2-lon1)*pirad2;
	float deltaradlat=(lat2-lat1)*pirad2;
	float a=sin(deltaradlat);
	float b=sin(deltaradlon);
	float h=(a*a)+cos(radlat1)*cos(radlat2)*(b*b);
	float distkm=atan2(sqrt(h),sqrt(1.0-h))*earthdiameter;
	return((int)distkm);
}

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
	
void split2(const string& in, string& p2){
	size_t pos=in.find(SEP);
	p2 = (pos==in.size()-1) ? "" : in.substr(pos+1);
}

void split12(const string& in, string& p1, string& p2){
	size_t pos=in.find(SEP);
	p1 = in.substr(0,pos);
	p2 = (pos==in.size()-1) ? "" : in.substr(pos+1);
}

bool gzmatch(string& bgbr, short int hwjaar, string& oudersgb, multiset<pr3,pr3cmp>& hwgezin, multiset<pr3,pr3cmp>& gbgezin, int latlonhw, map<int,string>& reflatlon, multimap<string,ov3>& ovl, bool ovcheck, string& ouderov){
	int lv;
	short int djaar;
	short int gbjaar=(*gbgezin.begin()).jaar;
	int latlongb=(*gbgezin.begin()).latlon;
	int afstand;
	if(hwgezin.empty()){
		//cout << "geen kinderen hw" << endl;
		lv=levenshtein(bgbr,oudersgb,100);
		djaar=abs(hwjaar-gbjaar);
		afstand=dlatlon(latlonhw,latlongb);
		//cout << "huwelijk: " << bgbr << "|" << hwjaar << "|" << reflatlon[latlonhw] << endl;
		//cout << "geboorte: " << oudersgb << "|" << gbjaar << "|" << reflatlon[latlongb] << endl;
		//cout << "lv=" << lv << endl;
		//cout << "verschil jaren=" << djaar << endl;
		//cout << "afstand=" << afstand << "km" << endl; 
		if(lv<=LVTH && djaar<=JAARTH2 && afstand<=DTH){
			return(true);
		}else{
			return(false);
		}
	}else{
		//cout << "wel kinderen hw" << endl;
		afstand=dlatlon(latlonhw,latlongb);
		if(afstand>20){
			//cout << "gezin ok, afstand te groot (" << reflatlon[latlonhw] << "-" << reflatlon[latlongb] << " = " << afstand << " km)" << endl;
			//cin.get();
			return(false);
		}
		
		multiset<pr3,pr3cmp> gz2 = hwgezin;
		multiset<pr3,pr3cmp>::iterator it1,it2;
		gz2.insert(gbgezin.begin(),gbgezin.end());
		pair<multimap<string,ov3>::iterator,multimap<string,pair<short int,int> >::iterator> itrov;
		multimap<string,pair<short int,int> >::iterator itov;
		it1=gz2.begin();
		it2=gz2.begin();
		it2++;
		bool dgb=true;
		while(it2!=gz2.end() && dgb){
			//cout << "kind: " << (*it1).jaar << "|" << (*it1).naam << "|" << reflatlon[(*it1).latlon] << endl;
			//cout << "volgende kind: " << (*it2).jaar << "|" << (*it2).naam << "|" << reflatlon[(*it2).latlon] << endl;
			if((*it2).jaar-(*it1).jaar==0 || (*it2).jaar-(*it1).jaar>JAARTH3){
				dgb=false;
			}
			if(dgb && ovcheck){
				if((*it1).hwgb!=(*it2).hwgb){
					itrov=ovl.equal_range(ouderov);
					dgb=false;
					for(itov=itrov.first;itov!=itrov.second;++itov){
						if(((*itov).second.first >= (*it1).jaar) && ((*itov).second.first <= (*it2).jaar)){
							if(
							dgb=true;
							
							//cout << "kind: " << (int)(*it1).hwgb << "|" << (*it1).jaar << "|" << (*it1).naam << "|" << reflatlon[(*it1).latlon] << endl;
							//cout << "volgende kind: " << (*it2).jaar << "|" << (*it2).naam << "|" << reflatlon[(*it2).latlon] << endl;
							//cout << "overlijden gevonden: " << (*itov).second << "|" << (*itov).first << endl;
						}
					}
					/*if(!dgb){
						cout << "overlijden niet gevonden (" << ouderov << ")" << endl;
					}
					cin.get();*/
				}
			}
			it1++;
			it2++;
		}
		return(dgb);
	}
}

void checkbgbr(string& vm, int bgpos, pair<int,int> ogz, multimap<string,pr3>& bgbr, multimap<pair<int,int>,prec>& gezinnen, multimap<pair<int,int>,pair<int,int> >& gezinnen2, multimap<string,pair<short int,int> >& ovl, multimap<string,int>& naamref, map<string,int>& naamrefuniek, map<int,string>& revnaam, map<int,string>& reflatlon,int& ngzmatch, int& ngzmatch0, int& cbtotaal, int& kandidaten, int& matchgezin, int& matchgeengezin, int& matchov, ofstream& muit){
	multimap<pair<int,int>,prec>::iterator itg2;
	pair<multimap<pair<int,int>,prec>::iterator,multimap<pair<int,int>,prec>::iterator> itrgz,itrgz2;
	pair<multimap<pair<int,int>,pair<int,int> >::iterator,multimap<pair<int,int>,pair<int,int> >::iterator> itrgzp;
	multimap<pair<int,int>,pair<int,int> >::iterator itgzp;
	pair<multimap<string,pr3>::iterator,multimap<string,pr3>::iterator> itrbgbr;
	multimap<string,pr3>::iterator itbgbr;
	multimap<string,int>::iterator itnaam1,itnaam2;
	map<string,int>::iterator itnaam1u, itnaam2u;
	pair<multimap<string,int>::iterator,multimap<string,int>::iterator> itrnaam1,itrnaam2;
	
	int lv;
	bool check0,ovcheck,hwgv;
	
	multiset<pr3,pr3cmp> gbgezin,hwgezin;
	pr3 tpersoon;
	string oudershw,oudersgb;
	string vader,moeder,bg,br;
	string ouderov;
	
	vader=revnaam[ogz.first];
	moeder=revnaam[ogz.second];
	oudersgb=vader+"|"+moeder;
	
	itrbgbr=bgbr.equal_range(vm);
	if(itrbgbr.first!=itrbgbr.second){
		itrgz2=gezinnen.equal_range(ogz);
		for(itg2=itrgz2.first;itg2!=itrgz2.second;++itg2){
			tpersoon.naam=revnaam[(*itg2).second.naam];
			tpersoon.jaar=(*itg2).second.jaar;
			tpersoon.latlon=(*itg2).second.latlon;
			tpersoon.hwgb=1;
			tpersoon.gid=(*itg2).second.gid;
			gbgezin.insert(tpersoon);
		}
		if(bgpos==0){
			bg=vm;
			itnaam1u=naamrefuniek.find(bg);
		}else{
			br=vm;
			itnaam2u=naamrefuniek.find(br);
		}
	}
	//cout << "geb (vader): " << checkf << endl;
	//cout << "vader (geb): " << vader << " moeder (geb): " << moeder << endl;
	for(itbgbr=itrbgbr.first;itbgbr!=itrbgbr.second;++itbgbr){
		
		if(bgpos==0){
			br=(*itbgbr).second.naam;
		}else{
			bg=(*itbgbr).second.naam;
		}
		//bg = (bgpos==0) ? (*itbgbr).first : (*itbgbr).second.naam;
		//br = (bgpos==0) ? (*itbgbr).second.naam : (*itbgbr).first;
							
		oudershw = bg+"|"+br;
		lv=levenshtein(oudersgb,oudershw,100);
		//cout << "kandidaat: " << (*itbgbr).second.jaar << "|" << oudershw << endl;
		if(lv<=100){
			//if(kandidaten==0){
				//cout << "geen match: " << (*itg).first.first << "," << (*itg).first.second << "," << (*itg).second.naam << endl;
				//cout << vader << "|" << moeder << endl << "kind: " << geb << endl;
				//cout << "geboortejaar: " << (*itg).second.jaar << endl;
				//if(distance(itrgz2.first,itrgz2.second)>1){
					//cout << "gezin (geb):" << endl;
					//for(itg2=itrgz2.first;itg2!=itrgz2.second;++itg2){
						//cout << "\t" << (*itg2).second.jaar << "|" << revnaam[(*itg2).second.naam] << endl;
					//}
				//}
			//}
			//cout << "huwelijk: " << oudershw << "|" << (*itbgbr).second.jaar << endl;
			//cout << "afstand (lv): " << lv << endl;
			//cout << "afstand (jaren): " << abs((*itg).second.jaar-(*itbgbr).second.jaar) << endl;
			//itrnaam1=naamref.equal_range(bg);							//bg
			//itrnaam2=naamref.equal_range(br);				//br
			int gezincheck=0;
			
			if(bgpos==0){
				itnaam2u=naamrefuniek.find(br);
			}else{
				itnaam1u=naamrefuniek.find(bg);
			}
			
			//itnaam1u=naamrefuniek.find(bg);
			//itnaam2u=naamrefuniek.find(br);
			check0=false;
			//cout << "check: " << bg << "|" << br << endl;
			if(itnaam1u!=naamrefuniek.end() && itnaam2u!=naamrefuniek.end()){						//bg en br gevonden in naamref geb
				itrgzp=gezinnen2.equal_range(make_pair((*itnaam1u).second,(*itnaam2u).second));
				if(itrgzp.first==itrgzp.second){						//combinatie bg/br niet gevonden in gezinnen
					check0=true;
					//cout << "combinatie niet gevonden in gezinnen" << endl;
			//if(itrnaam1.first==itrnaam1.second || itrnaam2.first==itrnaam2.second){			//bg of br niet gevonden in naamref geb
					
					//cin.get();
					//cout << "------------------------------------------------------------------" << endl;
				}else{																																					//combinatie bg/br gevonden in gezinnen
				//cout << "bg en br gevonden in geb" << endl;
				//check0=true;
				for(itgzp=itrgzp.first;itgzp!=itrgzp.second;++itgzp){
				//cout << "vader: " << (*itgzp).second.first << " " << revnaam[(*itgzp).second.first] << endl;	
				//cout << "moeder: " << (*itgzp).second.second << " " << revnaam[(*itgzp).second.second] << endl;
				//for(itnaam1=itrnaam1.first;itnaam1!=itrnaam1.second;++itnaam1){
					//for(itnaam2=itrnaam2.first;itnaam2!=itrnaam2.second;++itnaam2){
						gezincheck++;
						hwgezin.clear();
						//itrgz=gezinnen.equal_range(make_pair((*itnaam1).second,(*itnaam2).second));
						itrgz=gezinnen.equal_range(make_pair((*itgzp).second.first,(*itgzp).second.second));
						//if(itrgz.first!=itrgz.second){				//combinatie bg en br gevonden in gezinnen
							hwgv=true;
							//cout << "combinatie gevonden in gezinnen: " << oudershw << endl;
							//cout << "gezin (bgbr):" << endl;
							for(itg2=itrgz.first;itg2!=itrgz.second;++itg2){	
								//cout << "\t" << (*itg2).second.jaar << "|" << revnaam[(*itg2).second.naam] << endl;
								tpersoon.naam=revnaam[(*itg2).second.naam];
								tpersoon.jaar=(*itg2).second.jaar;
								tpersoon.latlon=(*itg2).second.latlon;
								tpersoon.hwgb=0;
								tpersoon.gid=(*itg2).second.gid;
								hwgezin.insert(tpersoon);
							}
							//cout << "gezin (geb):" << endl;
							//for(itg2=itrgz2.first;itg2!=itrgz2.second;++itg2){
								//cout << "\t" << (*itg2).second.jaar << "|" << revnaam[(*itg2).second.naam] << endl;
							//}
						//}//else{
						//	if(check0) //cout << "combinatie niet gevonden in gezinnen: " << oudershw << endl;
						//}
						//if((!hwgezin.empty()) || check0){
							//cout << (*itnaam1).second << "|" << (*itnaam1).first << endl;
							//cout << (*itnaam2).second << "|" << (*itnaam2).first << endl;
							ovcheck=false;
							string vnaam1, fnaam1, vnaam2, fnaam2;
							if(bgpos==0){				//bg/vader gelijk
								split12(br,vnaam1,fnaam1);
								split12(moeder,vnaam2,fnaam2);
							}else{
								split12(bg,vnaam1,fnaam1);
								split12(vader,vnaam2,fnaam2);
							}
							if(vnaam1!=vnaam2 && fnaam1!=fnaam2){
								//cout << "ouders gb: " << oudersgb << endl << "ouders hw: " << oudershw << endl;
								//cout << "4 namen verschillend" << endl;
								if(levenshtein(vnaam1+"|"+fnaam1,vnaam2+"|"+fnaam2,100)>4){
									//cout << "lv>4" << endl;
									//cout << "jaar hw: " << (*itbgbr).second.jaar << endl;
									//cout << "jaar eerste gb: " << (*gbgezin.begin()).jaar << endl;
									ovcheck=true;
									if((*gbgezin.begin()).jaar>(*itbgbr).second.jaar){
										ouderov=vnaam1+"|"+fnaam1;
									}else{
										ouderov=vnaam2+"|"+fnaam2;
									}
									//cout << "ouder voor ov-check: " << ouderov << endl;
									//cin.get();
								}
							}
							if(gzmatch(oudershw,(*itbgbr).second.jaar,oudersgb,hwgezin,gbgezin,(*itbgbr).second.latlon,reflatlon,ovl,ovcheck,ouderov)){
								muit << "ouders gb: " << (*gbgezin.begin()).gid << " " << oudersgb << " (" << reflatlon[(*gbgezin.begin()).latlon] << ", " << (*gbgezin.begin()).jaar << ")" << endl;
								muit << "ouders hw: " << (*itbgbr).second.gid << " " << oudershw << " (" << reflatlon[(*itbgbr).second.latlon] << ", " << (*itbgbr).second.jaar << ")" << endl;
								//cout << "afstand (lv): " << levenshtein(oudersgb,oudershw,100) << endl;
								//cout << "afstand (km): " << dlatlon((*gbgezin.begin()).latlon,(*itbgbr).second.latlon) << endl;
								int jaren = abs((*gbgezin.begin()).jaar-(*itbgbr).second.jaar);
								muit << "jaren=" << jaren << " lv=" << lv << " km=" << dlatlon((*gbgezin.begin()).latlon,(*itbgbr).second.latlon) << endl;
								//cout << "huwelijk (" << (*itnaam1).second << "," << (*itnaam2).second << "): " << (*itbgbr).second.gid << "|" << bg << "|" << br << "|" << (*itbgbr).second.jaar << endl;
								//cout << "gezin gb: " << !hwgezin.empty() << endl;
								muit << "gezin:" << endl;
								multiset<pr3,pr3cmp> gztmp;
								pr3 pstmp;
								for(multiset<pr3,pr3cmp>::iterator itg1=hwgezin.begin();itg1!=hwgezin.end();++itg1){
									pstmp.jaar=(*itg1).jaar;
									pstmp.naam="\E[0m\E[34mHW\E[0m "+(*itg1).naam;
									pstmp.hwgb=(*itg1).hwgb;
									pstmp.gid=(*itg1).gid;
									gztmp.insert(pstmp);
								}
								for(multiset<pr3,pr3cmp>::iterator itg1=gbgezin.begin();itg1!=gbgezin.end();++itg1){
									pstmp.jaar=(*itg1).jaar;
									pstmp.naam="\E[0m\E[32mGEB\E[0m "+(*itg1).naam;
									pstmp.hwgb=(*itg1).hwgb;
									pstmp.gid=(*itg1).gid;
									gztmp.insert(pstmp);
								}
								char gzref=255;
								pair<multimap<string,pair<short int,int> >::iterator,multimap<string,pair<short int,int> >::iterator> itrov;
								multimap<string,pair<short int,int> >::iterator itov;
								//ToDo: soms onterecht een overlijden getoond
								for(multiset<pr3,pr3cmp>::iterator itg1=gztmp.begin();itg1!=gztmp.end();++itg1){
									if(ovcheck){
										if((*itg1).hwgb!=gzref){
											if(itg1!=gztmp.begin()){
												multiset<pr3,pr3cmp>::iterator itgv=itg1;
												itgv--;
												itrov=ovl.equal_range(ouderov);
												for(itov=itrov.first;itov!=itrov.second;++itov){
													if(((*itov).second.first <= (*itg1).jaar) && ((*itov).second.first >=(*itgv).jaar)){
														muit << "\E[0m\E[31mOV\E[0m " << (*itov).second.second << " " << (*itov).second.first << "|" << (*itov).first << endl;
													}
													//ToDo: toon het overlijden met de korste afstand tussen de akteplaats van itg1 of itgv
												}
											}
										}
									}
									gzref=(*itg1).hwgb;
									muit << (*itg1).jaar << "|" << (*itg1).gid << " " << (*itg1).naam << endl;
								}
								
								ngzmatch++;
								matchgezin++;
								if(ovcheck) matchov++;
								//cin.get();
								muit << endl;
								//cin.get();
								//if(oudershw=="Geert|Ziengs|Fenna|Klaasens") cin.get();
								//if(oudershw=="Albert|Jonker|Klaasje|Vestering") cin.get();
							}//else{
							//	cout << "gzmatch false, lv=" << lv << " km=" << dlatlon((*gbgezin.begin()).latlon,(*itbgbr).second.latlon) << endl;
							//}
							kandidaten++;
							//cin.get();
							//cout << "------------------------------------------------------------------" << endl;
							//check0=false;
						//}
					}
				}
			}else{
				//cout << "losse namen niet gevonden in naamref" << endl;
				check0=true;
			}
			if(check0){
				gezincheck++;
				hwgezin.clear();
				//cout << "bg of br niet gevonden in geb" << endl;
				ovcheck=false;
				ouderov="";
				if(gzmatch(oudershw,(*itbgbr).second.jaar,oudersgb,hwgezin,gbgezin,(*itbgbr).second.latlon,reflatlon,ovl,ovcheck,ouderov)){
					muit << "ouders gb: " << (*gbgezin.begin()).gid << " " << oudersgb << " (" << reflatlon[(*gbgezin.begin()).latlon] << ", " << (*gbgezin.begin()).jaar << ")" << endl;
					muit << "ouders hw: " << (*itbgbr).second.gid << " " << oudershw << " (" << reflatlon[(*itbgbr).second.latlon] << ", " << (*itbgbr).second.jaar << ")" << endl;
					//cout << "afstand (lv): " << levenshtein(oudersgb,oudershw,100) << endl;
					//cout << "afstand (km): " << dlatlon((*gbgezin.begin()).latlon,(*itbgbr).second.latlon) << endl;
					int jaren = abs((*gbgezin.begin()).jaar-(*itbgbr).second.jaar);
					muit << "jaren=" << jaren << " lv=" << lv << " km=" << dlatlon((*gbgezin.begin()).latlon,(*itbgbr).second.latlon) << endl;
					//cout << "huwelijk: " << (*itbgbr).second.gid << "|" << bg << "|" << br << "|" << (*itbgbr).second.jaar << endl;
					ngzmatch++;
					matchgeengezin++;
					//cin.get();
					muit << endl;
					//cin.get();
				}//else{
				//	cout << "gzmatch false, lv=" << lv << " km=" << dlatlon((*gbgezin.begin()).latlon,(*itbgbr).second.latlon) << endl;
				//}
				kandidaten++;
			}
			//cout << "aantal combinaties: " << gezincheck << endl;
			cbtotaal+=gezincheck;
			//cin.get();
		}
	}
}


int main(){
	multimap<string,int> naamref;
	map<int,string> revnaam;
	multimap<string,int>::iterator itnaam1,itnaam2;
	pair<multimap<string,int>::iterator,multimap<string,int>::iterator> itrnaam1,itrnaam2;
	int naamid=1;
	multimap<pair<int,int>,prec> gezinnen;
	multimap<pair<int,int>,prec>::iterator itg,itg2;
	pair<multimap<pair<int,int>,prec>::iterator,multimap<pair<int,int>,prec>::iterator> itrgz,itrgz2;
	string regel;
	vector<string> velden;
	int n=0;
	ifstream refin("/local/Links/data/dec11/naamref.txt");
	while(getline(refin,regel)){
		if(n%1000000==0) cout << n << endl;
		n++;
		split(regel,velden);
		naamid=atoi(velden[2].c_str());
		naamref.insert(make_pair(velden[0]+"|"+velden[1],naamid));
		revnaam.insert(make_pair(naamid,velden[0]+"|"+velden[1]));
	}
	refin.close();
	cout << "naamref.txt inlezen ok" << endl;
	ifstream gzin("/local/Links/data/dec11/gezinnen.txt");
	prec p1;
	int idvader=-1,idmoeder=-1;
	n=0;
	while(getline(gzin,regel)){
		if(n%1000000==0) cout << n << endl;
		n++;
		split(regel,velden);
		idvader=atoi(velden[0].c_str());
		idmoeder=atoi(velden[1].c_str());
		p1.gid=atoi(velden[2].c_str());
		p1.naam=atoi(velden[3].c_str());
		p1.jaar=atoi(velden[4].c_str());
		p1.latlon=atoi(velden[5].c_str());
		p1.match=atoi(velden[6].c_str());
		//cout << regel << endl;
		//cout << "vader: " << idvader << " moeder: " << idmoeder << " gid: " << p1.gid << " kind: " << p1.naam << " gbjaar: " << p1.jaar << " match: " << p1.match << endl;
		//cin.get();
		gezinnen.insert(make_pair(make_pair(idvader,idmoeder),p1));
	}
	gzin.close();
	cout << "aantal ouderparen: " << gezinnen.size() << endl;
	cout << "gezinnen.txt inlezen ok" << endl;
	
	ifstream ltin("/local/Links/data/dec11/ilatlon.txt");
	map<int,string> reflatlon;
	while(getline(ltin,regel)){
		split(regel,velden);
		reflatlon.insert(make_pair(atoi(velden[0].c_str()),velden[1]+"|"+velden[2]));
	}
	ltin.close();
	cout << "referentie latlon ok." << endl << "aantal plaatsen: " << reflatlon.size() << endl;
	
	string bg,br;//,vaderbg,moederbg,vaderbr,moederbr;
	//int hwnamen[] = {15,12,29,26,33,30,22,19,37,34,41,38};
	//vector<int> vhwnamen(hwnamen,hwnamen+12);
	multimap<string,pr3> bgbr;
	pr3 prbgbr;
	string lat,lon;
	
	ifstream bgbrin("/local/Links/data/dec11/bgbr.txt");
	n=0;
	string ps1;
	while(getline(bgbrin,regel)){
		if(n%500000==0) cout << n << endl;
		n++;
		split(regel,velden);
		ps1=velden[0]+"|"+velden[1];
		prbgbr.naam=velden[2]+"|"+velden[3];
		prbgbr.jaar=atoi(velden[4].c_str());
		prbgbr.latlon=atoi(velden[5].c_str());
		prbgbr.gid=atoi(velden[6].c_str());
		bgbr.insert(make_pair(ps1,prbgbr));
	}
	cout << "bgbr.txt inlezen ok" << endl;
			
	/*n=0;
	int gid;
	bool leeg;
	int hwnamen[] = {15,12,29,26};
	vector<int> vhwnamen(hwnamen,hwnamen+4);
	vector<int>::iterator it1;
	leeg=false;
	string hwdatum;
	short int hwjaar=0;
	ifstream hwin("/local/Links/data/dec11/huwelijksaktenlatlon.txt");
	while(getline(hwin,regel)){
		if(n%500000==0) cout << n << endl;
		n++;
		split(regel,velden);
		leeg=false;
		for(it1=vhwnamen.begin();it1!=vhwnamen.end();it1++){
			if(velden[(*it1)]==""){
				leeg=true;
			}
		}
		if(!leeg){
			gid=atoi(velden[0].c_str());
			bg = velden[15]+"|"+velden[12];
			//vaderbg = velden[29]+"|"+velden[26];
			//moederbg = velden[33]+"|"+velden[30];
			br = velden[22]+"|"+velden[19];
			//vaderbr = velden[37]+"|"+velden[34];
			//moederbr = velden[41]+"|"+velden[38];
			hwdatum = velden[4];
			lat = velden[8];
			lon = velden[9];
			if(hwdatum.length()==10){
				hwjaar = atoi(hwdatum.substr(6,4).c_str());
			}else{
				cout << regel << endl << hwdatum << endl;
				hwjaar=0;
			}
			prbgbr.naam=br;
			prbgbr.jaar=hwjaar;
			prbgbr.latlon=ilatlon(lat,lon);
			prbgbr.gid=atoi(velden[0].c_str());
			bgbr.insert(make_pair(bg,prbgbr));
			prbgbr.naam=bg;
			bgbr.insert(make_pair(br,prbgbr));
		}
	}
	*/
	cout << "aantal personen bgbr: " << bgbr.size() << endl;
	//cout << "bgbr inlezen ok" << endl;
	/*ofstream bgbruit("/local/Links/data/dec11/bgbr.txt");
	for(multimap<string,pr3>::iterator ituit=bgbr.begin();ituit!=bgbr.end();++ituit){
		bgbruit << (*ituit).first << "|" << (*ituit).second.naam << "|" << (*ituit).second.jaar << "|" << (*ituit).second.latlon << "|" << (*ituit).second.gid << endl;
	}
	bgbruit.close();
	cout << "bgbr wegschrijven ok" << endl;
	cin.get();
	*/
	
	map<string,int> naamrefuniek;
	multimap<pair<int,int>,pair<int,int> > gezinnen2;	
	ifstream uref("/local/Links/data/dec11/unaamref.txt");
	ifstream gz2("/local/Links/data/dec11/gezinnenu.txt");
	n=0;
	while(getline(uref,regel)){
		if(n%500000==0) cout << n << endl;
		n++;
		split(regel,velden);
		naamrefuniek.insert(make_pair(velden[0]+"|"+velden[1],atoi(velden[2].c_str())));
	}
	uref.close();
	n=0;
	while(getline(gz2,regel)){
		if(n%500000==0) cout << n << endl;
		n++;
		split(regel,velden);
		gezinnen2.insert(make_pair(make_pair(atoi(velden[0].c_str()),atoi(velden[1].c_str())),make_pair(atoi(velden[2].c_str()),atoi(velden[3].c_str()))));
	}
	gz2.close();
	cout << "unaamref en gezinnen2 ok" << endl;
	cout << "aantal namen unaamref: " << naamrefuniek.size() << endl;
	cout << "aantal gezinnen: " << gezinnen2.size() << endl;
	
	multimap<string,ov3> ovl;
	ifstream ov("/local/Links/data/dec11/overlijdensaktenlatlon.txt");
	string ovps, aktedatum, ovldatum, ovllft;
	short int ovjaar, lftov;
	int ovgid;
	int ovlatlon;
	n=0;
	while(getline(ov,regel)){
		if(n%500000==0) cout << n << endl;
		n++;
		split(regel,velden);
		ovgid=atoi(velden[0].c_str());
		ovps=velden[15]+"|"+velden[12];
		aktedatum=velden[4];
		ovldatum=velden[20];
		if(aktedatum.size()==10 || ovldatum.size()==10){
			if(ovldatum.size()==10){
				ovjaar=atoi(ovldatum.substr(6).c_str());
			}else{
				ovjaar=atoi(aktedatum.substr(6).c_str());
			}
			ovllft=velden[19];
			if(ovllft!=""){
				lftov=atoi(ovllft.c_str());
			}else{
				lftov=-1;
			}
			if(lftov==-1 || lftov>10){
				ovlatlon = ilatlon(velden[8],velden[9]);
				ovl.insert(make_pair(ovps,ov3(ovgid,ovjaar,ovlatlon)));
			}
		}
	}
	cout << "overlijdensakten ok" << endl;
	cout << "aantal personen ov: " << ovl.size() << endl;
	//ToDo: overlijdens uit ov-akten en opmerkingen huwelijk
	
	string vader,moeder,geb;
	pair<multimap<string,pr3>::iterator,multimap<string,pr3>::iterator> itrbgbr;
	multimap<string,pr3>::iterator itbgbr;
	int kandidaten=0;
	int check=0,checkf=0;
	int ngzmatch=0,ngzmatch0=0,matchdubbel=0, matchov=0;
	int matchgezin=0,matchgeengezin=0;
	itg=gezinnen.begin();
	pair<int,int> vmref = make_pair(-1,-1);
	int cbtotaal;
	ofstream muit("/local/Links/res/reconstructie/gzreconstructie.txt");
	while(itg!=gezinnen.end()){
		check++;
		if((*itg).first==vmref){
			++itg;
		}else{
			vmref=(*itg).first;
			if((*itg).second.match==0){
				cbtotaal=0;
				ngzmatch0=ngzmatch;
				if(checkf%50000==0){
					cout << checkf << " / " << check << endl;
					cout << "aantal gzmatches: " << ngzmatch << endl;
					cout << "waarvan dubbele matches: " << matchdubbel << endl << "alleen enkele: " << ngzmatch-matchdubbel << endl;
					cout << "match gezin: " << matchgezin << endl << "match geen gezin: " << matchgeengezin << endl;
					cout << "match via ov: " << matchov << endl;
				}
				checkf++;
				vader=revnaam[(*itg).first.first];
				moeder=revnaam[(*itg).first.second];
				geb=revnaam[(*itg).second.naam];
				kandidaten=0;
				//cout << "geboorteakte: " << vader << "|" << moeder << "|" << geb << endl;
				checkbgbr(vader,0,(*itg).first,bgbr,gezinnen,gezinnen2,ovl,naamref,naamrefuniek,revnaam,reflatlon,ngzmatch,ngzmatch0,cbtotaal,kandidaten,matchgezin,matchgeengezin,matchov,muit);
				checkbgbr(moeder,1,(*itg).first,bgbr,gezinnen,gezinnen2,ovl,naamref,naamrefuniek,revnaam,reflatlon,ngzmatch,ngzmatch0,cbtotaal,kandidaten,matchgezin,matchgeengezin,matchov,muit);
				
				//cout << check << " # " << kandidaten << " kandidaten " << (*itg).first.first << "|" << vader << " " << (*itg).first.second << "|" << moeder << endl;
				//cout << "------------------------------------------------------------------" << endl;
				//cout << "kandidaten/totaal aantal combinaties gecheckt: " << kandidaten << "/" << cbtotaal << " = " << (float)kandidaten/cbtotaal << endl;
				//cin.get();
				if(ngzmatch-ngzmatch0>1) matchdubbel+=(ngzmatch-ngzmatch0-1);			//cin.get();
			}
			++itg;
		}
	}
	muit.close();
	cout << checkf << " / " << check << endl;
	cout << "aantal gzmatches: " << ngzmatch << endl;
	cout << "waarvan dubbele matches: " << matchdubbel << endl << "alleen enkele: " << ngzmatch-matchdubbel << endl;
	cout << "match gezin: " << matchgezin << endl << "match geen gezin: " << matchgeengezin << endl;
	cout << "match via ov: " << matchov << endl;
	return(0);
}
