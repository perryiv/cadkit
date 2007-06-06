///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef COMPASSEVENTHANDLER_H
#define COMPASSEVENTHANDLER_H

#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/Image>
#include <osgViewer/Viewer>

#include <osgDB/ReadFile>

#include <osg/Math>

#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>

#include <osgUtil/Optimizer>

#include <osgDB/Registry>


#include "Compass.h"

class CompassEventHandler : public osgGA::GUIEventHandler
{
public:
	CompassEventHandler(Compass* c);
	CompassEventHandler();
	virtual ~CompassEventHandler();

	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&);
	 


protected:
	

private:
	
	osg::ref_ptr<Compass> _compass;

	
	
	

};

#endif