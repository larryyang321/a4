#include "q2.h"
//#include "q2printer.h"
#include <iostream>
using namespace std;
#if defined (IMPLTYPE_MC)
TallyVotes::~TallyVotes(){}
TallyVotes::TallyVotes(unsigned int group, Printer & printer):group(group),printer(printer){}
void TallyVotes::flush(){
	number_so_far =0;
	pic =0;
	sta =0;
	gift =0;
	}

TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot){
//	#if defined (IMPLTYPE_MC)
	mutx.acquire();
//	printer.print(id,Voter::States::Start);
	if(flag){
		printer.print(id,Voter::States::Barging);
		barge.wait(mutx);
		}
	number_so_far ++;
	pic += ballot.picture;
	sta += ballot.statue;
	gift += ballot.giftshop;
	printer.print(id,Voter::States::Vote,ballot);

	if(number_so_far <group){
		if(!barge.empty()){
			flag = true;
			barge.signal();
		}else{
			flag = false;	
		}
		blocked_num++;
		printer.print(id,Voter::States::Block,blocked_num);
		blocked.wait(mutx);
		blocked_num--;
		printer.print(id,Voter::States::Unblock,blocked_num);
		//print u
	}else{
		//print c
		printer.print(id,Voter::States::Complete);
		flag = true;
		//set result
//		cout<<pic<<" "<<sta<< " "<<gift<<endl;
//		flush();
		//blocked.signal();
			
	}
	Tour result;
	if((gift>=pic)&&(gift>=sta)){
		result = TallyVotes::Tour::GiftShop;
	}else if((pic>=gift)&&(pic>=sta)){
		result = TallyVotes::Tour::Picture;
	}else{
		result = TallyVotes::Tour::Statue;
	}
	
	if(!blocked.empty()){
		flag = true;
		blocked.signal();
   	 }else{
		flush();
		if(!barge.empty()){
			flag = true;
			barge.signal();
		}else{
			flag = false;
		}
	}

	printer.print(id,Voter::States::Finished,result);
	mutx.release();
//	#elif defined(IMPLTYPE_BAR)

//	#endif
	
	return result;
	
	}


#elif defined (IMPLTYPE_BAR)
TallyVotes::~TallyVotes(){}
TallyVotes::TallyVotes(unsigned int group, Printer & printer):uBarrier(group),group(group),printer(printer){}
void TallyVotes::flush(){
        number_so_far =0;
        pic =0;
        sta =0;
        gift =0;
        }
TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot){
//	printer.print(id,Voter::States::Start);
	number_so_far ++;
        pic += ballot.picture;
        sta += ballot.statue;
        gift += ballot.giftshop;
        printer.print(id,Voter::States::Vote,ballot);
	 if(number_so_far <group){
		blocked_num++;
                printer.print(id,Voter::States::Block,blocked_num);
                uBarrier::block();
                blocked_num--;
                printer.print(id,Voter::States::Unblock,blocked_num);


	}else{

		printer.print(id,Voter::States::Complete);
		uBarrier::block();
	}
	Tour result;
        if((gift>=pic)&&(gift>=sta)){
                result = TallyVotes::Tour::GiftShop;
        }else if((pic>=gift)&&(pic>=sta)){
                result = TallyVotes::Tour::Picture;
        }else{
                result = TallyVotes::Tour::Statue;
        }
	if(blocked_num == 0)flush();
	 printer.print(id,Voter::States::Finished,result);
	return result;



}
#elif IMPLTYPE_SEM
TallyVotes::TallyVotes(unsigned int group, Printer & printer):group(group),printer(printer){
	mutx = new uSemaphore(1);
	barge = new uSemaphore(0);
	flag = false;
	barge_num = 0;
	groups = new uSemaphore*[group];
	int i =0;
	while(i < (int)group){
		groups[i] = new uSemaphore(0);
		i++; 

	}


}
TallyVotes::~TallyVotes(){
	delete mutx;
	int i =0;
	while(i < (int)group){
		delete groups[i];
		i++;
	}
	i = 0;
	delete barge;
	delete[] groups;

}
void TallyVotes::flush(){
	
		
        number_so_far =0;
        pic =0;
        sta =0;
        gift =0;
        }

TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot){
	mutx->P();
	if(flag){
		printer.print(id,Voter::States::Barging);
		barge_num ++;
		barge->P(*mutx);
		mutx->P();
		barge_num--;
	}
	number_so_far ++;
        pic += ballot.picture;
        sta += ballot.statue;
        gift += ballot.giftshop;
        printer.print(id,Voter::States::Vote,ballot);
         if(number_so_far <group){
		if(barge_num!=0){
                        flag = true;
                        barge->V();
                }else{
                        flag = false;
                }
		blocked_num++;
                printer.print(id,Voter::States::Block,blocked_num);
//		cout<<barge_num<<endl;		
                groups[blocked_num-1]->P(*mutx);
		mutx->P();
                blocked_num--;
                printer.print(id,Voter::States::Unblock,blocked_num);


	}	else{

		printer.print(id,Voter::States::Complete);
                



	}
	Tour result;
        if((gift>=pic)&&(gift>=sta)){
                result = TallyVotes::Tour::GiftShop;
        }else if((pic>=gift)&&(pic>=sta)){
                result = TallyVotes::Tour::Picture;
        }else{
                result = TallyVotes::Tour::Statue;
        }
	if(blocked_num == 0){
		flush();
		if(barge_num!=0){
                        flag = true;
			
                        barge->V();
                }else{
                        flag = false;
                }
	}else{
		flag = true;

		groups[blocked_num-1]->V();
	}

	printer.print(id,Voter::States::Finished,result);
	mutx->V();
	return result;

}


#endif

//extern MPRNG .iprng;
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
printer.print(id,Voter::States::Start);

//vote

voteTallier.vote(id, cast());

//finish
//printer.print(id,Voter::States::Start);


}

Printer::Printer( unsigned int voters ):voters(voters){
	states = new Voter::States[voters];
	save = new bool[voters];
	nb = new int[voters];
	vo = new TallyVotes::Ballot [voters];
	re = new TallyVotes::Tour [voters];
	int i =0;
	while(i<(int)voters){
		save[i] =false;
		cout<<"V"<<(i+1)<<'\t';
		i++;

	}
	cout<<endl;
	i = 0;
	while(i <(int)voters){

		cout << "*******"<<'\t';
		i++;
	}
	cout<<endl;

}
	void Printer::printinfo(){
		int i= 0;
                        while(i <(int) voters){
                                if(save[i]){
					cout<<(char)states[i];
					if((char)states[i] == 'V'){
						cout<<" "<<vo[i].picture<<","<<vo[i].statue<< ","<<vo[i].giftshop<<'\t';

					}else if((char)states[i] == 'B'){
						cout<<" "<<nb[i]<<'\t';


					}else if((char)states[i]=='U'){
						cout<<" " <<nb[i]<<'\t';

					}else if((char)states[i] =='F'){
						cout<<" "<<(char)re[i]<<'\t';

					}else{
                                        	cout<<'\t';
                                        }
					save[i] =false;


                                }
                                else{

                                        cout<<'\t';
                                }
                                i++;


                        }
                        cout<<endl;

	}
    void Printer::print( unsigned int id, Voter::States state ){
//		cout<<id<<endl;
		if(save[id-1]){
			printinfo();
			


		}
			states[id-1] = state;
			save[id-1] = true;
		
	}
    void Printer::print( unsigned int id, Voter::States state, TallyVotes::Tour tour ){

		if(save[id-1]){
			printinfo();
		}
		states[id-1] = state;
		save[id-1] = true;
		re[id-1] = tour;

	}
    void Printer::print( unsigned int id, Voter::States state, TallyVotes::Ballot ballot ){
		if(save[id-1]){
                        printinfo();
                }
                states[id-1] = state;
                save[id-1] = true;
                vo[id-1] = ballot;
	}
	Printer::~Printer(){
		printinfo();
		cout<<"*****************"<<endl;
		cout<<"All tours started"<<endl;
		delete[] states;
		delete[] save;
		delete[] nb;
		delete[] re;
		delete[] vo;
	}
    void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked ){
		if(save[id-1]){
                        printinfo();
                }
                states[id-1] = state;
                save[id-1] = true;
		nb[id-1] = numBlocked;

	}
//Printer::Printer(unsigned int voters):voters(voters){}
/*
void Printer::print(unsigned int id, Voter::States state ){}
void Printer::print( unsigned int id, Voter::States state, TallyVotes::Tour tour ){}
    void Printer::print( unsigned int id, Voter::States state, TallyVotes::Ballot ballot ){}
    void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked ){}*/
