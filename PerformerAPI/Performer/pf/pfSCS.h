#ifndef d65219bb_549f_4a07_9ee2_07ea8fdd56b7
#define d65219bb_549f_4a07_9ee2_07ea8fdd56b7

#include "Performer/pf/pfGroup.h"

class pfSCS : public pfGroup
{
public:
  pfSCS() : pfGroup(){}
protected:
  virtual ~pfSCS(){}
  virtual pfNode *_clone() { return new pfSCS; }
};

#endif
