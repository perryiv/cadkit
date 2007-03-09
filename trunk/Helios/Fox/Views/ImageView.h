
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FOX_TOOLS_IMAGE_VIEW_H__
#define __FOX_TOOLS_IMAGE_VIEW_H__

#include "Export.h"

#include "FoxTools/Headers/GLCanvas.h"

#include "Usul/Interfaces/IBackground.h"
#include "Usul/Interfaces/IViewer.h"
#include "Usul/Interfaces/IImageView.h"
#include "Usul/Interfaces/IDocument.h"

#include "osg/ref_ptr"
#include "osg/Image"

#include "osgUtil/SceneView"

namespace Helios {
namespace Views {

class OSG_FOX_VIEWS_EXPORT ImageView : public FX::FXGLCanvas,
                                       public Usul::Interfaces::IBackground,
                                       public Usul::Interfaces::IImageView,
                                       public Usul::Interfaces::IViewer
{
public:
  // Typedefs
  typedef FX::FXGLCanvas BaseClass;

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  enum
  {
    ID_BACKGROUND_CHANGED = BaseClass::ID_LAST,
    ID_LAST
  };

  ImageView ( Usul::Interfaces::IDocument* document, FX::FXComposite *parent, FX::FXGLVisual *visual );

  virtual ~ImageView();

  void setImage( osg::Image* img );

  void create();

    // Message maps.
  long                  onBackgroundChanged ( FXObject*, FX::FXSelector, void * );
  long                  onResize            ( FX::FXObject *, FX::FXSelector, void * );
  long                  onPaint             ( FX::FXObject *, FX::FXSelector, void * );

protected:
  ImageView();

  /// Usul::Interfaces::IViewer
  virtual void            render();
  virtual void            clearScene();
  virtual int             x() ;
  virtual int             y() ;
  virtual int    height() ;
  virtual int    width() ;
  virtual void            handleMessage ( unsigned short message );

  /// Usul::Interfaces::IImageView
  virtual Helios::Views::ImageView*       imageView()       { return this; }
  virtual const Helios::Views::ImageView* imageView() const { return this; }

  /// Usul::Interfaces::IBackground
  virtual void            editBackground();
  virtual void            defaultBackground();

private:
  Usul::Interfaces::IDocument::ValidRefPtr _document;

  osg::ref_ptr< osg::Image > _image;

  osg::ref_ptr< osgUtil::SceneView > _viewer;

  FXDECLARE( ImageView )
};

}
}


#endif // __FOX_TOOLS_IMAGE_VIEW_H__
