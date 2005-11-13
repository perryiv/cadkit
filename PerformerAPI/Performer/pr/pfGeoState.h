#ifndef ddsgfg43bba1c_7c8c_440f_93de_15c566f7ac49
#define dghdfg43bba1c_7c8c_440f_93de_15c566f7ac49

#include "Performer/pr/pfObject.h"


#define PFSTATE_TRANSPARENCY    1
#define PFSTATE_ENLIGHTING      2
#define PFSTATE_FRONTMTL        3
#define PFSTATE_BACKMTL         4

#define PF_ON    true
#define PF_OFF   false
#define PFTR_ON  true
#define PFTR_OFF false


class pfGeoState : public pfObject
{
public:
  pfGeoState() : pfObject(){}
  void setAttr ( unsigned int attribute, void *value ){}
  void setMode ( unsigned int mode, bool value ){}
protected:
  virtual ~pfGeoState(){}
};

#endif
