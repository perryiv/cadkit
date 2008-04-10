
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/Flash/FlashModel/FlashDocument.h"
#include "Experimental/Flash/FlashModel/H5File.h"
#include "Experimental/Flash/FlashModel/Dataset.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Case.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

#include "OsgTools/Box.h"
#include "OsgTools/State/StateSet.h"

#include "osg/MatrixTransform"

#include "hdf5.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( FlashDocument, FlashDocument::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( FlashDocument );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FlashDocument::FlashDocument() : 
  BaseClass ( "Flash Document" ),
  _filenames(),
  _currentTimestep ( 0 ),
  _root ( new osg::Group ),
  _dirty ( true ),
  SERIALIZE_XML_INITIALIZER_LIST
{
  this->_addMember ( "filenames", _filenames );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

FlashDocument::~FlashDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *FlashDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast<Usul::Interfaces::IUpdateListener*> ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::canExport ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::canInsert ( const std::string &file ) const
{
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "flash" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "flash" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::write ( const std::string &filename, Unknown *caller, Unknown *progress ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );
  
  if( "flash" == ext )
  {
    Serialize::XML::serialize ( *this, filename );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::read ( const std::string &filename, Unknown *caller, Unknown *progress )
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );
  
  if( "flash" == ext )
  {
    // Deserialize the xml tree.
    XmlTree::XercesLife life;
    XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
    document->load ( filename );
    this->deserialize ( *document );
  }
  else
  {
    _filenames.push_back ( filename );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _filenames.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

FlashDocument::Filters FlashDocument::filtersSave() const
{
  Filters filters;
  filters.push_back ( Filter ( "Flash xml (*.flash)", "*.flash" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

FlashDocument::Filters FlashDocument::filtersExport() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

FlashDocument::Filters FlashDocument::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "Flash xml (*.flash)", "*.flash" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

FlashDocument::Filters FlashDocument::filtersInsert() const
{
  Filters filters;
  filters.push_back ( Filter ( "Flash HDF5 file (*)", "*" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *FlashDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &FlashDocument::_buildScene ), "3960901428" );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::_buildScene()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  // Remove what we have.
  _root->removeChild ( 0, _root->getNumChildren() );
  
  // Make sure we are in range...
  if ( _currentTimestep < _filenames.size() )
  {
    // Open the file.
    H5File file ( _filenames.at ( _currentTimestep ) );
    
    // Make sure we got a good file handle.
    if ( false == file.isOpen())
      throw std::runtime_error ( "Error 1747059583: Could not open file: " + _filenames.at ( _currentTimestep ) );
    
    // Open the data set.
    Dataset::RefPtr boundingBox ( file.openDataset( "bounding box" ) );
    
    // Make sure we got a good data set handle.
    if ( false == boundingBox.valid() || false == boundingBox->isOpen() )
      throw std::runtime_error ( "Error 1220099573: Could not open data set." );
    
    // Get the rank.
    //const int dimensions ( boundingBox->dimensions() );
    
    // Get the number of elements.
    const hsize_t elements ( boundingBox->elements() );
    
    // Buffer for the data.
    std::vector<double> buffer ( elements );
    
    // Read the data.
    boundingBox->read ( H5T_NATIVE_DOUBLE, &buffer[0] );
    
    // Number of nodes.
    const unsigned int numNodes ( boundingBox->size ( 0 ) );
    
    // Open the data set.
    Dataset::RefPtr levelsData ( file.openDataset( "refine level" ) );
    
    // Make sure we got a good data set handle.
    if ( false == levelsData.valid() || false == levelsData->isOpen() )
      throw std::runtime_error ( "Error 3972712559: Could not open data set." );
    
    // Buffer for refine level.
    std::vector<int> levels ( levelsData->elements() );
    
    // Read the levels.
    levelsData->read ( H5T_NATIVE_INT, &levels[0] );
    
    // Open the data set.
    Dataset::RefPtr nodeData ( file.openDataset( "node type" ) );
    
    // Make sure we got a good data set handle.
    if ( false == nodeData.valid() || false == nodeData->isOpen() )
      throw std::runtime_error ( "Error 1356818230: Could not open data set." );
    
    // Buffer for node type.
    std::vector<int> nodeType ( nodeData->elements() );
    
    // Read the levels.
    nodeData->read ( H5T_NATIVE_INT, &nodeType[0] );
    
    // Colors.
    std::vector<osg::Vec4> colors ( 6 );
    colors[0] = osg::Vec4 ( 1.0, 0.0, 0.0, 1.0 );
    colors[1] = osg::Vec4 ( 0.0, 1.0, 0.0, 1.0 );
    colors[2] = osg::Vec4 ( 0.0, 0.0, 1.0, 1.0 );
    colors[3] = osg::Vec4 ( 1.0, 1.0, 0.0, 1.0 );
    colors[4] = osg::Vec4 ( 1.0, 0.0, 1.0, 1.0 );
    colors[5] = osg::Vec4 ( 0.0, 1.0, 1.0, 1.0 );
    
    // Make bounding boxes.
    for ( unsigned int i = 0; i < numNodes; ++i )
    {
      const bool isLeaf ( 1 == nodeType.at ( i ) );
      
      if ( isLeaf )
      {
        const unsigned int start ( i * 6 );

        osg::BoundingBox bb ( buffer[start    ], buffer[start + 2], buffer[start + 4], 
                              buffer[start + 1], buffer[start + 3], buffer[start + 5] );
      
        bb._min =  bb._min / float ( 1e+20f );
        bb._max =  bb._max / float ( 1e+20f );
        
        OsgTools::ColorBox box ( bb );
        box.color_policy().color ( colors.at ( levels.at ( i ) - 1 ) );
        
        osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
        mt->setMatrix ( osg::Matrix::translate ( bb.center() ) );
        mt->addChild ( box() );
        
        // Add the bounding box.
        _root->addChild ( mt.get() );
      }
    }
    
    // Wire-frame.
    OsgTools::State::StateSet::setPolygonsLines ( _root.get(), true );
    OsgTools::State::StateSet::setLighting ( _root.get(), false );
  }
  
  // No longer dirty.
  this->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  
  // Buid the scene if we need to.
  if ( this->dirty () )
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &FlashDocument::_buildScene ), "3279281359" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::deserialize ( const XmlTree::Node &node )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );
  _dataMemberMap.deserialize ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::dirty ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _dirty = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::dirty () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _dirty;
}
