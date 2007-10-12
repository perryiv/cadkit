
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Application class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_JSAL_APPLICATION_CLASS_H_
#define _OSG_JSAL_APPLICATION_CLASS_H_

#include "JSAL/OSG/CompileGuard.h"
#include "JSAL/Interfaces/IApplication.h"
#include "JSAL/Interfaces/IChild.h"
#include "JSAL/Interfaces/IQuit.h"
#include "JSAL/Interfaces/IViewer.h"
#include "JSAL/Interfaces/ILight.h"

#include "osgVRJ/Application.h"

namespace osg { class Light; };


namespace JSAL {
namespace OSG {


class Application : public osgVRJ::Application,
                    public JSAL::Interfaces::IApplication,
                    public JSAL::Interfaces::IChild,
                    public JSAL::Interfaces::IQuit,
                    public JSAL::Interfaces::IViewer,
                    public JSAL::Interfaces::ILight
{
public:

  /// Typedefs.
  typedef osgVRJ::Application BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Application );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  Application();

protected:

  typedef Usul::Interfaces::IUnknown Unknown;
  typedef SAL::Interfaces::INode Node;

  /// Do not copy.
  Application ( const Application & );
  Application &operator = ( const Application & );

  /// Use reference counting.
  virtual ~Application();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  JSAL::Interfaces::IChild
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set/get the parent.
  virtual void              parent ( Unknown * );
  virtual Unknown *         parent();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  JSAL::Interfaces::IQuit
  //
  /////////////////////////////////////////////////////////////////////////////

  // Called when program is about to quit.
  virtual void              quit();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  JSAL::Interfaces::IViewer
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set/get the scene.
  virtual void              scene ( Node * );
  virtual Node *            scene();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  JSAL::Interfaces::ILight
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set/get the lighting properties.
  virtual void              getAmbientLightColor ( Vec4f &c ) const;
  virtual void              setAmbientLightColor ( const Vec4f &c );
  virtual void              getDiffuseLightColor ( Vec4f &c ) const;
  virtual void              setDiffuseLightColor ( const Vec4f &c );
  virtual void              getSpecularLightColor ( Vec4f &c ) const;
  virtual void              setSpecularLightColor ( const Vec4f &c );
  virtual void              getLightDirection ( Vec3f &d ) const;
  virtual void              setLightDirection ( const Vec3f &d );
  virtual State             getLightState() const;
  virtual void              setLightState ( State );

  /////////////////////////////////////////////////////////////////////////////

  // Called by the kernel to initialize this instance.
  virtual void              init();

  // Called once for each display to initialize the OpenGL context.
  virtual void              contextInit();

  // Called once for each display every frame.
  virtual void              contextPreDraw();

  // Called before and after the frame is drawn.
  virtual void              preFrame();
  virtual void              postFrame();

private:

  Usul::Interfaces::IUnknown *_parent; // No circular reference.
  USUL_VALID_ACCESS_REF_POINTER(osg::LightSource) _source;
  USUL_VALID_ACCESS_REF_POINTER(osg::Light) _light;
};


}; // namespace OSG
}; // namespace JSAL


#endif // _OSG_JSAL_APPLICATION_CLASS_H_
