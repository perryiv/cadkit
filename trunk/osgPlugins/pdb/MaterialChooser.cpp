///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "MaterialChooser.h"
#include <cassert>

MaterialChooser::Material MaterialChooser::getMaterial (const std::string atom) const
{
	Map::const_iterator i = _materialMap.find ( atom );
	if ( _materialMap.end() != i )
    return i->second;

  // When we get here we have to add a new material.
  return this->addMaterial ( atom );
}

MaterialChooser::Material MaterialChooser::addMaterial(std::string atom) const
{
	/*int red = rand() % 101;
	int green = rand() % 101;
	int blue = rand() % 101;
	float r = (float) (red / 100);
	float g = (float) (green / 100);
	float b = (float) (blue / 100);*/
	const float factor = 1.0f / RAND_MAX;
	const float r = (float) (rand() * factor);
	const float g = (float) (rand() * factor);
	const float b = (float) (rand() * factor);
  assert ( r >= 0 && r <= 1 );
  assert ( g >= 0 && g <= 1 );
  assert ( b >= 0 && b <= 1 );
	Material mat = new osg::Material();
	mat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(r, g, b, 1.0));
	mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(r, g, b, 1.0));
	mat->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.1, 0.1, 0.1, 1.0));
	mat->setShininess(osg::Material::FRONT_AND_BACK, 100);
	mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.8, 0.8, 0.8, 1.0));
	//glmaterial
	/*
	int red = rand() % 101;
	int green = rand() % 101;
	int blue = rand() % 101;
	Color c;
	c.r = (float) (red / 100);
	c.g = (float) (green / 100);
	c.b = (float) (blue / 100);
	c.alpha = 1.0f;
	*/
	_materialMap.insert( pair( atom, mat ) );
	return mat;
}