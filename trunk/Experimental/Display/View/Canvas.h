
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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

#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Interfaces/IView.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Math/Matrix44.h"

#include "osg/MatrixTransform"
#include "osg/ClipNode"
#include "osg/ref_ptr"
#include "osgViewer/Viewer"

#include <string>
#include <stack>


namespace Display {
namespace View {


class DISPLAY_LIBRARY_EXPORT Canvas : public Usul::Base::BaseObject,
                                      public Usul::Interfaces::IView,
                                      public Usul::Interfaces::IViewMatrix,
                                      public Usul::Interfaces::IRedraw
{
public:

  // Typedefs.
  typedef Usul::Base::BaseObject BaseClass;
  typedef osg::ref_ptr<osg::MatrixTransform> TransformPtr;
  typedef osg::ref_ptr<osg::ClipNode> ClipNodePtr;
  typedef osg::ref_ptr<osg::Group> GroupPtr;
  typedef osg::ref_ptr<osg::Node> NodePtr;
  typedef Display::Render::Renderer::RefPtr RendererPtr;
  typedef std::stack<RendererPtr> Renderers;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef osg::ref_ptr<osgViewer::Viewer> ViewerPtr;
  typedef Usul::Interfaces::IView IView;
  typedef Usul::Interfaces::IDocument IDocument;
  typedef Usul::Math::Matrix44d Matrix;

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

  // Add a model. Specify whether or not it can be clipped.
  void                      modelAdd ( NodePtr, bool clipped = true );

  // Set/get the navigation matrix.
  Matrix                    navigationMatrix() const;
  void                      navigationMatrix ( const Matrix & );

  // Return the matrix that will view all the models.
  Matrix                    navigationViewAll ( double zScale = 3.0 ) const;

  // Push/pop the renderer.
  void                      pushRenderer ( RendererPtr );
  void                      popRenderer();

  // Get the current renderer.
  RendererPtr               renderer();
  const RendererPtr         renderer() const;

  // Render the scene.
  void                      render();

  // Call this when you want the viewport to resize.
  void                      resize ( unsigned int width, unsigned int height );

  // Usul::Interfaces::IViewMatrix
  virtual void              setViewMatrix ( const osg::Matrixd & );
  virtual osg::Matrixd      getViewMatrix() const;

  // Usul::Interfaces::IRedraw
  virtual void              redraw();
  virtual void              setStatsDisplay ( bool );

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
  Renderers _renderers;
  GroupPtr _scene;
  TransformPtr _models;
  ClipNodePtr _clipped;
  GroupPtr _unclipped;
  GroupPtr _decoration;
  IDocument::QueryPtr _document;
  ViewerPtr _viewer;
};


}; // namespace Scene
}; // namespace MTR


#endif // _DISPLAY_LIBRARY_CANVAS_CLASS_H_
