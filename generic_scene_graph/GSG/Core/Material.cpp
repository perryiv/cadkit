
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A material class.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/Core/Precompiled.h"
#include "GSG/Core/Material.h"

using namespace GSG;

GSG_IMPLEMENT_CLONE ( Material );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Material::Material() :
  _ambient(),
  _diffuse(),
  _specular(),
  _emissive(),
  _shininess ( 0 )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Material::Material ( const Material &m ) :
  _ambient   ( m._ambient ),
  _diffuse   ( m._diffuse ),
  _specular  ( m._specular ),
  _emissive  ( m._emissive ),
  _shininess ( m._shininess )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Material::Material (
  const Color &a, 
  const Color &d, 
  const Color &sp, 
  const Color &e, 
  value_type sh ) :
  _ambient   ( a ),
  _diffuse   ( d ),
  _specular  ( sp ),
  _emissive  ( e ),
  _shininess ( sh )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to the colors.
//
///////////////////////////////////////////////////////////////////////////////

const Color &Material::ambient() const
{
  Lock lock ( this );
  return _ambient;
}
Color &Material::ambient()
{
  Lock lock ( this );
  return _ambient;
}
const Color &Material::diffuse() const
{
  Lock lock ( this );
  return _diffuse;
}
Color &Material::diffuse()
{
  Lock lock ( this );
  return _diffuse;
}
const Color &Material::specular() const
{
  Lock lock ( this );
  return _specular;
}
Color &Material::specular()
{
  Lock lock ( this );
  return _specular;
}
const Color &Material::emissive() const
{
  Lock lock ( this );
  return _emissive;
}
Color &Material::emissive()
{
  Lock lock ( this );
  return _emissive;
}
const Material::value_type &Material::shininess() const
{
  Lock lock ( this );
  return _shininess;
}
Material::value_type &Material::shininess()
{
  Lock lock ( this );
  return _shininess;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the material.
//
///////////////////////////////////////////////////////////////////////////////

void Material::set ( const Material &m )
{
  Lock lock ( this );
  _ambient   = m._ambient;
  _diffuse   = m._diffuse;
  _specular  = m._specular;
  _emissive  = m._emissive;
  _shininess = m._shininess;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the material.
//
///////////////////////////////////////////////////////////////////////////////

void Material::set ( const Color &a, const Color &d, const Color &sp, const Color &e, value_type sh )
{
  Lock lock ( this );
  _ambient   = a;
  _diffuse   = d;
  _specular  = sp;
  _emissive  = e;
  _shininess = sh;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are they equal?
//
///////////////////////////////////////////////////////////////////////////////

bool Material::equal ( const Material &m ) const
{
  return
    _ambient   == m._ambient &&
    _diffuse   == m._diffuse &&
    _specular  == m._specular &&
    _emissive  == m._emissive &&
    _shininess == m._shininess;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

Material &Material::operator = ( const Material &rhs )
{
  Lock lock ( this );
  this->set ( rhs );
  return *this;
}
