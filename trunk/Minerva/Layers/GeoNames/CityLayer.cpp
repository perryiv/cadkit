
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/GeoNames/CityLayer.h"
#include "Minerva/Core/Functions/CacheString.h"
#include "Minerva/Core/TileEngine/Tile.h"

#include "XmlTree/Document.h"
#include "XmlTree/XercesLife.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Make.h"
#include "Usul/File/Remove.h"
#include "Usul/Network/Curl.h"
#include "Usul/Registry/Database.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Threads/Safe.h"

#include "OsgTools/Font.h"

#include "osg/Geode"
#include "osg/MatrixTransform"

#include "osgText/FadeText"

using namespace Minerva::Layers::GeoNames;


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( CityLayer, CityLayer::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( CityLayer );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CityLayer::CityLayer() : BaseClass(),
  _citiesToAdd()
{
  this->_nameSet ( "City Names" );
  this->extents ( Extents ( -180, -90, 180, 90 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CityLayer::~CityLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query Interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* CityLayer::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Minerva::Interfaces::ITilesChangedListener::IID:
    return static_cast<Minerva::Interfaces::ITilesChangedListener*> ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast<Usul::Interfaces::IUpdateListener*> ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void CityLayer::deserialize( const XmlTree::Node &node )
{
  BaseClass::deserialize ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Serialize.
//
///////////////////////////////////////////////////////////////////////////////

void CityLayer::serialize ( XmlTree::Node &parent ) const
{
  Serialize::XML::DataMemberMap dataMemberMap ( Usul::Threads::Safe::get ( this->mutex(), _dataMemberMap ) );
  
  // Don't serialize the layers.
  dataMemberMap.erase ( "layers" );
  
  // Serialize.
  dataMemberMap.serialize ( parent );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tile has been added.
//
///////////////////////////////////////////////////////////////////////////////

void CityLayer::tileAddNotify ( Usul::Interfaces::IUnknown::RefPtr child, Usul::Interfaces::IUnknown::RefPtr )
{
  Minerva::Interfaces::ITile::QueryPtr iTile ( child );
  Minerva::Core::TileEngine::Tile::RefPtr tile ( ( true == iTile.valid() ) ? iTile->tile() : 0x0 );
  if ( false == tile.valid() )
    return;

  Minerva::Core::TileEngine::Tile::Extents extents ( tile->extents() );

  // Get the cities that lay within this tile.
  Cities cities ( this->citiesGet ( extents, tile->level(), 10 ) );

  Guard guard ( this->mutex() );
  _citiesToAdd.insert ( std::make_pair ( child, cities ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tile has been removed.
//
///////////////////////////////////////////////////////////////////////////////

void CityLayer::tileRemovedNotify ( Usul::Interfaces::IUnknown::RefPtr child, Usul::Interfaces::IUnknown::RefPtr )
{
  // Remove the node from the tile.
  this->_removeNode ( child );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all the cities in the extents up to the maximum number allowed.
//
///////////////////////////////////////////////////////////////////////////////

CityLayer::Cities CityLayer::citiesGet ( const Extents& extents, unsigned int level, unsigned int maximumItems ) const
{
  const std::string filename ( this->_makeCacheDirectory ( extents, level ) );

  // If it's not in the cache.
  if ( false == Usul::Predicates::FileExists::test ( filename ) )
  {
    // http://www.geonames.org/export/JSON-webservices.html#citiesJSON
    const std::string url ( "http://ws.geonames.org/cities" );
    const std::string request ( Usul::Strings::format ( url, "?", 
                                "north=",  extents.maximum()[1], 
                                "&south=", extents.minimum()[1], 
                                "&east=",  extents.minimum()[0], 
                                "&west=",  extents.maximum()[0], 
                                "&maxRows=", maximumItems ) );

    bool succeeded ( false );

    // Download to the temp file.
    try
    {
      // The timeout.
      const unsigned int timeout ( Usul::Registry::Database::instance()["network_download"]["cities_layer"]["timeout_milliseconds"].get<unsigned int> ( 600000, true ) );

      Usul::Network::Curl curl ( request, filename );
      curl.download ( timeout, 0x0, "" );
      succeeded = true;
    }
    USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "3454987436" );

    // Return if download did not succeed.
    if ( false == succeeded )
      return Cities();
  }

  // Vector for the cities.
  Cities cities;

  // Download.
  this->_citiesGet ( cities, filename, extents, level, maximumItems );

  // Remove the file if we didn't get any cities.
  if ( true == cities.empty() )
    Usul::File::remove ( filename );

  // return the answer.
  return cities;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the node to the tile.
//
///////////////////////////////////////////////////////////////////////////////

void CityLayer::_addNode ( Usul::Interfaces::IUnknown::RefPtr unknown, osg::Node* node )
{
  if ( 0x0 != node )
  {
    Minerva::Interfaces::ITile::QueryPtr iTile ( unknown );
    Minerva::Core::TileEngine::Tile::RefPtr tile ( ( true == iTile.valid() ) ? iTile->tile() : 0x0 );
    if ( true == tile.valid() )
    {
      // Add the node to the tile.
      tile->addVectorData ( node );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tile was removed.
//
///////////////////////////////////////////////////////////////////////////////

void CityLayer::_removeNode ( Usul::Interfaces::IUnknown::RefPtr tile )
{
  // Cannot remove the node from the tile.  This causes a crash.
  // This should not cause a memory leak because the tiles are purged in the body.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make text.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  osg::Node* makeTextForCity ( const Minerva::Core::TileEngine::Tile& tile, const City& city, unsigned int size = 45, double altitude = 2000 )
  {
    osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );

    // Get the position.
    osg::Vec3d position;
    tile.latLonHeightToXYZ ( city.location()[1], city.location()[0], altitude, position );

    // Set the matrix.
    mt->setMatrix ( osg::Matrix::translate ( position ) );

    // Make the geode.
    osg::ref_ptr<osg::Geode> geode ( new osg::Geode );

    // Make the text.
    osg::ref_ptr<osgText::Text> text ( new osgText::Text );
    text->setFont ( OsgTools::Font::defaultFont() );
    text->setText ( city.name() );
    text->setPosition ( osg::Vec3f ( 0.0, 0.0, 0.0 ) );
    text->setColor( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
    text->setCharacterSize ( size );
    text->setCharacterSizeMode ( osgText::Text::SCREEN_COORDS );
    text->setAutoRotateToScreen ( true );

    // Add the text to the geode.
    geode->addDrawable ( text.get() );

    // Add the geode.
    mt->addChild ( geode.get() );

    return mt.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* CityLayer::_buildScene ( Usul::Interfaces::IUnknown::RefPtr unknown, const Cities& cities )
{
  Minerva::Interfaces::ITile::QueryPtr iTile ( unknown );
  Minerva::Core::TileEngine::Tile::RefPtr tile ( ( true == iTile.valid() ) ? iTile->tile() : 0x0 );
  if ( false == tile.valid() )
    return 0x0;

  // Make a new group.
  osg::ref_ptr<osg::Group> group ( new osg::Group );

  // Loop over the cities and create text.
  for ( Cities::const_iterator iter = cities.begin(); iter != cities.end(); ++iter )
  {
    group->addChild ( Detail::makeTextForCity ( *tile, *iter ) );
  }

  // Return the group.
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all the cities in the extents up to the maximum number allowed.
//
///////////////////////////////////////////////////////////////////////////////

void CityLayer::_citiesGet ( Cities& cities, const std::string& filename, const Extents& extents, unsigned int level, unsigned int maximumItems ) const
{
  // Create the Xml Document.
  XmlTree::Document::RefPtr doc ( new XmlTree::Document );
  doc->load ( filename );

  // Get all the GeoNames.
  typedef XmlTree::Node::Children Children;
  Children children ( doc->find ( "geoname", false ) );

  // Iterate over the nodes.
  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    cities.push_back ( City ( *(*iter) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the cache directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string CityLayer::_makeCacheDirectory ( const Extents& extents, unsigned int level ) const
{
  const std::string baseCacheDirectory ( Feature::defaultCacheDirectory() );
  const std::string directory ( Usul::Strings::format ( baseCacheDirectory, 
                                                        "/Minerva/GeoNames/Cities/", 
                                                        Minerva::Core::Functions::makeLevelString ( level ), "/" ) );
  
  // Make sure the directory is created.
  {
    Guard guard ( this );
    Usul::Functions::safeCallR1 ( Usul::File::make, directory, "9094464280" );
  }
  
  // Make the filename.
  const std::string filename ( Usul::Strings::format ( directory, Minerva::Core::Functions::makeExtentsString ( extents ), ".xml" ) );

  // Return the filename.
  return filename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void CityLayer::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  // Get a copy of the cities.
  CitiesToAdd citiesToAdd ( Usul::Threads::Safe::get ( this->mutex(), _citiesToAdd ) );

  // Clear the cities to add data member.
  {
    Guard guard ( this->mutex() );
    _citiesToAdd.clear();
  }

  // Loop through the tiles and add the cities.
  for ( CitiesToAdd::iterator iter = citiesToAdd.begin(); iter != citiesToAdd.end(); ++iter )
  {
    // Build the scene.
    osg::ref_ptr<osg::Node> node ( this->_buildScene ( iter->first, iter->second ) );

    // Add the node to the tile.
    this->_addNode ( iter->first, node.get() );
  }
}
