
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The camera path class.
//
///////////////////////////////////////////////////////////////////////////////

#include "PathAnimation/CameraPath.h"
#include "Serialize/XML/Deserialize.h"
#include "Serialize/XML/Serialize.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

USUL_FACTORY_REGISTER_CREATOR ( CameraPath );

USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_3 ( CameraPath::Triplet );
USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_3 ( CameraPath::Triplet );
SERIALIZE_XML_DECLARE_VECTOR_3_WRAPPER ( CameraPath::Triplet );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CameraPath::CameraPath() : BaseClass ( "Camera Path Document" ),
  _values()
{
  USUL_TRACE_SCOPE;

  this->_addMember ( "values", _values );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CameraPath::~CameraPath()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the camera info.
//
///////////////////////////////////////////////////////////////////////////////

void CameraPath::camera ( Usul::Math::Vec3d &eye, Usul::Math::Vec3d &center, Usul::Math::Vec3d &up, unsigned int num ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  if ( num < this->size() )
  {
    Triplet t ( _values.at ( num ) );
    eye = t[0];
    center = t[1];
    up = t[2];
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the camera information.
//
///////////////////////////////////////////////////////////////////////////////

void CameraPath::append ( const Usul::Math::Vec3d &eye, const Usul::Math::Vec3d &center, const Usul::Math::Vec3d &up )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _values.insert ( _values.end(), Triplet ( eye, center, up ) );
  this->modified ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prepend the camera information.
//
///////////////////////////////////////////////////////////////////////////////

void CameraPath::prepend ( const Usul::Math::Vec3d &eye, const Usul::Math::Vec3d &center, const Usul::Math::Vec3d &up )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _values.insert ( _values.begin(), Triplet ( eye, center, up ) );
  this->modified ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert the camera between the two nearest.
//
///////////////////////////////////////////////////////////////////////////////

void CameraPath::insertBetweenNearest ( const Usul::Math::Vec3d &eye, const Usul::Math::Vec3d &center, const Usul::Math::Vec3d &up )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // If the path does not have at least two then append.
  if ( _values.size() < 2 )
  {
    this->append ( eye, center, up );
    return;
  }

  // The value to insert.
  const Triplet camera ( eye, center, up );

  // If there are two camers then it's trivial.
  if ( 2 == _values.size() )
  {
    _values.insert ( _values.begin() + 1, camera );
    return;
  }

  // Find the nearest camera.
  const unsigned int nearest ( this->_closest ( eye ) );
  if ( nearest >= _values.size() )
    throw std::runtime_error ( "Error 2921860230: Failed to find nearest camera" );

  // Is the nearest camera the first one?
  if ( 0 == nearest )
  {
    _values.insert ( _values.begin() + 1, camera );
    return;
  }

  // Is the nearest camera the last one?
  if ( ( _values.size() - 1 ) == nearest )
  {
    _values.insert ( _values.begin() + _values.size() - 1, camera );
    return;
  }

  // When we get here we know that there are at least 3 cameras, and that the 
  // nearest camera is not either end. See which neighboring camera is closer.
  const double distBefore ( eye.distanceSquared ( _values.at ( nearest - 1 )[0] ) );
  const double distAfter  ( eye.distanceSquared ( _values.at ( nearest + 1 )[0] ) );

  // Insert between nearest and the one before it.
  if ( distBefore < distAfter )
  {
    _values.insert ( _values.begin() + nearest, camera );
  }

  // Insert between nearest and the one after it.
  else
  {
    _values.insert ( _values.begin() + nearest + 1, camera );
  }

  // The path is modified.
  this->modified ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the camera closest to the given eye position.
//
///////////////////////////////////////////////////////////////////////////////

void CameraPath::removeNearest ( const Usul::Math::Vec3d &eye, const Usul::Math::Vec3d &, const Usul::Math::Vec3d & )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle empty path.
  if ( true == _values.empty() )
    return;

  // Find the nearest camera.
  const unsigned int nearest ( this->_closest ( eye ) );
  if ( nearest >= _values.size() )
    throw std::runtime_error ( "Error 4726092730: Failed to find nearest camera" );

  // Remove the nearest camera.
  _values.erase ( _values.begin() + nearest );

  // The path is modified.
  this->modified ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Replace the camera closest to the given eye position with the new values.
//
///////////////////////////////////////////////////////////////////////////////

void CameraPath::replaceNearest ( const Usul::Math::Vec3d &eye, const Usul::Math::Vec3d &center, const Usul::Math::Vec3d &up )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle empty path.
  if ( true == _values.empty() )
    return;

  // Find the nearest camera.
  const unsigned int nearest ( this->_closest ( eye ) );
  if ( nearest >= _values.size() )
    throw std::runtime_error ( "Error 4274807095: Failed to find nearest camera" );

  // Update the nearest camera.
  _values.at ( nearest ) = Triplet ( eye, center, up );

  // The path is modified.
  this->modified ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close the path if we can.
//
///////////////////////////////////////////////////////////////////////////////

void CameraPath::closePath()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( false == this->canClose() )
    return;

  _values.insert ( _values.end(), _values.front() );
  this->modified ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can we close the path?
//
///////////////////////////////////////////////////////////////////////////////

bool CameraPath::canClose() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( _values.size() > 1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can we play the animation?
//
///////////////////////////////////////////////////////////////////////////////

bool CameraPath::canPlay() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( _values.size() > 1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool CameraPath::canExport ( const std::string &file ) const
{
  return this->canSave ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool CameraPath::canInsert ( const std::string &file ) const
{
  return this->canSave ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool CameraPath::canOpen ( const std::string &file ) const
{
  return this->canSave ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool CameraPath::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "cpf" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

CameraPath::Filters CameraPath::filtersExport() const
{
  return this->filtersSave();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

CameraPath::Filters CameraPath::filtersInsert() const
{
  return this->filtersSave();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
//////////////////////////////////////////////////////////////////////////////

CameraPath::Filters CameraPath::filtersOpen() const
{
  return this->filtersSave();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

CameraPath::Filters CameraPath::filtersSave() const
{
  Filters filters;
  filters.push_back ( Filter ( "Camera Path (*.cpf)", "*.cpf" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read.
//
///////////////////////////////////////////////////////////////////////////////

void CameraPath::read ( const std::string &file, Unknown *caller, Unknown *progress )
{
  XmlTree::XercesLife life;
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->load ( file );
  this->deserialize ( *document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write.
//
///////////////////////////////////////////////////////////////////////////////

void CameraPath::write ( const std::string &file, Unknown *caller, Unknown *progress  ) const
{
  Serialize::XML::serialize ( *this, file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear all the paths.
//
///////////////////////////////////////////////////////////////////////////////

void CameraPath::clear ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _values.clear();
  this->modified ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of cameras.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int CameraPath::size() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _values.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the values.
//
///////////////////////////////////////////////////////////////////////////////

void CameraPath::values ( Values &v, bool reverseOrder ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( true == reverseOrder )
  {
    v.assign ( _values.rbegin(), _values.rend() );
  }
  else
  {
    v.assign ( _values.begin(), _values.end() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the closest camera.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int CameraPath::_closest ( const Usul::Math::Vec3d &eye ) const
{
  USUL_TRACE_SCOPE;

  // Copy the values.
  Values values ( Usul::Threads::Safe::get ( this->mutex(), _values ) );

  // Initialize.
  unsigned int nearest ( values.size() );
  double minDist ( std::numeric_limits<double>::max() );

  // Loop through the cameras.
  for ( unsigned int i = 0; i < values.size(); ++i )
  {
    const double currentDist ( eye.distanceSquared ( values.at(i)[0] ) );
    if ( currentDist < minDist )
    {
      minDist = currentDist;
      nearest = i;
    }
  }

  // Return nearest, which could be the end.
  return nearest;
}
