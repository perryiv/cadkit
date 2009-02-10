
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Layer that treats whole directory as a single elevation layer.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/ElevationLayerDirectory.h"
#include "Minerva/Core/Functions/SearchDirectory.h"
#include "Minerva/Core/Visitors/FindUnknowns.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

using namespace Minerva::Core::Layers;


USUL_FACTORY_REGISTER_CREATOR ( ElevationLayerDirectory );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerDirectory::ElevationLayerDirectory() : BaseClass(),
  _directory()
{
  this->_addMember ( "directory", _directory );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerDirectory::ElevationLayerDirectory ( const ElevationLayerDirectory& rhs ) : BaseClass ( rhs ),
  _directory ( rhs._directory )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerDirectory::~ElevationLayerDirectory()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerDirectory::IUnknown* ElevationLayerDirectory::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr clone ( new ElevationLayerDirectory ( *this ) );
  return clone.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void ElevationLayerDirectory::deserialize ( const XmlTree::Node& node )
{
  BaseClass::deserialize ( node );
  
  this->readDirectory();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the directory.
//
///////////////////////////////////////////////////////////////////////////////

void ElevationLayerDirectory::directory ( const std::string& value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _directory = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string ElevationLayerDirectory::directory() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _directory;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the directory.
//
///////////////////////////////////////////////////////////////////////////////

void ElevationLayerDirectory::readDirectory()
{
  // Clear what we have.
  this->clear();
  
  // Reset our extents.
  this->extents ( Extents ( 0.0, 0.0, 0.0, 0.0 ) );
  
  // Search for all loadable files.
  Minerva::Core::Functions::searchDirectory ( *this, this->directory(), true );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Serialize.
//
///////////////////////////////////////////////////////////////////////////////

void ElevationLayerDirectory::serialize ( XmlTree::Node &parent ) const
{
  Serialize::XML::DataMemberMap dataMemberMap ( Usul::Threads::Safe::get ( this->mutex(), _dataMemberMap ) );
  
  // Don't serialize the layers.
  dataMemberMap.erase ( "layers" );
  
  // Serialize.
  dataMemberMap.serialize ( parent );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int ElevationLayerDirectory::getNumChildNodes() const
{
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child node (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerDirectory::ITreeNode::RefPtr ElevationLayerDirectory::getChildNode ( unsigned int which )
{
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerDirectory::ImagePtr ElevationLayerDirectory::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *job, IUnknown *caller )
{
  Layers layers;
  
  // Get all the layers within the requested extents.
  Minerva::Core::Visitors::FindUnknowns<IRasterLayer>::RefPtr visitor ( new Minerva::Core::Visitors::FindUnknowns<IRasterLayer> ( extents, layers ) );
  this->accept ( *visitor );
 
  // Create the texture from our layers.
  return this->_texture ( layers, extents, width, height, level, job, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the raster data as elevation data.
//
///////////////////////////////////////////////////////////////////////////////

ElevationLayerDirectory::IElevationData::RefPtr ElevationLayerDirectory::elevationData ( 
  double minLon,
  double minLat,
  double maxLon,
  double maxLat,
  unsigned int width,
  unsigned int height,
  unsigned int level,
  Usul::Jobs::Job* job,
  Usul::Interfaces::IUnknown* caller )
{
  Extents requestExtents ( minLon, minLat, maxLon, maxLat );

  Layers layers;
  
  // Get all the layers within the requested extents.
  Minerva::Core::Visitors::FindUnknowns<IRasterLayer>::RefPtr visitor ( new Minerva::Core::Visitors::FindUnknowns<IRasterLayer> ( requestExtents, layers ) );
  this->accept ( *visitor );
 
  // Create the elevation data from our layers.
  return this->_elevationData ( layers, minLon, minLat, maxLon, maxLat, width, height, level, job, caller );
}
