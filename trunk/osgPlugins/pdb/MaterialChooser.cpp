
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "MaterialChooser.h"
#include "osg/Vec4"

#include <cassert>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MaterialChooser::MaterialChooser()
{
  srand(0);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MaterialChooser::MaterialChooser(unsigned int seed)
{
  srand(seed);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MaterialChooser::~MaterialChooser()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Gets Material for given atom.
//
///////////////////////////////////////////////////////////////////////////////

MaterialChooser::Material MaterialChooser::getMaterial (const std::string atom) const
{
  //see if a material exists for the atom, if so return it
	Map::const_iterator i = _materialMap.find ( atom );
	if ( _materialMap.end() != i )
    return i->second;

  // When we get here we have to add a new material.
  return this->addMaterial ( atom );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Adds new material for atom to the map.
//
///////////////////////////////////////////////////////////////////////////////

MaterialChooser::Material MaterialChooser::addMaterial(std::string atom) const
{
  //get random numbers and normalize for rgb values
	const float factor = 1.0f / RAND_MAX;
	float r = (float) (rand() * factor);
	float g = (float) (rand() * factor);
	float b = (float) (rand() * factor);
  assert ( r >= 0 && r <= 1 );
  assert ( g >= 0 && g <= 1 );
  assert ( b >= 0 && b <= 1 );
  
  //disallow colors close to white or black
  if(r < .2 && g < .2 && b < .2)
    r = g = b = 0.2f;
  if(r > .8 && g > .8 && b > .8)
    r = g = b = 0.8f;
	
  //build a new material
  Material mat = new osg::Material();
	mat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(r, g, b, 1.0));
	mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(r, g, b, 1.0));
	mat->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.1, 0.1, 0.1, 1.0));
	mat->setShininess(osg::Material::FRONT_AND_BACK, 100);
	mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.8, 0.8, 0.8, 1.0));
  
  //add material to the map
	_materialMap.insert( pair( atom, mat ) );
	return mat;
}
