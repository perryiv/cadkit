
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlMaterial: A template material class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_MATERIAL_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_MATERIAL_CLASS_H_

#include "SlVec4.h"
#include "SlClamp.h"
#include "SlTruncate.h"


namespace CadKit
{
template<class T> class SlMaterial
{
public:

  // Flags for the valid colors.
  enum
  {
    AMBIENT   = 1,
    DIFFUSE   = 2,
    SPECULAR  = 4,
    EMISSIVE  = 8,
    SHININESS = 16,
    ALL       = 0xFFFFFFFF
  };

  typedef T Type; // For the client.

  SlMaterial();
  SlMaterial ( const SlVec4<T> &ambient, const SlVec4<T> &diffuse, const SlVec4<T> &emissive, const SlVec4<T> &specular, const T &shininess );
  SlMaterial ( const SlMaterial<T> &material );

  void                    clamp ( const T &minColor, const T &maxColor, const T &minShininess, const T &maxShininess );

  const SlVec4<T> &       getAmbient()   const { return _ambient; }
  const SlVec4<T> &       getDiffuse()   const { return _diffuse; }
  const SlVec4<T> &       getSpecular()  const { return _specular; }
  const SlVec4<T> &       getEmissive()  const { return _emissive; }
  const T &               getShininess() const { return _shininess; }
  const unsigned int &    getValid()     const { return _valid; }

  SlVec4<T> &             getAmbient()   { return _ambient; }
  SlVec4<T> &             getDiffuse()   { return _diffuse; }
  SlVec4<T> &             getSpecular()  { return _specular; }
  SlVec4<T> &             getEmissive()  { return _emissive; }
  T &                     getShininess() { return _shininess; }
  unsigned int &          getValid()     { return _valid; }

  bool                    isEqual      ( const SlMaterial<T> &material, bool checkValidFlag = false ) const;
  bool                    isNotEqual   ( const SlMaterial<T> &material, bool checkValidFlag = false ) const { return ( false == this->isEqual ( material, checkValidFlag ) ); }
  bool                    isValid ( const unsigned int &flags ) const { return ( ( _valid & flags ) == flags ); }

  SlMaterial<T> &         operator = ( const SlMaterial<T> &m ) { this->setValue ( m ); return *this; }

  void                    setAmbient   ( const SlVec4<T> &ambient )   { _ambient   = ambient;   _valid |= AMBIENT; }
  void                    setDiffuse   ( const SlVec4<T> &diffuse )   { _diffuse   = diffuse;   _valid |= DIFFUSE; }
  void                    setSpecular  ( const SlVec4<T> &specular )  { _specular  = specular;  _valid |= SPECULAR; }
  void                    setEmissive  ( const SlVec4<T> &emissive )  { _emissive  = emissive;  _valid |= EMISSIVE; }
  void                    setShininess ( const T &shininess )         { _shininess = shininess; _valid |= SHININESS; }
  void                    setValid     ( const unsigned int &valid )  { _valid = valid; }
  void                    setValue     ( const SlMaterial<T> &m );
  void                    setValue     ( const SlVec4<T> &ambient, const SlVec4<T> &diffuse, const SlVec4<T> &emissive, const SlVec4<T> &specular, const T &shininess );

  void                    truncate ( const T &negativeZero, const T &positiveZero );

protected:

  SlVec4<T> _ambient;
  SlVec4<T> _diffuse;
  SlVec4<T> _specular;
  SlVec4<T> _emissive;
  T _shininess;
  unsigned int _valid;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Additional operators. These are not members of the class because compilers
//  vary too much in the proper syntax for friend functions in templates. 
//  See http://gcc.gnu.org/faq.html#friend and http://www.bero.org/gcc296.html
//
///////////////////////////////////////////////////////////////////////////////

template<class T> bool operator == ( const SlMaterial<T> &left, const SlMaterial<T> &right );
template<class T> bool operator != ( const SlMaterial<T> &left, const SlMaterial<T> &right );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlMaterial<T>::SlMaterial() :
  _ambient   ( 0, 0, 0, 1 ),
  _diffuse   ( 0, 0, 0, 1 ),
  _specular  ( 0, 0, 0, 1 ),
  _emissive  ( 0, 0, 0, 1 ),
  _shininess ( 0 ),
  _valid     ( 0 )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlMaterial<T>::SlMaterial (
  const SlVec4<T> &ambient, 
  const SlVec4<T> &diffuse, 
  const SlVec4<T> &emissive, 
  const SlVec4<T> &specular, 
  const T &shininess ) :
  _ambient   ( ambient ),
  _diffuse   ( diffuse ),
  _specular  ( specular ),
  _emissive  ( emissive ),
  _shininess ( shininess ),
  _valid     ( ALL )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlMaterial<T>::SlMaterial ( const SlMaterial &m ) :
  _ambient   ( m._ambient ),
  _diffuse   ( m._diffuse ),
  _specular  ( m._specular ),
  _emissive  ( m._emissive ),
  _shininess ( m._shininess ),
  _valid     ( m._valid )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clamp the values.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMaterial<T>::clamp ( const T &minColor, const T &maxColor, const T &minShininess, const T &maxShininess )
{
  // Ignore the _valid flag and clamp all values. Doesn't matter if we 
  // clamp invalid values, the _valid flag will still say it is invalid.
  _ambient.clamp  ( minColor, maxColor );
  _diffuse.clamp  ( minColor, maxColor );
  _specular.clamp ( minColor, maxColor );
  _emissive.clamp ( minColor, maxColor );
  CadKit::clamp ( minShininess, maxShininess, _shininess );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Truncate to zero the values in the range [negativeZero,positiveZero].
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMaterial<T>::truncate ( const T &negativeZero, const T &positiveZero )
{
  _ambient.truncate  ( negativeZero, positiveZero );
  _diffuse.truncate  ( negativeZero, positiveZero );
  _specular.truncate ( negativeZero, positiveZero );
  _emissive.truncate ( negativeZero, positiveZero );
  CadKit::truncate ( negativeZero, CadKit::SlConstants<T>::zero(), positiveZero, _shininess );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMaterial<T>::setValue ( 
  const SlVec4<T> &ambient, 
  const SlVec4<T> &diffuse, 
  const SlVec4<T> &emissive, 
  const SlVec4<T> &specular, 
  const T &shininess )
{
  // Set all the colors.
  _ambient.setValue   ( ambient );
  _diffuse.setValue   ( diffuse );
  _specular.setValue  ( specular );
  _emissive.setValue  ( emissive );
  _shininess          = shininess;

  // They are all valid.
  _valid = ALL;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMaterial<T>::setValue ( const SlMaterial<T> &m )
{
  // Call the other one.
  this->setValue ( m._ambient, m._diffuse, m._specular, m._emissive, m._shininess );

  // Reset the flag.
  _valid = m._valid;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are they equal?
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMaterial<T>::isEqual ( const SlMaterial<T> &right, bool checkValidFlag ) const
{
  // See if the colors are equal.
  bool result = (
    _ambient   == right._ambient &&
    _diffuse   == right._diffuse &&
    _specular  == right._specular &&
    _emissive  == right._emissive &&
    _shininess == right._shininess );

  // Return the result, checking "_valid" if we should.
  return ( checkValidFlag ) ? ( result && ( _valid == right._valid ) ) : result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> bool operator == ( const SlMaterial<T> &left, const SlMaterial<T> &right )
{
  return left.isEqual ( right );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Inequality operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> bool operator != ( const SlMaterial<T> &left, const SlMaterial<T> &right )
{
  return left.isNotEqual ( right );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Common types.
//
///////////////////////////////////////////////////////////////////////////////

typedef SlMaterial<long double> SlMaterialld;
typedef SlMaterial<double>      SlMateriald;
typedef SlMaterial<float>       SlMaterialf;


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_TEMPLATE_MATERIAL_CLASS_H_
