
//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAGRATHEA_MANIPULATOR_H__
#define __MAGRATHEA_MANIPULATOR_H__

#include "Magrathea/Export.h"
#include "Magrathea/Navigator.h"

#include "ossimPlanet/ossimPlanetManipulator.h"

namespace Magrathea {

class MAGRATHEA_EXPORT Manipulator : public ossimPlanetManipulator
{
public:
  typedef ossimPlanetManipulator BaseClass;

  Manipulator();

  void gotoLocation( double lat, double lon, double height );

  /** set the position of the matrix manipulator using a 4x4 Matrix.*/
   virtual void setByMatrix(const osg::Matrixd& matrix);
   
   /** set the position of the matrix manipulator using a 4x4 Matrix.*/
   virtual void setByInverseMatrix(const osg::Matrixd& matrix) { setByMatrix(osg::Matrixd::inverse(matrix)); }
   
   /** get the position of the manipulator as 4x4 Matrix.*/
   virtual osg::Matrixd getMatrix() const;
   
   /** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
   virtual osg::Matrixd getInverseMatrix() const;
   
   /** Move the camera to the default position. 
       May be ignored by manipulators if home functionality is not appropriate.*/
   virtual void home(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);
   
   /** Start/restart the manipulator.*/
   virtual void init(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);
   
   /** handle events, return true if handled, false otherwise.*/
   virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);

   // Set the planet.
   void         planet( ossimPlanet * planet );
protected:
  virtual ~Manipulator();

  void          _flushEvents();
  void          _addEvent ( const osgGA::GUIEventAdapter& ea );

  bool          _mouseButtonPressed( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us );
  bool          _mouseButtonReleased( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us );
  bool          _mouseDragged( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us );

  bool          _calculateMovement();

private:

  // Last two events.
  osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t1;
  osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t0;

  // The navigator.
  osg::ref_ptr < Navigator > _navigator;
};

}

#endif // __MAGRATHEA_MANIPULATOR_H__
