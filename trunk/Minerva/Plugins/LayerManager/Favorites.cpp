
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/LayerManager/Favorites.h"
#include "Minerva/Core/Commands/AddLayer.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/App/Application.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Make.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IClonable.h"
#include "Usul/User/Directory.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

#include "QtGui/QHeaderView"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Favorites::Favorites(Usul::Interfaces::IUnknown* caller, QWidget* parent ) : BaseClass( parent ),
  _caller ( caller ),
  _favoritesMap(),
  SERIALIZE_XML_INITIALIZER_LIST
{
  this->setupUi( this );
  
  _favoritesTree->header()->setHidden ( 1 );
  
  this->_addMember ( "favorites", _favoritesMap );
  
  // Restore favorites.
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Favorites::_restoreState ) );
  
  this->_buildTree();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Favorites::~Favorites()
{
  // Save favorites.
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Favorites::_saveState ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add layer to the favorites.
//
///////////////////////////////////////////////////////////////////////////////

void Favorites::addLayer( Usul::Interfaces::IUnknown* unknown )
{
  Usul::Interfaces::ILayer::QueryPtr layer ( unknown );
  
  if( layer.valid() )
    _favoritesMap[layer->name()] = layer;
  
  // Build the tree.
  this->_buildTree();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a favorite from the string.
//
///////////////////////////////////////////////////////////////////////////////

void Favorites::on_addLayer_clicked()
{
  // Get the current item.
  QTreeWidgetItem * item ( _favoritesTree->currentItem() );
  
  if ( 0x0 == item )
    return;
  
  std::string name ( item->text( 0 ).toStdString() );
  
  FavoritesMap::const_iterator iter = _favoritesMap.find( name );
  if( iter != _favoritesMap.end() )
  {
    Usul::Interfaces::IClonable::QueryPtr clonable ( iter->second.get() );
    
    if( clonable.valid() )
    {
      // Clone the "template"
      Usul::Interfaces::ILayer::QueryPtr layer ( clonable->clone() );
      
      if( layer.valid() )
      {
        // Make sure that the favorite is shown.
        layer->showLayer( true );
      
        // Add the layer.
        Minerva::Core::Commands::AddLayer::RefPtr addLayer ( new Minerva::Core::Commands::AddLayer ( _caller, layer.get() ) );
        addLayer->execute ( Usul::Documents::Manager::instance().activeDocument() );
        
        // Emit the layer added signal.
        Usul::Interfaces::IUnknown::QueryPtr unknown ( layer );
        emit layerAdded ( unknown.get() );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the favorite.
//
///////////////////////////////////////////////////////////////////////////////

void Favorites::on_removeFavoriteButton_clicked()
{
  // Get the current item.
  QTreeWidgetItem * item ( _favoritesTree->currentItem() );
  
  // Return if no layer.
  if ( 0x0 == item )
    return;
  
  // Get the name of the favorite.
  std::string name ( item->text( 0 ).toStdString() );
  
  // Remove the item from our map.
  FavoritesMap::iterator iter = _favoritesMap.find( name );
  if( iter != _favoritesMap.end() )
    _favoritesMap.erase ( iter );
  
  // Rebuild the tree.
  this->_buildTree();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the tree.
//
///////////////////////////////////////////////////////////////////////////////

void Favorites::_buildTree()
{
  // Clear anything we may have.
  _favoritesTree->clear();
  
  for ( FavoritesMap::const_iterator iter = _favoritesMap.begin(); iter != _favoritesMap.end(); ++iter )
  {
    // Make the item.
    QTreeWidgetItem *item ( new QTreeWidgetItem ( _favoritesTree ) );
    item->setText ( 0, iter->first.c_str() );
    _favoritesTree->addTopLevelItem ( item );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save State.
//
///////////////////////////////////////////////////////////////////////////////

void Favorites::_saveState()
{
   Serialize::XML::serialize ( *this, this->_filename() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Restore State.
//
///////////////////////////////////////////////////////////////////////////////

void Favorites::_restoreState()
{
  const std::string file ( this->_filename() );
  if ( true == Usul::Predicates::FileExists::test ( file ) )
  {
    XmlTree::XercesLife life;
    XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
    document->load ( file );
    this->deserialize ( *document );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filename.
//
///////////////////////////////////////////////////////////////////////////////

std::string Favorites::_filename() const
{
  // Get the vendor and program names.
  const std::string vendor ( Usul::App::Application::instance().vendor() );
  const std::string program ( Usul::App::Application::instance().program() );
  
  // Get persistant directory and make sure it exists.
  const std::string persistantDir ( Usul::User::Directory::vendor ( vendor, true ) + program + "/" );
  Usul::File::make ( persistantDir );
  
  // Build the filename.
  const std::string filename ( persistantDir + "gis_favorites.xml" );
  
  // Return the file name.
  return filename;
}
