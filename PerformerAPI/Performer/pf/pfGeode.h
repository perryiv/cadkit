#ifndef a1ce9aff2_aa84_4cad_b216_189200cb8a16567
#define a1ce9aff2_aa84_4cad_b216_189200cb8a16567

#include "Performer/pf/pfNode.h"

class pfGeoSet;


class pfGeode : public pfNode
{
public:
  pfGeode() : pfNode(){}
  void addGSet ( pfGeoSet *gset ){}
  unsigned int getNumGSets() const { return 1; }
protected:
  virtual ~pfGeode(){}
  virtual pfNode *_clone() { return new pfGeode; }
};

#endif
