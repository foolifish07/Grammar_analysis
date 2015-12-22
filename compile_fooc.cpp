#include <iostream>
#include <string>
#include <string.h>
#include <cstdio>
#include <map>
#include <bitset>
#include <vector>
#include <set>
#include <stack>

#define LL long long 
using namespace std;
const int maxn = 105;

map<string,int> keyword , id;
vector<string> constv , limiter , op;

bool isAlpha(char c){
	if(c >='A' && c<='Z') return true;
	if(c >='a' && c<='z') return true;
	return false;
}
bool isNumber(char c){
	return '0'<=c && c<='9' ;
}
bool inString(char c){
	if ( isNumber(c)||isAlpha(c) ) 
		return true;
	else if ( c=='_' || c=='.' )
		return true;
	else return false;
}


struct LL1{

	set<string> keyword;
	map<string,int> vt, vn;
	struct NFA{
		string name;
		vector< pair<int,int> > edge;
		bitset<maxn> first , follow;
		map<int,int> to;

		vector< vector<string> >production;
		vector< bitset<maxn> > select;
	};
	NFA nfa[maxn]; int start;

	bool read_grammar(string filename){
		FILE *fin = fopen( filename.c_str() , "r");
		char s1[maxn] , s2[maxn];
		keyword.clear();
		vector<string> tmp;  
		vn.clear(); int now = 0;
		while ( fscanf(fin,"%s%s" , s1,s2)!=EOF ){
			if ( strcmp(s1,"##")==0 ){
				while ( 1 ){
					if ( strcmp(s2 , "##")==0 ) break;
					if ( fscanf(fin,"%s" ,s2)==EOF ) break;
				}
				continue;
			}
			if ( s2[0]==':' ){
				if ( vn.count(s1)>0 ){
					now = vn[s1];
				}
				else {
					vn[s1] = ++now; 
					nfa[now].name = s1; 
					nfa[now].production.clear();
				}
			}
			else if ( nfa[now].name=="keyword" ){
				keyword.insert(s1);
				while ( 1 ){
					if ( s2[0]=='|'||s2[0]=='\\' ) break;
					keyword.insert(s2);
					fscanf(fin,"%s" , s2);
				}
			}else {
				tmp.clear();
				tmp.push_back(s1);
				while ( 1 ){
					if ( s2[0]=='|'||s2[0]=='\\' ) break;
					tmp.push_back(s2);
					fscanf(fin,"%s" , s2);
				}
				nfa[now].production.push_back(tmp);
			}
		}
		fclose( fin );
		get_terminal();
		return true;
	}

	void get_terminal(){
		vt.clear(); string s;
		for(int i=1;i<=vn.size();i++){
			for(int j=0;j<nfa[i].production.size();j++)
				for(int k=0;k<nfa[i].production[j].size();k++){
					s = nfa[i].production[j][k];
					if ( isTerminal(s) ){
						if ( vt.count(s)==0 ){
							int tmp = vt.size();
							vt[s] = tmp+1;
						}
					}
				}
		}
	}
	bool isTerminal(string s){
		if ( vn.count(s)==0 ) return true;
		return false;
	}

