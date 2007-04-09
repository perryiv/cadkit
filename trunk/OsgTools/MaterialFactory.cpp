
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "MaterialFactory.h"
#include "osg/Vec4"

#include "Usul/Adaptors/Random.h"

using namespace OsgTools;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MaterialFactory::MaterialFactory() :
BaseClass(),
_materialMap()
{
  srand(0);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MaterialFactory::MaterialFactory(unsigned int seed)
{
  srand(seed);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MaterialFactory::~MaterialFactory()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Gets Material for given key.
//
///////////////////////////////////////////////////////////////////////////////

osg::Material* MaterialFactory::create ( const std::string& key, float min, float max )
{
  //see if a material exists for the key, if so return it
	Map::iterator i = _materialMap.find ( key );
	if ( _materialMap.end() != i )
    return i->second.get();

  // When we get here we have to add a new material.
  Material mat ( this->create ( min, max ) );
  _materialMap.insert( Map::value_type ( key, mat ) );
  return mat.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a random material
//
///////////////////////////////////////////////////////////////////////////////

osg::Material* MaterialFactory::create( float min, float max )
{
  osg::Vec4 emissive ( 0.0f, 0.0f, 0.0f, 1.0f );
  osg::Vec4 specular ( 0.8f, 0.8f, 0.8f, 1.0f );

  //get random numbers and normalize for rgb values
  Usul::Adaptors::Random< float > rd ( min, max );

	osg::Vec4 diffuse ( rd(), rd(), rd(), 1.0f );
  osg::Vec4 ambient ( diffuse );

  //build a new material
  Material mat ( new osg::Material );
	mat->setAmbient   ( osg::Material::FRONT_AND_BACK, ambient  );
	mat->setDiffuse   ( osg::Material::FRONT_AND_BACK, diffuse  );
	mat->setEmission  ( osg::Material::FRONT_AND_BACK, emissive );
	mat->setSpecular  ( osg::Material::FRONT_AND_BACK, specular );
  mat->setShininess ( osg::Material::FRONT_AND_BACK, 100      );
  
  //return material
  return mat.release();
}
