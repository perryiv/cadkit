
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for Volume models.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/VolumeModel/VolumeDocument.h"
#include "Helios/Plugins/VolumeModel/ImageReaderWriter.h"
#include "Helios/Plugins/VolumeModel/RawReaderWriter.h"

#include "OsgTools/Images/Image3d.h"
#include "OsgTools/Volume/Texture3DVolume.h"
#include "OsgTools/Volume/GPURayCasting.h"

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"
#include "Usul/Interfaces/IViewport.h"
#include "Usul/Interfaces/IViewMatrix.h"

#include "osg/MatrixTransform"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( VolumeDocument, VolumeDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::VolumeDocument() : BaseClass ( "Volume Document" ),
  _root ( new osg::Group ),
  _projection ( new osg::Projection ),
  _image3D ( 0x0 ),
  _bb ( -1.0, -1.0, -1.0, 1.0, 1.0, 1.0 ),
  _readerWriter ( 0x0 ),
  _dirty ( false ),
  _transferFunctions(),
  _activeTransferFunction()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::~VolumeDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *VolumeDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener * > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VolumeDocument::canExport ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VolumeDocument::canInsert ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "jpeg" || ext == "jpg" || ext == "tiff" || ext == "tif" || ext == "gif" || ext == "png" || ext == "bmp" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VolumeDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "rawvol" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VolumeDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing data.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  if ( false == _readerWriter.valid () )
  {
    // Image reader writer is the default.
    _readerWriter = new ImageReaderWriter;

    std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );
    if ( "rawvol" == ext )
      _readerWriter = new RawReaderWriter;
  }

  if ( _readerWriter.valid () )
    _readerWriter->read ( name, *this, caller );

  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::write ( const std::string &filename, Unknown *caller, Unknown *progress ) const
{
  if ( _readerWriter.valid () )
    _readerWriter->write ( filename, *this, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
  if ( _readerWriter.valid () )
    _readerWriter->clear ( caller );
  _readerWriter = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::Filters VolumeDocument::filtersExport() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::Filters VolumeDocument::filtersInsert() const
{
  Filters filters;
  filters.push_back ( Filter ( "All Images (*.jpeg *.jpg *.tiff *.tif *.png *.gif *.bmp)", "*.jpeg;*.jpg;*.tiff;*.tif;*.png;*.gif;*.bmp" ) );
  filters.push_back ( Filter ( "JPEG (*.jpeg *.jpg)", "*.jpeg;*.jpg" ) );
  filters.push_back ( Filter ( "TIFF (*.tiff *.tif)", "*.tiff;*.tif" ) );
  filters.push_back ( Filter ( "PNG  (*.png)",        "*.png"        ) );
  filters.push_back ( Filter ( "GIF  (*.gif)",        "*.gif"        ) );
  filters.push_back ( Filter ( "BMP  (*.bmp)",        "*.bmp"        ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::Filters VolumeDocument::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "Raw  (*.rawvol)",        "*.rawvol"        ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

VolumeDocument::Filters VolumeDocument::filtersSave() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *VolumeDocument::buildScene ( const BaseClass::Options &opt, Unknown *caller )
{
  this->setStatusBar ( "Building scene..." );

  this->_buildScene ();

  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::dirty ( bool b )
{
  Guard guard ( this->mutex() );
  _dirty = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool VolumeDocument::dirty () const
{
  Guard guard ( this->mutex() );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IViewport::QueryPtr vp ( caller );
  if ( vp.valid () )
    _projection->setMatrix ( osg::Matrix::ortho2D ( vp->x(), vp->width(), vp->y(), vp->height () ) );

  if ( this->dirty ( ) )
    this->_buildScene ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::_buildScene ()
{
  _root->removeChildren ( 0, _root->getNumChildren() );

  if ( _readerWriter.valid () )
  {
#if 1
    osg::ref_ptr < OsgTools::Volume::Texture3DVolume > volume ( new OsgTools::Volume::Texture3DVolume );
    volume->numPlanes ( 256 );
    volume->resizePowerTwo ( true );
    volume->image ( this->image3D() );
    volume->boundingBox ( this->boundingBox () );

    if ( _transferFunctions.size() > 0 )
      volume->transferFunction ( _transferFunctions.at ( _activeTransferFunction ) );

    _root->addChild ( volume.get() );
#else
    osg::ref_ptr < OsgTools::Volume::GPURayCasting > volume ( new OsgTools::Volume::GPURayCasting );
    volume->samplingRate ( 0.05 );
    //volume->resizePowerTwo ( true );
    volume->image ( this->image3D() );
    osg::BoundingBox bb ( osg::Vec3 ( 0.0, 0.0, 0.0 ), osg::Vec3 ( 1.0, 1.0, 1.0 ) );
    volume->boundingBox ( bb );
    //volume->boundingBox ( this->boundingBox () );

    volume->camera ( osg::Vec3 ( 0.50, 0.5, 1.0 ) );

    if ( _transferFunctions.size() > 0 )
      volume->transferFunction ( _transferFunctions.at ( _activeTransferFunction ) );

    osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );

    mt->addChild ( volume.get() );

    _root->addChild ( mt.get() );

    _node = volume.get();


    //_root->addChild ( volume.get() );
#endif
  }

  this->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the 3D image.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::image3D ( osg::Image* image )
{
  {
    Guard guard ( this->mutex() );
    _image3D = image;
  }
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the 3D image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* VolumeDocument::image3D () const
{
  Guard guard ( this->mutex() );
  return _image3D.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::boundingBox ( const osg::BoundingBox& bb )
{
  {
    Guard guard ( this->mutex() );
    _bb = bb;
  }
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

osg::BoundingBox VolumeDocument::boundingBox () const
{
  Guard guard ( this->mutex() );
  return _bb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a transfer function.
//
///////////////////////////////////////////////////////////////////////////////

void VolumeDocument::addTransferFunction ( TransferFunction* tf )
{
  Guard guard ( this->mutex () );
  _activeTransferFunction = _transferFunctions.size();
  _transferFunctions.push_back ( tf );
}
