
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Perspective projection class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DISPLAY_LIBRARY_PERSPECTIVE_PROJECTION_CLASS_H_
#define _DISPLAY_LIBRARY_PERSPECTIVE_PROJECTION_CLASS_H_

#include "Display/Export/Export.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/ISceneGraph.h"


namespace Display {
namespace Projection {


class DISPLAY_LIBRARY_EXPORT Perspective : 
  public Usul::Base::Object,
  public Usul::Interfaces::SceneGraph::IProjectionMatrix,
  public Usul::Interfaces::SceneGraph::IPerspectiveProjection,
  public Usul::Interfaces::SceneGraph::IViewportSet,
  public Usul::Interfaces::SceneGraph::IViewportGet
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::SceneGraph::IProjectionMatrix IProjectionMatrix;
  typedef IProjectionMatrix::Matrix Matrix;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Perspective );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Perspective );

  // Usul::Interfaces::IUnknown
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction.
  Perspective();

  // Usul::Interfaces::SceneGraph::IPerspectiveProjection
  virtual void              getPerspectiveProjection ( double &fieldOfViewDegrees, double &aspectRatio, double &zNear, double &zFar ) const;
  virtual void              setPerspectiveProjection ( double fieldOfViewDegrees, double aspectRatio, double zNear, double zFar );

  // Usul::Interfaces::SceneGraph::IProjectionMatrix
  virtual Matrix            getProjectionMatrix() const;

  // Usul::Interfaces::SceneGraph::IViewportGet
  virtual void              getViewport ( double &width, double &height ) const;

  // Usul::Interfaces::SceneGraph::IViewportSet
  virtual void              setViewport ( double width, double height );

protected:

  // Use reference counting.
  virtual ~Perspective();

  void                      _updateMatrix();

private:

  // No copying or assignment.
  Perspective ( const Perspective & );
  Perspective &operator = ( const Perspective & );

  double _fov;
  double _width;
  double _height;
  double _zNear;
  double _zFar;
  Matrix _matrix;
  bool _dirty;
};


}; // namespace Projection
}; // namespace Display


#endif // _DISPLAY_LIBRARY_PERSPECTIVE_PROJECTION_CLASS_H_
