
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

#include "Helios/Qt/Views/OSG/Viewer.h"

#include "OsgTools/Render/Viewer.h"

class VaporIntrusionGUIViewer : public CadKit::Helios::Views::OSG::Viewer
{

public:
  typedef CadKit::Helios::Views::OSG::Viewer BaseClass;
  typedef OsgTools::Render::Viewer::CameraOption CameraOption;
  VaporIntrusionGUIViewer ( Document *doc, const QGLFormat& format, QWidget* parent, IUnknown* caller );
  //virtual ~VaporIntrusionGUIViewer ();

  void                    camera( CameraOption option );
  CameraOption            camera();

protected:

  // Override these events.
 
  virtual void            mouseMoveEvent ( QMouseEvent * );
  virtual void            mousePressEvent ( QMouseEvent * );
  virtual void            mouseReleaseEvent ( QMouseEvent * );
  virtual void            keyPressEvent ( QKeyEvent * );
  virtual void            keyReleaseEvent ( QKeyEvent * );
  virtual void            wheelEvent ( QWheelEvent * );

  void                    _initializeViewDirection();

private:
  CameraOption            _cameraDirection;          

};



#endif // _VAPOR_INTRUSION_GUI_VIEWER_CLASS_H_
