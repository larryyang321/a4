#ifndef P_H
#define P_H
#include "q2.h"

_Task Voter;
_Monitor  Printer {        // chose one of the two kinds of type constructor
        unsigned int voters;

  public:
    Printer( unsigned int voters );
    void print( unsigned int id, Voter::States state );
    void print( unsigned int id, Voter::States state, TallyVotes::Tour tour );
    void print( unsigned int id, Voter::States state, TallyVotes::Ballot ballot );
    void print( unsigned int id, Voter::States state, unsigned int numBlocked );
};
#endif
