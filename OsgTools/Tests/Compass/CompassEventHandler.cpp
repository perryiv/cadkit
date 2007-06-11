///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "CompassEventHandler.h"

CompassEventHandler::CompassEventHandler( OsgTools::Builders::Compass* c) : _compass(c) {}
CompassEventHandler::CompassEventHandler(){}
CompassEventHandler::~CompassEventHandler(){}

bool CompassEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	
	switch(ea.getEventType())
	{
		case(osgGA::GUIEventAdapter::KEYDOWN):
		{

			//_compass->keyChange(ea.getKey(),1);
			
			return false;
		}
		case(osgGA::GUIEventAdapter::PUSH):
		{
			//osg::Vec3f eye, center, temp;
			//_compass->setMatrix(_viewer->getCameraWithFocus()->getViewMatrix());
			//_compass->getMatrix().getLookAt(eye, center,temp);

			/*_compass->setMatrix(_viewer->getCamera()->getViewMatrix());
			_compass->getMatrix().getLookAt(eye, temp,temp);*/
			/*_compass->setMatrix(_viewer->getCameraWithFocus()->getProjectionMatrix());
			_compass->getMatrix().getLookAt(temp, center,temp);*/
			
			//_compass->setMouseClickPosition(eye.x(),eye.z(),center.x(),center.z());

			//_compass->setMouseClickPosition(ea.getX(),ea.getY());
			
			
			return false;
		}
		case(osgGA::GUIEventAdapter::RELEASE):
		{
			

			return false;
		}
		case(osgGA::GUIEventAdapter::DRAG):
		{
			if(ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
			{
				//osg::Vec3f eye, center, temp;
				
				//_compass->setMatrix(_viewer->getCameraWithFocus()->getViewMatrix());
				//_compass->getMatrix().getLookAt(eye, center,temp);

				/*_compass->setMatrix(_viewer->getCamera()->getViewMatrix());
				_compass->getMatrix().getLookAt(eye, temp,temp);*/
				/*_compass->setMatrix(_viewer->getCameraWithFocus()->getProjectionMatrix());
				_compass->getMatrix().getLookAt(temp, center,temp);*/

				//_compass->updateModelRotation(eye.x(),eye.z(),center.x(),center.z());


				//_compass->keyChange(ea.getButtonMask(),1);

				//_compass->updateModelRotation(ea.getX(),ea.getY());
			}
			return false;
		}
		case(osgGA::GUIEventAdapter::KEYUP):
		{
			//keyChange(ea.getKey(),0);
			return true;
		}

		default:
			return false;
	}
}