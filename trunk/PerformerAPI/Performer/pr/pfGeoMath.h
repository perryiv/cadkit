#ifndef a8355edca_d102_4e09_b32c_db0c98ec8da3
#define a8355edca_d102_4e09_b32c_db0c98ec8da3

#include "Performer/pr/pfLinMath.h"

class pfSphere
{
public:
  pfVec3 center;
  float radius;
  pfSphere() : center ( 0.0f, 0.0f, 0.0f ), radius ( -1.0f ){}
};

#endif
