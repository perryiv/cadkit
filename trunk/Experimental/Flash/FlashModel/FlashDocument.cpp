
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/Flash/FlashModel/FlashDocument.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/Color.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Strings/Case.h"
#include "Usul/Threads/Safe.h"

#include "OsgTools/Volume/TransferFunction1D.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

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
  _currentTransferFunction ( 0 ),
  _transferFunctions(),
  _timesteps(),
  SERIALIZE_XML_INITIALIZER_LIST
{
  this->_addMember ( "filenames", _filenames );
  
  this->_buildDefaultTransferFunctions();
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
  case Usul::Interfaces::ITimeVaryingData::IID:
    return static_cast<Usul::Interfaces::ITimeVaryingData*> ( this );
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
    // Do we need to load the data?
    if ( false == this->_hasTimestep( _currentTimestep ) )
      this->_loadTimestep ( _currentTimestep );
    
    // Get the timestep.
    Timestep::RefPtr timestep ( _timesteps[_currentTimestep] );

    // Add the child to the scene.
    if ( timestep.valid() )
      _root->addChild ( timestep->buildScene() );
  }
  
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


///////////////////////////////////////////////////////////////////////////////
//
//  Build the default transfer function.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::_buildDefaultTransferFunctions ()
{
  // Typedefs.
  typedef OsgTools::Volume::TransferFunction1D TransferFunction1D;
  typedef TransferFunction1D::Colors           Colors;
  
  const unsigned int size ( 256 );
  
  Colors hsv       ( size );
  
  // Don't draw voxels with a value of zero.
  hsv.at ( 0 ) [ 3 ] = 0;
  
  const double tmin ( ::log10 ( 1.0 / ( size - 1 ) ) + 3.0 );
  const double tmax ( ::log10 ( 5.0 ) + 3.0 );
  
  for ( unsigned int i = 1; i < size; ++i )
  {
    float value ( static_cast < float > ( i ) / ( size - 1 ) );
    const unsigned char alpha ( 35 );
    
    float r ( 0.0 ), g ( 0.0 ), b ( 0.0 );
    
    double temp ( ( ( ::log10 ( value * 5 ) + 3.0 ) - tmin ) / ( tmax - tmin ) );
    
    Usul::Functions::Color::hsvToRgb ( r, g, b, 300 - static_cast<float> ( temp * 300.0f ), 1.0f, 1.0f );
    //Usul::Functions::Color::hsvToRgb ( r, g, b, 300 - static_cast<float> ( value * 300.0f ), 1.0f, 1.0f );
    hsv.at ( i ) [ 0 ] = static_cast < unsigned char > ( r * 255 );
    hsv.at ( i ) [ 1 ] = static_cast < unsigned char > ( g * 255 );
    hsv.at ( i ) [ 2 ] = static_cast < unsigned char > ( b * 255 );
    hsv.at ( i ) [ 3 ] = static_cast < unsigned char > ( value * alpha );
    //hsv.at ( i ) [ 3 ] = static_cast < unsigned char > ( 127 );
  }
  
  _transferFunctions.push_back ( new TransferFunction1D ( hsv ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current time step.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::setCurrentTimeStep ( unsigned int current )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _currentTimestep = current;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current time step.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int FlashDocument::getCurrentTimeStep () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _currentTimestep;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of timesteps.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int FlashDocument::getNumberOfTimeSteps () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _filenames.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the i'th timestep.
//
///////////////////////////////////////////////////////////////////////////////

void FlashDocument::_loadTimestep ( unsigned int i )
{
  USUL_TRACE_SCOPE;
  
  // Get the filename for the timestep.
  std::string filename ( Usul::Threads::Safe::get ( this->mutex(), _filenames.at ( i ) ) );
  
  // Make the timestep.
  Timestep::RefPtr timestep ( new Timestep ( filename ) );
  timestep->init();
  
  // Add the timestep.
  Guard guard ( this->mutex() );
  _timesteps[i] = timestep;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the i'th timestep loaded?
//
///////////////////////////////////////////////////////////////////////////////

bool FlashDocument::_hasTimestep ( unsigned int i ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Timesteps::const_iterator iter ( _timesteps.find ( i ) );
  return ( iter != _timesteps.end() );
}
