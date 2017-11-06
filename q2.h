#include "MPRNG.h"
extern MPRNG mprng;
#if defined( IMPLTYPE_MC )              // mutex/condition solution
// includes for this kind of vote-tallier
class TallyVotes {

	uOwnerLock mutx;
	uCondLock barge;
	uCondLock blocked;
	bool flag=false;；
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_BAR )           // barrier solution
// includes for this kind of vote-tallier
_Cormonitor TallyVotes : public uBarrier {
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_SEM )           // semaphore solution
// includes for this kind of vote-tallier
class TallyVotes {
    // private declarations for this kind of vote-tallier
#else
    #error unsupported voter type
#endif
  
  unsigned int group;
  Printer printer;
  unsigned int number_so_fa=0;
  unsigned int pic=0;
  unsigned int sta=0;
  unsigned int gift =0;

  void flush();
  // common declarations
  public:                               // common interface
    TallyVotes( unsigned int group, Printer & printer );
    struct Ballot { unsigned int picture, statue, giftshop; };
    enum Tour { Picture = 'p', Statue = 's', GiftShop = 'g' };
    Tour vote( unsigned int id, Ballot ballot );
	
};

_Task Voter {

	unsigned int id;
	TallyVoters voteTallier;
	Printer printer;
    //TallyVotes::Ballot *result=null;
	// Choose ranking of picture tour, then relationship of statue to gift shop.
    TallyVotes::Ballot cast() {         // cast 3-way vote
        static unsigned int voting[3][2][2] = { { {2,1}, {1,2} }, { {0,2}, {2,0} }, { {0,1}, {1,0} } };
        unsigned int picture = mprng( 2 ), statue = mprng( 1 );
        return (TallyVotes::Ballot){ picture, voting[picture][statue][0], voting[picture][statue][1] };
    }
  public:
    enum States { Start = 'S', Vote = 'V', Block = 'B', Unblock = 'U', Barging = 'b',
                   Complete = 'C', Finished = 'F' };
    Voter( unsigned int id, TallyVotes & voteTallier, Printer & printer );
	void main();
};

_Monitor / _Cormonitor Printer {        // chose one of the two kinds of type constructor
  public:
    Printer( unsigned int voters );
    void print( unsigned int id, Voter::States state );
    void print( unsigned int id, Voter::States state, TallyVotes::Tour tour );
    void print( unsigned int id, Voter::States state, TallyVotes::Ballot ballot );
    void print( unsigned int id, Voter::States state, unsigned int numBlocked );
};
