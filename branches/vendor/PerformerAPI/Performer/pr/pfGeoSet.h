#ifndef fg43bba1c_7c8c_440f_93de_15c566f7ac49
#define fg43bba1c_7c8c_440f_93de_15c566f7ac49

#include "Performer/pr/pfObject.h"

#define PFGS_LINESTRIPS 1
#define PFGS_POINTS     2
#define PFGS_POLYS      3
#define PFGS_TRIFANS    4
#define PFGS_TRISTRIPS  5

#define PFGS_COLOR4         0
#define PFGS_NORMAL3        1
#define PFGS_TEXCOORD2      2
#define PFGS_COORD3         3

#define PFGS_OFF            0
#define PFGS_OVERALL        1
#define PFGS_PER_PRIM       2
#define PFGS_PER_VERTEX     3

class pfGeoState;


class pfGeoSet : public pfObject
{
public:
  pfGeoSet() : pfObject(){}
  void setNumPrims ( int numPrims ){}
  void setPrimType ( int type ){}
  void setPrimLengths ( int *lengths ){}
  void setAttr ( int attribute, int binding, void *attributeList, unsigned short * indexList ){}
  void setGState ( pfGeoState *state ){}
protected:
  virtual ~pfGeoSet(){}
};

#endif
