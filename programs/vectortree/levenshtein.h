#include <vector>
#include <map>
using namespace std;

typedef vector< vector<int> > Tmatrix; 

int levenshtein(const string& source, const string& target, const int th);
float lv_ordered(const string& source, const string& target, map<string,float>& orderedmap);