#ifndef a0897ea16_4090_467d_b3f0_d855d546b84c
#define a0897ea16_4090_467d_b3f0_d855d546b84c

class pfMatrix
{
public:
  pfMatrix ( float m00, float m01, float m02, float m03,
             float m10, float m11, float m12, float m13,
             float m20, float m21, float m22, float m23,
             float m30, float m31, float m32, float m33 ){}
};

class pfVec2
{
public:
  pfVec2(){}
  pfVec2 ( float v0, float v1 ){}
  void set ( float v0, float v1 ){}
};

class pfVec3
{
public:
  pfVec3(){}
  pfVec3 ( float v0, float v1, float v2 ){}
  void set ( float v0, float v1, float v2 ){}
};

class pfVec4
{
public:
  pfVec4(){}
  pfVec4 ( float v0, float v1, float v2, float v3 ){}
  void set ( float v0, float v1, float v2, float v3 ){}
};

#endif
