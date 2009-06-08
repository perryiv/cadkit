#ifndef _PF_GEO_MATH_H_
#define _PF_GEO_MATH_H_

#include "Performer/pr/pfLinMath.h"


class pfSphere
{
public:

  pfVec3 center;
  float radius;

  pfSphere() : center ( 0.0f, 0.0f, 0.0f ), radius ( 1.0f ){}
};


#endif
