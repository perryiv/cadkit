
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

#include "Display/Export/Export.h"

#include "Usul/Base/BaseObject.h"
#include "Usul/Interfaces/ISceneGraph.h"
#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Interfaces/IView.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Math/Matrix44.h"

#include <string>
#include <stack>


namespace Display {
namespace View {


class DISPLAY_LIBRARY_EXPORT Canvas : 
  public Usul::Base::BaseObject,
  public Usul::Interfaces::IRedraw,
  public Usul::Interfaces::IView
{
public:

  // Typedefs.
  typedef Usul::Base::BaseObject BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Math::Matrix44d Matrix;
  typedef Usul::Interfaces::SceneGraph::IGroup IGroup;
  typedef Usul::Interfaces::SceneGraph::IProjectionMatrix IProjectionMatrix;
  typedef std::stack<IUnknown::RefPtr> Renderers;
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

  // Add a model. Specify whether or not it can be clipped.
  void                      modelAdd ( IUnknown::RefPtr, bool clipped = true );

  // Set/get the navigation matrix.
  Matrix                    navigationMatrix() const;
  void                      navigationMatrix ( const Matrix & );

  // Return the matrix that will view all the models.
  Matrix                    navigationViewAll ( double zScale = 3.0 ) const;

  // Set/get the projection.
  void                      projection ( IUnknown::RefPtr );
  IUnknown::RefPtr          projection();

  // Push/pop the renderer.
  void                      pushRenderer ( IUnknown::RefPtr );
  void                      popRenderer();

  // Get the current renderer.
  IUnknown::RefPtr          renderer();
  const IUnknown::RefPtr    renderer() const;

  // Render the scene.
  void                      render();

  // Call this when the window has resized.
  void                      resize ( double width, double height );

  // Usul::Interfaces::IRedraw
  virtual void              redraw();
  virtual void              setStatsDisplay ( bool );

protected:

  // Use reference counting.
  virtual ~Canvas();

  IUnknown::RefPtr          _getScene() const;

  void                      _setFlag ( unsigned int, bool );

private:

  // No copying or assignment.
  Canvas ( const Canvas & );
  Canvas &operator = ( const Canvas & );

  void                      _destroy();

  unsigned int _flags;
  Renderers _renderers;
  IGroup::QueryPtr _scene;
  IGroup::QueryPtr _models;
  IGroup::QueryPtr _clipped;
  IGroup::QueryPtr _unclipped;
  IGroup::QueryPtr _decoration;
  IDocument::QueryPtr _document;
  IProjectionMatrix::QueryPtr _projection;
};


}; // namespace View
}; // namespace Display


#endif // _DISPLAY_LIBRARY_CANVAS_CLASS_H_
