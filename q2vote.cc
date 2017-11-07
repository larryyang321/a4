#include "q2.h"
#include <iostream>
using namespace std;

TallyVotes::TallyVotes(unsigned int group, Printer & printer):group(group),printer(printer){}
void TallyVotes::flush(){
	number_so_far =0;
	pic =0;
	sta =0;
	gift =0;
	}

Tour TallyVotes::vote(unsigned int id, Ballot ballot){
	#if deined (IMPLTYPE_MC)
	muxt.acquire();
	if(flag){
		barge.wait(mutx);
		}
	number_so_far ++;
	pic += ballot.picture;
	sta += ballot.statue;
	gift += ballot.giftshop;

	if(number_so_far <group){
		if(!barge.empty()){
			flag = true;
			barge.signal();
		}else{
			flag = false;	
		}
		blocked.wait(mutx);
		//print u
	}else{
		//print c
		flag = true;
		//set result
		cout<<pic<<" "<<sta<< " "<<gift<<endl;
		flush();
		blocked.signal();
			
	}
	if(!blocked.empty()){
		flag = true;
		blocked.signal();
    }else{
		if(!barge.empty()){
			flag = true;
			barge.signal();
		}else{
			flag = false;
		}
	}
	mutx.release();


	#endif
	
	
	
	}



//extern MPRNG mprng;
Voter::Voter( unsigned int id, TallyVotes & voteTallier, Printer & printer )
:id(id),voteTallier(voteTallier),printer(printer){}

void Voter::main(){
/*
	• yield a random number of times, between 0 and 19 inclusive, so all tasks do not start simultaneously
• print start message
• yield once
• vote
• yield once
• print finish message
*/
yield(mprng(19));

//print start


//vote

voteTallier::vote(id, cast());

//finish



}

Printer::Printer(unsigned int voters):voters(voters){}

void Printer::print(unsigned int id, Voter::States state ){}
void Printer::print( unsigned int id, Voter::States state, TallyVotes::Tour tour ){}
    void Printer::print( unsigned int id, Voter::States state, TallyVotes::Ballot ballot ){}
    void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked ){}
