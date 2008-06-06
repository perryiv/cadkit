
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_FOX_VIEWS_OPEN_GL_CONTEXT_H__
#define __OSG_FOX_VIEWS_OPEN_GL_CONTEXT_H__

#include "Helios/Fox/Views/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IOpenGLContext.h"

namespace Helios {
namespace Views {

class FoxCanvas;

class OSG_FOX_VIEWS_EXPORT FoxContext : public Usul::Base::Referenced, 
                                        public Usul::Interfaces::IOpenGLContext
{
public:
  typedef Usul::Base::Referenced BaseClass;

  FoxContext ( FoxCanvas &canvas ) : _canvas ( canvas ) { }
  virtual ~FoxContext() { }

  /// Usul::Interfaces::IOpenGLContext
  virtual void    makeCurrent();
  virtual void    swapBuffers();

  void ref ();
  void unref ( bool allowDeletion );
  Usul::Interfaces::IUnknown* queryInterface ( unsigned long iid );

private:
  FoxCanvas& _canvas;
};

}
}

#endif // __OSG_FOX_VIEWS_OPEN_GL_CONTEXT_H__
