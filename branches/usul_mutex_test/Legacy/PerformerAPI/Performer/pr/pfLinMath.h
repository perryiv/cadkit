#ifndef _PF_LIN_MATH_H_
#define _PF_LIN_MATH_H_

#include "Standard/SlVec2.h"
#include "Standard/SlVec3.h"
#include "Standard/SlVec4.h"
#include "Standard/SlMatrix44.h"


class pfMatrix : public CadKit::SlMatrix44<float>
{
public:

  pfMatrix ( float m00, float m01, float m02, float m03,
             float m10, float m11, float m12, float m13,
             float m20, float m21, float m22, float m23,
             float m30, float m31, float m32, float m33 )
  {
    CadKit::SlMatrix44<float> ( m00, m01, m02, m03,
                                m10, m11, m12, m13,
                                m20, m21, m22, m23,
                                m30, m31, m32, m33 );
  }
};


class pfVec2 : public CadKit::SlVec2<float>
{
public:
  pfVec2() : CadKit::SlVec2<float> ( 0.0f, 0.0f ){}
  pfVec2 ( float v0, float v1 ) : CadKit::SlVec2<float> ( v0, v1 ){}
  void set ( float v0, float v1 ) { this->setValue ( v0, v1 ); }
};


class pfVec3 : public CadKit::SlVec3<float>
{
public:
  pfVec3() : CadKit::SlVec3<float> ( 0.0f, 0.0f, 0.0f ){}
  pfVec3 ( float v0, float v1, float v2 ) : CadKit::SlVec3<float> ( v0, v1, v2 ){}
  void set ( float v0, float v1, float v2 ) { this->setValue ( v0, v1, v2 ); }
};


class pfVec4 : public CadKit::SlVec4<float>
{
public:
  pfVec4() : CadKit::SlVec4<float> ( 0.0f, 0.0f, 0.0f, 0.0f ){}
  pfVec4 ( float v0, float v1, float v2, float v3 ) : CadKit::SlVec4<float> ( v0, v1, v2, v3 ){}
  void set ( float v0, float v1, float v2, float v3 ) { this->setValue ( v0, v1, v2, v3 ); }
};


#endif
