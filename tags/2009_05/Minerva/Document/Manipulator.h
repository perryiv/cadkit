
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

#include "Minerva/Document/Export.h"

#include "Minerva/Core/Navigator.h"

#include "osgGA/MatrixManipulator"

namespace Minerva {
namespace Document {

class MINERVA_DOCUMENT_EXPORT Manipulator : public osgGA::MatrixManipulator
{
public:
  typedef osgGA::MatrixManipulator BaseClass;

  Manipulator ( Minerva::Core::Navigator* navigator );

  /// Set the position of the matrix manipulator using a 4x4 Matrix.
  virtual void setByMatrix ( const osg::Matrixd& matrix );

  /// Set the position of the matrix manipulator using a 4x4 Matrix.
  virtual void setByInverseMatrix ( const osg::Matrixd& matrix );

  /// Get the position of the manipulator as 4x4 Matrix.
  virtual osg::Matrixd getMatrix() const;

  /// Get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.
  virtual osg::Matrixd getInverseMatrix() const;

  /// Move the camera to the default position. 
  virtual void home ( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

  /// Start/restart the manipulator.
  virtual void init ( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

  /// Handle events, return true if handled, false otherwise.
  virtual bool handle ( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

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
  Minerva::Core::Navigator::RefPtr _navigator;
};

}
}

#endif // __MAGRATHEA_MANIPULATOR_H__