	void get_firstset(){
		for(int i=1;i<=vn.size();i++){
			nfa[i].first.reset();
		}
		string s; bitset<maxn> tmp;
		while (1){
			bool flag = false;
			for(int i=1;i<=vn.size();i++){ 
				tmp = nfa[i].first;

				for(int j=0;j<nfa[i].production.size();j++){
					int k = 0; 
					while( k<nfa[i].production[j].size() ){
						s = nfa[i].production[j][k];
						if ( isTerminal(s) ){
							nfa[i].first.set(vt[s]);
							break;
						}
						else {
							int id = vn[s];
							nfa[i].first |= nfa[id].first;
							if ( nfa[id].first.test( vt["$"] ) ){
								nfa[i].first.reset( vt["$"] );
							}
							else break;
						}
						k++;
					}
					if ( k==nfa[i].production[j].size() ) 
						nfa[i].first.set( vt["$"] );
				}

				if ( !flag && tmp!=nfa[i].first )
					flag = true;
			}
			if ( !flag ) break;
		}
	}
	bool get_followset(){
		get_firstset();

		start = -1;
		for(int i=1;i<=vn.size();i++)
			if ( nfa[i].name=="start" ){
				start = i; break;
			}
		if ( start==-1) {
			printf("Your grammer should have a non-ternimal 'start' as a start.");
			return false;
		}
		// add "@" as end of the context
		for(int i=1;i<=vn.size();i++)
			nfa[i].follow.reset();
		int sz = vt.size();
		vt[ "@" ] = sz+1;
		nfa[start].follow.set( vt[ "@" ] );

		string s; bitset<maxn> tmp;
		while (1){
			bool flag = false;
			for(int i=1;i<=vn.size();i++)
				for(int j=0;j<nfa[i].production.size();j++)
					for(int k=0;k<nfa[i].production[j].size();k++){
						s = nfa[i].production[j][k];
						if ( !isTerminal(s) ){
							int id = vn[s]; tmp = nfa[id].follow;

							int h = k+1;
							while ( h<nfa[i].production[j].size() ){
								s = nfa[i].production[j][h];
								if ( isTerminal(s) ){
									nfa[id].follow.set(vt[s]);
									break;
								}
								else {
									int kk = vn[s];
									nfa[id].follow |= nfa[ kk ].first;
									if ( nfa[kk].first.test( vt["$"] ) ){
										nfa[id].follow.reset( vt["$"] );
									}
									else break;
								}
								h++;
							}
							if ( h==nfa[i].production[j].size() )
								nfa[id].follow |= nfa[i].follow;
		
							if ( !flag && nfa[id].follow!=tmp )
								flag = true;
						}
					}
			
			if ( !flag ) break;
		}	
		return true;
	}
	bool get_selectset(){
		if ( !get_followset() ) return false;
		string s; bitset<maxn> tmp; 
		for(int i=1;i<=vn.size();i++){
			nfa[i].select.clear();

			for(int j=0;j<nfa[i].production.size();j++){
				int k = 0; tmp.reset();
				while ( k<nfa[i].production[j].size() ){
					s = nfa[i].production[j][k];
					if ( s=="$" ) {}
					else if ( isTerminal(s) ){
						tmp.set( vt[s] );
						nfa[i].select.push_back( tmp );
						break;
					}
					else {
						int id = vn[s];
						tmp |= nfa[ id ].first;

						if ( nfa[id].first.test( vt["$"] ) )
							tmp.reset( vt["$"] );
						else {
							nfa[i].select.push_back(tmp); 
							break;
						}
					}
					k++;
				}
				if ( k==nfa[i].production[j].size() ){
					tmp |= nfa[i].follow;
					nfa[i].select.push_back(tmp);
				}
			} 
		}

		return true;
	}

	bool build_edge(){
		if ( !get_selectset() ) return false;
		
		bitset<maxn> tmp;
		for(int i=1;i<=vn.size();i++){
			nfa[i].to.clear();
			for(int j=0;j<nfa[i].production.size();j++){
				tmp = nfa[i].select[j];
				for(int k=1;k<=vt.size();k++)
					if ( tmp.test(k) ){
						if ( nfa[i].to.count(k)>0 ) {
							printf("It is not a LL(1) grammer.\n");
							printf("Error in %s\n" , nfa[i].name.c_str() );

							return false;
						}
						nfa[i].to[k] = j;
					}
			}
		}

		return true;
	}
	bool init(string s){ 
		if ( !read_grammar(s) ) return false;
		if ( !build_edge() ) return false;
		return true;
	}

	// lex analysis
	vector<string> st;
	bool check_a_string(string s , FILE *fout , int openlog){
		if ( openlog ) printf("Compiling string : %s\n" ,s.c_str() );
		
		s += '@'; st.clear(); 
		st.push_back("@"); st.push_back("start");

		string x , lextype; char tmp[5]; tmp[1] = 0;
		for(int i=0;i<s.size();){
			if ( openlog ){
				cout<< st.size()<<" : ";
				for(int j=0;j<st.size();j++)
					cout<< st[j]<<' '; cout<<endl;
			}

			x = st.back();
			if ( isTerminal(x) ){
				if ( x[0]!=s[i] ){
					printf("Lex Error with %s\n" ,s.c_str() );
					return false;
				}
				else {
					st.pop_back(); i++;
				}
			}
			else {
				int id = vn[x]; 
				tmp[0] = s[i]; int sid = vt[tmp];
				if ( nfa[id].to.count( sid )==0 ){
					printf("Lex Error with %s\n" ,s.c_str() );
					return false;
				}  
				else {
					int k = nfa[id].to[ sid ]; 
					st.pop_back();
					for(int j=nfa[id].production[k].size()-1;j>=0;j--)
						if ( nfa[id].production[k][j]!="$" )
							st.push_back( nfa[id].production[k][j] );
					// output the tokens 
					if ( x=="start" ) 
						lextype = st[1];
				}
			}
		}
		s.erase(s.end()-1);
		if ( keyword.count(s)>0 ) lextype = "keyword";;
		fprintf(fout , "%s %s\n" ,lextype.c_str() , s.c_str() );
		
		if ( openlog ) cout<<s <<" successful! " <<endl<<endl;
		
		return true;
	}
	bool compile_source_code(string inputfile , string outfile ,int openlog){
		FILE *fin = fopen(inputfile.c_str() , "r");
		FILE *fout = fopen(outfile.c_str() , "w");

		char s[maxn*10]; int len , nowstart;
		string x;
		while ( fscanf(fin,"%s", s)!=EOF ){
			len = strlen(s) , nowstart = 0; 
			for(int h=0;h<len;h++)
				if ( !inString( s[h] ) ){
					x.clear();
					for(int i=nowstart;i<h;i++) x += s[i];
					if ( x.size()>0 && !check_a_string(x ,fout , openlog ) ){
						fclose(fin); fclose(fout); return false;
					}
					x.clear(); x += s[h];
					if ( !check_a_string(x, fout , openlog) ){
						fclose(fin); fclose(fout); return false;
					}
					nowstart = h+1;
				}
			x.clear();
			for(int i=nowstart;i<len;i++)  x += s[i];

			if ( x.size()>0 && !check_a_string(x , fout , openlog) ){
				fclose(fin); fclose(fout); return false;
			}
		}
		printf("Lex Analysis success.\n");
		printf("You can have a look in %s  \n" , outfile.c_str() );
		fclose( fin ); fclose(fout);
		return true;
	}
	bool lex_analysis(string lex_grammar, string lex_table,string source_code, int openlog){
		if ( !init(lex_grammar) ) return false;
		if ( !compile_source_code(source_code,lex_table,openlog) ) return false;
		return true;
	}

