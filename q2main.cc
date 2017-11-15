#include <iostream>
#include "q2.h"
//#include "q2printer.h"
MPRNG mprng;
using namespace std;

/*Printer::Printer(unsigned int voters):voters(voters){}

void Printer::print(unsigned int id, Voter::States state ){}
void Printer::print( unsigned int id, Voter::States state, TallyVotes::Tour tour ){}
    void Printer::print( unsigned int id, Voter::States state, TallyVotes::Ballot ballot ){}
    void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked ){}

*/
void usage(){

	cout<<"Usage: ./voteBAR.txt  Voters (> 0 & V mod G = 0, default 6)  Group (> 0, default 3)  Seed (> 0)"<<endl;
	exit(-1);
}
void uMain::main(){
	int V = 6;
	int G = 3;
	int seed = 0;
	seed = getpid();
	switch(argc){
		case 4:
			try{

				seed = stoi(argv[3]);
			}catch(invalid_argument& e){
				usage();
			}
			if(seed <=0)usage();
		case 3:		
			try{

                                G = stoi(argv[2]);
                        }catch(invalid_argument& e){
                                usage();
                        }
			if(G<= 0)usage();
		case 2:
			try{

                               V = stoi(argv[1]);
                        }catch(invalid_argument& e){
                                usage();
                        }
			if(V<=0) usage();
		default:
			break;
	}
	if((V%G) != 0)usage();
	
	mprng.set_seed(seed);
	Printer p(V);
	TallyVotes t(G, p);
	Voter *v[V];
	int i =0;
	while(i < V){
		v[i] = new Voter((i+1),t,p);
		i++;

	} 
	i = 0;
	while(i < V){
		delete v[i];
		i++;

	}



}
