#ifndef fg43bba1c_7c8c_440f_93de_15c566f7ac49ght
#define fg43bba1c_7c8c_440f_93de_15c566f7ac49ght

#include "Performer/pr/pfObject.h"

#define PFMTL_AMBIENT   1
#define PFMTL_DIFFUSE   2
#define PFMTL_EMISSION  3
#define PFMTL_SPECULAR  4


class pfMaterial : public pfObject
{
public:
  pfMaterial() : pfObject(){}
  void setColor ( int color, float r, float g, float b ){}
  void setShininess ( float shininess ){}
protected:
  virtual ~pfMaterial(){}
};

#endif