	// syntax analysis
	bool compile_lex_table(string inputfile , int openlog){
		FILE *fin = fopen(inputfile.c_str() , "r");

		char s1[maxn*10] ,  s2[maxn*10]; int cnt = 0;
		string x , y;	
		st.clear(); st.push_back("@"); st.push_back("start");
		while (1){
			if ( fscanf(fin,"%s%s" ,s1,s2)==EOF ) y = "@";
			else { 
				y = s1;
				if ( y=="keyword"||y=="operator"||y=="limiter" )
					y = s2;
			}
			x = st.back(); cnt++;

			while (1){
				if ( openlog ){
					cout<< y <<"\t: ";
					for(int j=0;j<st.size();j++) cout<< st[j]<<' '; cout<<endl;
				}

				x = st.back();
				if ( isTerminal(x) ){
					if ( x==y){
						st.pop_back(); break;
					}
					else{
						printf("Syntax Error with in %d line.\n", cnt );
						fclose( fin );
						return false;
					}
				}
				else {
					int id = vn[x];
					if ( nfa[id].to.count( vt[y] )>0 ){
						int k = nfa[id].to[ vt[y] ]; st.pop_back();
						for(int j=nfa[id].production[k].size()-1;j>=0;j--)
							if ( nfa[id].production[k][j]!="$" )
								st.push_back( nfa[id].production[k][j] );
					}
					else {
						printf("Syntax Error with in %d line.\n", cnt );
						fclose( fin );
						return false;	
					}
				}
			}
			if ( y=="@" ) break;
		}

		printf("Compiling all string successfully.\n");
		fclose( fin );
		return true;
	}
	bool syntax_analysis(string syntax_grammar, string lex_table, int openlog){
		if ( !init(syntax_grammar) ) return false;
		if ( !compile_lex_table(lex_table,openlog) ) return false;
		return true;
	}
	void test(){
		cout<< vn.size() <<' '<<vt.size()<<endl;
		for(int i=1;i<=vn.size();i++){
			cout<< nfa[i].name << " : "<< nfa[i].production.size()<<endl;
			cout<< nfa[i].first.count()<<' '<<nfa[i].follow.count()<<
				' '<<nfa[i].to.size()<< endl;
			/*
			cout<<" first : ";
			for(map<string,int>::iterator it=vt.begin();it!=vt.end();it++){
				string s1 = it->first; int s2 = it->second;
				if ( nfa[i].first.test(s2) ) cout<< s1<<' ';
			}cout<<endl;
			cout<<" follow : ";
			for(map<string,int>::iterator it=vt.begin();it!=vt.end();it++){
				string s1 = it->first; int s2 = it->second;
				if ( nfa[i].follow.test(s2) ) cout<< s1<<' ';
			}cout<<endl;
			*/
			for(int j=0;j<nfa[i].production.size();j++){
				cout<< nfa[i].select[j].count()<<" : ";
				for(map<string,int>::iterator it=vt.begin();it!=vt.end();it++){
					string s1 = it->first; int s2 = it->second;
					if ( nfa[i].select[j].test(s2) ) cout<< s1<<' ';
				}cout<<endl;
			}cout<<endl;
		}
		int id = vn[ "start" ]; cout << id<<" : "<<endl;
		cout<< nfa[id].to.count( vt[";"] );
	}
	
}compile;

int main(int argc , char* argv[]){

	char lex_grammar[maxn] = "lex_grammar.txt";
	char lex_table[maxn] = "lex_table.data";
	char syntax_grammar[maxn] = "syntax_grammar.txt";
	char source_code[maxn] = "source_code.fooc";
	int openlog = 1;

	if ( compile.lex_analysis(lex_grammar , lex_table ,source_code,openlog )
		 && compile.syntax_analysis( syntax_grammar,lex_table,openlog ) )
		cout<< "YES" << endl;
	else 
		cout<< "NO" <<endl;

	return 0;
}