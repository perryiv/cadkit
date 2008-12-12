
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Canvas class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DISPLAY_LIBRARY_CANVAS_CLASS_H_
#define _DISPLAY_LIBRARY_CANVAS_CLASS_H_

#include "Display/Render/Renderer.h"

#include "Usul/Interfaces/IView.h"
#include "Usul/Interfaces/IDocument.h"

#include "osg/Group"
#include "osg/ref_ptr"
#include "osgViewer/Viewer"

#include <string>


namespace Display {
namespace View {


class DISPLAY_LIBRARY_EXPORT Canvas : public Usul::Base::Object,
                                      public Usul::Interfaces::IView
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;
  typedef osg::ref_ptr<osg::Group> GroupPtr;
  typedef osg::ref_ptr<osg::Node> NodePtr;
  typedef Display::Render::Renderer::RefPtr RendererPtr;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef osg::ref_ptr<osgViewer::Viewer> ViewerPtr;
  typedef Usul::Interfaces::IView IView;
  typedef Usul::Interfaces::IDocument IDocument;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Canvas );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Canvas );

  // Usul::Interfaces::IUnknown
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction.
  Canvas ( IUnknown::RefPtr doc = IUnknown::RefPtr ( 0x0 ) );

  // Flags for this canvas.
  enum
  {
    RENDERING = 0x00000001
  };

  // Clear the internal state. Call this when you're done with it.
  void                      clear();

  // Set/get the document.
  virtual IDocument *       document();
  virtual const IDocument * document() const;
  void                      document ( IUnknown::RefPtr );

  // Return the current flags.
  unsigned int              flags() const;

  // Is this canvas currently rendering?
  bool                      isRendering() const;

  // Add a model.
  void                      modelAdd ( NodePtr );

  // Set/get the renderer.
  void                      renderer ( RendererPtr );
  RendererPtr               renderer();
  const RendererPtr         renderer() const;

  // Render the scene.
  void                      render();

  // Call this when you want the viewport to resize.
  void                      resize ( unsigned int width, unsigned int height );

protected:

  // Use reference counting.
  virtual ~Canvas();

  void                      _setFlag ( unsigned int, bool );

private:

  // No copying or assignment.
  Canvas ( const Canvas & );
  Canvas &operator = ( const Canvas & );

  void                      _destroy();

  unsigned int _flags;
  RendererPtr _renderer;
  GroupPtr _scene;
  GroupPtr _models;
  IDocument::QueryPtr _document;
  ViewerPtr _viewer;
};


}; // namespace Scene
}; // namespace MTR


#endif // _DISPLAY_LIBRARY_CANVAS_CLASS_H_
