#ifndef a90095413_3198_42c8_b6cf_6d914fc7a59d
#define a90095413_3198_42c8_b6cf_6d914fc7a59d

#include "Performer/pf/pfGroup.h"
#include "Performer/pr/pfLinMath.h"

class pfLOD : public pfGroup
{
public:
  pfLOD() : pfGroup(){}
  void setCenter ( pfVec3 &center ){}
  void setRange ( int index, float range ){}
protected:
  virtual ~pfLOD(){}
  virtual pfNode *_clone() { return new pfLOD; }
};

#endif
