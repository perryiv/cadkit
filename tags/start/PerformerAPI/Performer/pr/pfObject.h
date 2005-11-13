#ifndef e43bba1c_7c8c_440f_93de_15c566f7ac49
#define e43bba1c_7c8c_440f_93de_15c566f7ac49

#include "Performer/pr/pfMemory.h"

class pfObject : public pfMemory
{
public:
  pfObject() : pfMemory(){}
protected:
  virtual ~pfObject(){}
};

#endif
