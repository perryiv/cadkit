#ifndef _PF_MATERIAL_H_
#define _PF_MATERIAL_H_

#include "Performer/pr/pfObject.h"

#include "Standard/SlMaterial.h"
#include "Standard/SlVec4IO.h"

#define PFMTL_AMBIENT   1034806887
#define PFMTL_DIFFUSE   1034806888
#define PFMTL_SPECULAR  1034806889
#define PFMTL_EMISSION  1034806890


class pfMaterial : public pfObject
{
public:

  pfMaterial() : pfObject(){}

  virtual const char *          className() const { return "pfMaterial"; }

  const CadKit::SlMaterialf &   getMaterial() const { return _material; }
  
  void                          setColor ( int color, float r, float g, float b );
  void                          setShininess ( float shininess ) { _material.setShininess ( shininess ); }

  virtual void                  write ( std::ostream &out ) const;

protected:
  
  CadKit::SlMaterialf _material;

  virtual ~pfMaterial(){}

};

inline void pfMaterial::write ( std::ostream &out ) const
{
  out << _indent << this->className() << "::ambient   = " << _material.getAmbient()   << std::endl;
  out << _indent << this->className() << "::diffuse   = " << _material.getDiffuse()   << std::endl;
  out << _indent << this->className() << "::specular  = " << _material.getSpecular()  << std::endl;
  out << _indent << this->className() << "::emissive  = " << _material.getEmissive()  << std::endl;
  out << _indent << this->className() << "::shininess = " << _material.getShininess() << std::endl;
  pfObject::write ( out );
}

inline void pfMaterial::setColor ( int color, float r, float g, float b )
{
  switch ( color )
  {
  case PFMTL_AMBIENT:
    _material.setAmbient ( CadKit::SlVec4f ( r, g, b, 1.0 ) );
    break;
  case PFMTL_DIFFUSE:
    _material.setDiffuse ( CadKit::SlVec4f ( r, g, b, 1.0 ) );
    break;
  case PFMTL_SPECULAR:
    _material.setSpecular ( CadKit::SlVec4f ( r, g, b, 1.0 ) );
    break;
  case PFMTL_EMISSION:
    _material.setEmissive ( CadKit::SlVec4f ( r, g, b, 1.0 ) );
    break;
  default:
    SL_ASSERT ( 0 ); // Heads up.
    break;
  }
}

#endif
