///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "MaterialChooser.h"

osg::ref_ptr<osg::Material> MaterialChooser::getMaterial (const std::string atom) const
{
	if(_materialMap.count(atom) == 0) {
		addMaterial(atom);
	}
	for(Map::iterator i = _materialMap.begin(); i != _materialMap.end(); ++ i) {
		pair t = *i;
		if(t.first == atom)
			return t.second;
	}
	return NULL;
}

void MaterialChooser::addMaterial(std::string atom) const
{
	int red = rand() % 101;
	int green = rand() % 101;
	int blue = rand() % 101;
	float r = (float) (red / 100);
	float g = (float) (green / 100);
	float b = (float) (blue / 100);
	Material mat = new osg::Material();
	mat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(r, g, b, 1.0));
	mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(r, g, b, 1.0));
	mat->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.5, 0.5, 0.5, 1.0));
	mat->setShininess(osg::Material::FRONT_AND_BACK, 0.1f);
	mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 1.0, 1.0, 1.0));
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
	
}