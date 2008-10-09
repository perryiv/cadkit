
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_CORE_BASE_APPLICATION_H__
#define __VRV_CORE_BASE_APPLICATION_H__

#include "VRV/Export.h"

#include "vrj/Draw/OGL/GlApp.h"

namespace VRV {
namespace Core {


class VRV_EXPORT BaseApplication : public vrj::GlApp
{
public:

  // Typedefs.
  typedef vrj::GlApp                           BaseClass;

  // Constructors.
  BaseApplication();

  // Destructor.
  virtual ~BaseApplication();

protected:

  virtual void                  _init();
  virtual void                  _preFrame();
  virtual void                  _latePreFrame();
  virtual void                  _postFrame();

private:
  
  // Don't allow derived classes to implement these VR Juggler functions.
  // Implement the _function instead.  
  // This is to ensure that the functions are wrapped in a try/catch.

  virtual void            init();
  //virtual void            contextInit();
  virtual void            preFrame();
  virtual void            latePreFrame();
  //virtual void            contextPreDraw();
  //virtual void            contextPostDraw();
  //virtual void            draw();
  //virtual void            contextClose();
  virtual void            postFrame();

};


}
}

#endif // __VRV_CORE_BASE_APPLICATION_H__
