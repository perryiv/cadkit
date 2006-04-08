
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for voxel models.
//
///////////////////////////////////////////////////////////////////////////////

#include "VoxelDocument.h"

#include "OsgTools/Drawables/VoxelGeometry.h"

#include "osg/Geode"
#include "osg/TexEnv"

#include "Images/Core/Image.h"

#include "Usul/Errors/Assert.h"
#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"
#include "Usul/Policies/Update.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Resources/StatusBar.h"
#include "Usul/Resources/ProgressBar.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Interfaces/IStatusBar.h"
#include "Usul/Interfaces/IProgressBar.h"

#include <iostream>

using OsgTools::Drawables::VoxelGeometry;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( VoxelDocument, VoxelDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

VoxelDocument::VoxelDocument() : BaseClass ( "Voxel Document" ),
  _imageMap(),
  _texture ( new osg::Texture3D )
{
  // Set texture attributes.
  _texture->setFilter ( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR        );
  _texture->setFilter ( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR        );
  _texture->setWrap   ( osg::Texture3D::WRAP_S,     osg::Texture3D::CLAMP_TO_BORDER );
  _texture->setWrap   ( osg::Texture3D::WRAP_T,     osg::Texture3D::CLAMP_TO_BORDER );
  _texture->setWrap   ( osg::Texture3D::WRAP_R,     osg::Texture3D::CLAMP_TO_BORDER );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

VoxelDocument::~VoxelDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *VoxelDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VoxelDocument::canExport ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VoxelDocument::canInsert ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "jpeg" || 
           ext == "jpg"  ||
           ext == "tiff" || 
           ext == "tif"  ||
           ext == "gif"  ||
           ext == "png"  ||
           ext == "bmp"  );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VoxelDocument::canOpen ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool VoxelDocument::canSave ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing data.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelDocument::read ( const std::string &name, Unknown *caller )
{
  ImageInfo::ValidRefPtr info ( new ImageInfo ( name ) );
  _imageMap[name] = info.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelDocument::write ( const std::string &filename, Unknown *caller ) const
{
  USUL_ASSERT ( 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
  _imageMap.clear();
  _texture->setImage ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

VoxelDocument::Filters VoxelDocument::filtersExport() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

VoxelDocument::Filters VoxelDocument::filtersInsert() const
{
  Filters filters;
  filters.push_back ( Filter ( "All Images (*.jpeg *.jpg *.tiff *.tif *.png *.gif *.bmp)", 
                               "*.jpeg;*.jpg;*.tiff;*.tif;*.png;*.gif;*.bmp" ) );
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

VoxelDocument::Filters VoxelDocument::filtersOpen() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

VoxelDocument::Filters VoxelDocument::filtersSave() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the given variable from the options.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct SetFromOptionsIfFound
  {
    template < class OptionsType > static void set ( const OptionsType &options, const std::string &key, unsigned int &value  )
    {
      typename OptionsType::const_iterator itr = options.find ( key );
      if ( options.end() != itr )
      {
        const std::string num ( itr->second );
        if ( false == num.empty() )
        {
          const int n ( ::atoi ( num.c_str() ) );
          if ( n > 0 )
          {
            value = static_cast < unsigned int > ( n );
          }
        }
      }
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *VoxelDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  // Update the 3D texture.
  this->_updateTexture3D ( options, caller );

  // Make the planes.
  osg::ref_ptr<osg::Drawable> planes ( this->_makePlanes ( options, caller ) );

  // Build the scene.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  geode->addDrawable ( planes.get() );

  // Set the state to support textures.
  this->_setState ( geode.get() );

  // Return the scene.
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify this document of the message.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelDocument::notify ( unsigned short message )
{
  BaseClass::notify ( message );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the planes.
//
///////////////////////////////////////////////////////////////////////////////

osg::Drawable *VoxelDocument::_makePlanes ( const BaseClass::Options &options, Unknown *caller )
{
  // Make the planes.
  osg::ref_ptr<VoxelGeometry> planes ( new VoxelGeometry );

  // Set the number of planes.
  unsigned int num ( planes->numPlanes() );
  Detail::SetFromOptionsIfFound::set ( options, "num_planes", num );
  planes->numPlanes ( num );

  // Return the planes.
  return planes.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the 3D texture.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelDocument::_updateTexture3D ( const BaseClass::Options &options, Unknown *caller )
{
  // If there are no images then just return.
  if ( _imageMap.empty() )
    return;

  // Make a vector of image-info for speed.
  ImageVector images;
  this->_makeVector ( images );

  // Get the 3D image from the texture, or make it.
  osg::ref_ptr<osg::Image> image3d ( _texture->getImage() );
  if ( false == image3d.valid() )
    image3d = new osg::Image;

  // Get the size the volume should be.
  const Usul::Math::Vec3ui size ( this->_volumeSize ( images, caller ) );

  // Should not have any zero dimensions.
  if ( 0 == size[0] || 0 == size[1] || 0 == size[2] )
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 3160384588: One or more of the volume's dimensions is zero",
        ", rows: ", size[0], ", columns: ", size[1], ", layers: ", size[2] );

  // If the size is different then allocate.
  if ( size[0] != image3d->s() || size[1] != image3d->t() || size[2] != image3d->r() )
  {
    std::ostringstream out;
    out << "Allocating new volume: rows = " << size[0] << ", columns = " << size[1] << ", layers = " << size[2];
    this->setStatusBar ( out.str() );
    std::cout << out.str() << Usul::Resources::TextWindow::endl;
    image3d->allocateImage ( size[0], size[1], size[2], GL_RGBA, GL_UNSIGNED_BYTE );
  }

  // Initialize the layer. A single image can have more than one.
  unsigned int layerNum ( 0 );

  // Size of a single layer. This inclues all the color channels.
  const unsigned int layerSize ( image3d->getTotalSizeInBytes() / image3d->r() );

  // Feedback.
  const std::string message ( "Updating 3D texture..." );
  this->setStatusBar ( message );
  std::cout << message << Usul::Resources::TextWindow::endl;

  // Set up progress bar.
  Usul::Interfaces::IProgressBar::UpdateProgressBar progress ( 0, 1, Usul::Resources::progressBar() );

  // Loop through the images.
  for ( ImageVector::iterator i = images.begin(); i != images.end(); ++i )
  {
    // Get image info.
    ImageInfo::ValidRefPtr info ( *i );

    // If this image is dirty...
    if ( info->dirty() )
    {
      // Set the corresponding portion of the 3D image.
      unsigned char *data ( image3d->data ( 0, 0, layerNum ) );
      const unsigned int numBytes ( layerSize * info->layers() );
      info->set ( numBytes, data );

      // This image is no longer dirty.
      info->dirty ( false );

      // Increment the layer (some images may have more than one).
      layerNum += info->layers();

      // Show progress.
      std::cout << "Updating image: " << info->name() << Usul::Resources::TextWindow::endl;
      progress ( static_cast < unsigned int > ( std::distance<ImageVector::iterator> ( images.begin(), i ) ), images.size() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state of the node to support textures.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelDocument::_setState ( osg::Node *node )
{
  // Get or create the state set
  osg::ref_ptr < osg::StateSet > ss ( node->getOrCreateStateSet() );

  // Set the state.
  ss->setMode ( GL_LIGHTING, osg::StateAttribute::INHERIT );
  ss->setMode ( GL_CULL_FACE, osg::StateAttribute::OFF );

  // Add the texture and properties. We make the texture coordinates.
  ss->setTextureAttributeAndModes ( 0, _texture.get(), osg::StateAttribute::ON );
  ss->setTextureMode ( 0, GL_TEXTURE_GEN_S, osg::StateAttribute::ON );
  ss->setTextureMode ( 0, GL_TEXTURE_GEN_T, osg::StateAttribute::ON );
  ss->setTextureMode ( 0, GL_TEXTURE_GEN_R, osg::StateAttribute::ON );
  ss->setTextureAttributeAndModes ( 0, new osg::TexEnv ( osg::TexEnv::REPLACE ), osg::StateAttribute::ON );

  // Set needed states for transparency
  ss->setMode ( GL_BLEND, osg::StateAttribute::ON );
  ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );
  ss->setRenderingHint ( osg::StateSet::TRANSPARENT_BIN );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the volume size.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec3ui VoxelDocument::_volumeSize ( ImageVector &images, Usul::Interfaces::IUnknown *caller ) const
{
  // Initialize the size.
  Usul::Math::Vec3ui size ( 0, 0, 0 );

  // Set status bar.
  if ( Usul::Resources::statusBar() )
    Usul::Resources::statusBar()->setStatusBarText ( "Calculating volume size...", true );

  // Progress.
  Usul::Interfaces::IProgressBar::UpdateProgressBar progress ( 0, 1, Usul::Resources::progressBar() );

  // Loop through the images.
  for ( ImageVector::const_iterator i = images.begin(); i != images.end(); ++i )
  {
    // Get image info.
    ImageInfo::ValidRefPtr info ( *i );

    // Grow the number of rows and columns.
    size[0] = Usul::Math::maximum ( info->rows(),    size[0] );
    size[1] = Usul::Math::maximum ( info->columns(), size[1] );

    // Add the layers.
    size[2] += info->layers();

    // Progress.
    progress ( static_cast < unsigned int > ( std::distance<ImageVector::const_iterator> ( images.begin(), i ) ), images.size() );
  }

  // Return the size.
  return size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a vector from the map.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelDocument::_makeVector ( ImageVector &images )
{
  images.reserve ( _imageMap.size() );
  for ( ImageMap::const_iterator i = _imageMap.begin(); i != _imageMap.end(); ++i )
    images.push_back ( i->second.get() );
}
