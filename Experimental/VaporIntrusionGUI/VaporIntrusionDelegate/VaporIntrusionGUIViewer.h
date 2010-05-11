
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VAPOR_INTRUSION_GUI_VIEWER_CLASS_H_
#define _VAPOR_INTRUSION_GUI_VIEWER_CLASS_H_

#include "MaterialContainer.h"

#include "Helios/Qt/Views/OSG/Viewer.h"

#include "Usul/Math/Vector3.h"
#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "OsgTools/Render/Viewer.h"

#include "osg/Material"

class VaporIntrusionGUIViewer : public CadKit::Helios::Views::OSG::Viewer
{

public:
  typedef CadKit::Helios::Views::OSG::Viewer BaseClass;
  typedef OsgTools::Render::Viewer::CameraOption CameraOption;
  typedef OsgTools::Render::Viewer RenderViewer;
  typedef std::map< std::string, MaterialContainer* > MaterialContainers;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI IVPI;

  VaporIntrusionGUIViewer ( Document *doc, const QGLFormat& format, QWidget* parent, IUnknown* caller, MaterialContainers matContainers );
  //virtual ~VaporIntrusionGUIViewer ();

protected:

  // Override these events.
 
  virtual void                    mouseMoveEvent ( QMouseEvent * );
  virtual void                    mousePressEvent ( QMouseEvent * );
  virtual void                    mouseReleaseEvent ( QMouseEvent * );
  virtual void                    mouseDoubleClickEvent ( QMouseEvent * );
  virtual void                    keyPressEvent ( QKeyEvent * );
  virtual void                    keyReleaseEvent ( QKeyEvent * );
  virtual void                    wheelEvent ( QWheelEvent * );

  void                            _initializeViewDirection();
  void                            _handleArrowKeys( int key, IVPI::QueryPtr document );
  osg::Vec3d                      _getIntersectPoint( QMouseEvent * event );

	void														_checkLeftClick(  QMouseEvent * event );
	void														_checkRightClick(  QMouseEvent * event );
 private:
 
  IUnknown*                       _caller;
  int                             _mouseWheelPosition;
  float                           _mouseWheelSensitivity;
  MaterialContainers              _materialContainers;

};



#endif // _VAPOR_INTRUSION_GUI_VIEWER_CLASS_H_
