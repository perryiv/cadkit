#ifndef e65219bb_549f_4a07_9ee2_07ea8fdd56b7
#define e65219bb_549f_4a07_9ee2_07ea8fdd56b7

#include "Performer/pf/pfSCS.h"

class pfMatrix;


class pfDCS : public pfSCS
{
public:
  pfDCS() : pfSCS(){}
  void setMat ( const pfMatrix &matrix ){}
protected:
  virtual ~pfDCS(){}
};

#endif
