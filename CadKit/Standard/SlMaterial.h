
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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


namespace CadKit
{
template<class T> class SlMaterial
{
public:

  // Flags for the valid colors.
  enum
  {
    AMBIENT   = (1<<0),
    DIFFUSE   = (1<<1),
    SPECULAR  = (1<<2),
    EMISSIVE  = (1<<3),
    SHININESS = (1<<4),
    ALL       = 0xFFFFFFFF
  };

  typedef T Type; // For the client.

  SlMaterial();
  SlMaterial ( const SlVec4<T> &ambient, const SlVec4<T> &diffuse, const SlVec4<T> &emissive, const SlVec4<T> &specular, const T &shinniness );
  SlMaterial ( const SlMaterial<T> &material );

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

  bool                    isValid ( const unsigned int &flags ) const { return ( ( _valid & flags ) == flags ); }

  SlMaterial<T> &         operator = ( const SlMaterial<T> &m ) { this->setValue ( m ); return *this; }

  void                    setAmbient   ( const SlVec4<T> &ambient )   { _ambient   = ambient;   _valid |= AMBIENT; }
  void                    setDiffuse   ( const SlVec4<T> &diffuse )   { _diffuse   = diffuse;   _valid |= DIFFUSE; }
  void                    setSpecular  ( const SlVec4<T> &specular )  { _specular  = specular;  _valid |= SPECULAR; }
  void                    setEmissive  ( const SlVec4<T> &emissive )  { _emissive  = emissive;  _valid |= EMISSIVE; }
  void                    setShininess ( const T &shininess )         { _shininess = shininess; _valid |= SHININESS; }
  void                    setValid     ( const unsigned int &valid )  { _valid = valid; }
  void                    setValue     ( const SlMaterial<T> &m );
  void                    setValue     ( const SlVec4<T> &ambient, const SlVec4<T> &diffuse, const SlVec4<T> &emissive, const SlVec4<T> &specular, const T &shinniness );

  bool                    isEqual      ( const SlMaterial<T> &material, bool checkValidFlag = false ) const;
  bool                    isNotEqual   ( const SlMaterial<T> &material, bool checkValidFlag = false ) const { return ( false == this->isEqual ( material, checkValidFlag ) ); }

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
  const T &shinniness ) :
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
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMaterial<T>::setValue ( 
  const SlVec4<T> &ambient, 
  const SlVec4<T> &diffuse, 
  const SlVec4<T> &emissive, 
  const SlVec4<T> &specular, 
  const T &shinniness )
{
  // Set all the colors.
  _ambient.setValue   ( _ambient );
  _diffuse.setValue   ( _diffuse );
  _specular.setValue  ( _specular );
  _emissive.setValue  ( _emissive );
  _shininess          = _shininess;

  // They are all valid.
  _valid = ALL;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
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
