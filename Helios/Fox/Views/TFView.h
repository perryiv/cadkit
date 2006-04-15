
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_VIEWS_TF_VIEW_H__
#define __OSG_VIEWS_TF_VIEW_H__

#include "Export.h"

#include "FoxTools/Headers/GLCanvas.h"

#include "Usul/Interfaces/IViewer.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Math/Vector2.h"

#include "OsgTools/Draggers/Dragger.h"

#include "osg/ref_ptr"
#include "osgUtil/SceneView"

#include <vector>

namespace FX { struct FXEvent; }
namespace osgUtil { class Hit; }

namespace Helios {
namespace Views {


class OSG_FOX_VIEWS_EXPORT TFView : public FX::FXGLCanvas,
                                    public Usul::Interfaces::IViewer
{
public:
  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Useful typedefs.
  typedef FX::FXGLCanvas BaseClass;
  typedef OsgTools::Draggers::Dragger Dragger;

  TFView( Usul::Interfaces::IDocument* document, FX::FXComposite *parent, FX::FXGLVisual *visual );
  virtual ~TFView();

  /// Usul::Interfaces::IViewer
  virtual void            render();
  virtual void            clearScene();
  virtual int             x();
  virtual int             y();
  virtual int             height();
  virtual int             width();
  virtual void            handleMessage ( unsigned short message );

  void build();

  void create();

  long                  onResize            ( FX::FXObject *, FX::FXSelector, void * );
  long                  onPaint             ( FX::FXObject *, FX::FXSelector, void * );
  long                  onFocusIn           ( FX::FXObject *, FX::FXSelector, void * );
  long                  onLeftBtnPress      ( FX::FXObject *, FX::FXSelector, void * );
  long                  onLeftBtnRelease    ( FX::FXObject *, FX::FXSelector, void * );
  long                  onRightBtnPress     ( FX::FXObject *, FX::FXSelector, void * );
  long                  onMotion            ( FX::FXObject *, FX::FXSelector, void * );

private:

  virtual void        _handleDragging   ( const FX::FXEvent &event, Dragger::Command command );
  virtual void        _handlePicking    ( const FX::FXEvent &event );
  virtual bool        _intersect        ( const FX::FXEvent &event, osg::Node *scene, osgUtil::Hit &hit );
  virtual bool        _lineSegment      ( const FX::FXEvent &event, osg::Vec3 &pt0, osg::Vec3 &pt1 );
  virtual void        _findDragger      ( const osgUtil::Hit &hit );

  typedef Usul::Math::Vec2f ControlPoint;
  typedef std::vector< ControlPoint > ControlPoints;

  TFView () { }

  Usul::Interfaces::IDocument::ValidQueryPtr _document;

  float _paddingPercent;
  float _graphPercent;
  osg::ref_ptr< osgUtil::SceneView > _viewer;
  ControlPoints _controlPoints;

  FXDECLARE( TFView )

};

}
}

#endif // __OSG_VIEWS_TF_VIEW_H__
