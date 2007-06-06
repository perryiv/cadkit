///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef COMPASS_H
#define COMPASS_H

#include <iostream>
#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/Image>
#include <osgViewer/Viewer>
#include <osg/PolygonStipple>

#include <osgDB/ReadFile>

#include <osg/Math>

#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>

#include <osgUtil/Optimizer>

#include <osgDB/Registry>

#include "OsgTools/Export.h"


namespace OsgTools {
	namespace Builders {

class OSG_TOOLS_EXPORT Compass : public osg::Referenced
{
public:

	Compass();
	virtual ~Compass();
	
	void setTextureFilename(const std::string& fn);
	osg::Node* getCompass();
	
	bool getAnimationState();
	void setAnimationState(bool state);

	float getRotation();
	void setRotation(float r);

	void setRotationMode(bool m);

	const osg::Vec3& getPosition();
	void setPotion(const osg::Vec3& p);

	float getScale();
	void setScale(float s);

	bool getCompassState();
	
	void buildCompass();
	void keyChange(int key,int value);
	
	void updateCompass();

protected:
	osg::Geode* buildCompassObject();
	osg::MatrixTransform*  initCompass(osg::Node* geode);
	
	void showCompass();
	void hideCompass();
	void emptyCompass();
	
private:
	
	osg::Vec3 _pos;
	float _scale;
	std::string _texfn;
	int _numslices;
	float _radius;
	osg::ref_ptr<osg::Group> _compassgroup;
	osg::ref_ptr<osg::Geode> _compassobject;

	bool _showcompass, _animating;
	bool _rotate_by_view;
	float _animation_start, _animation_end, _animation_step;
	
	float _rotdeg;	

}; //Compass

} //Namespace Builders
} //Namespace OsgTools
#endif
