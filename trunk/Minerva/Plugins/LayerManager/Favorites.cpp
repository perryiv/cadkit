
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#define NOMINMAX
#endif

#include "Minerva/Plugins/LayerManager/Favorites.h"
#include "Minerva/Core/Commands/AddLayer.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Bind.h"
#include "Usul/App/Application.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Make.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Network/Curl.h"
#include "Usul/Interfaces/IClonable.h"
#include "Usul/User/Directory.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

#include "QtGui/QHeaderView"
#include "QtGui/QMenu"

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
  
  // We want a custom context menu.
  _favoritesTree->setContextMenuPolicy ( Qt::CustomContextMenu );

  // Notify us when a context menu is requested.
  connect ( _favoritesTree, SIGNAL ( customContextMenuRequested ( const QPoint& ) ),
            this,  SLOT   ( _onContextMenuShow ( const QPoint& ) ) );

  // We want exteneded selection.
  //_favoritesTree->selectionMode ( QAbstractItemView::ExtendedSelection );

  // Read from server.
  Usul::Jobs::Job::RefPtr job ( Usul::Jobs::create ( Usul::Adaptors::memberFunction ( this, &Favorites::_readFavoritesFromServer ), caller ) );
  
  // Add the job to the manager.
  Usul::Jobs::Manager::instance().addJob ( job );

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
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void Favorites::_addLayer ( Usul::Interfaces::IUnknown *parent, Usul::Interfaces::IUnknown* unknown )
{
  // Return now if unknown is null.
  if ( 0x0 == unknown )
    return;

  Usul::Interfaces::IClonable::QueryPtr clonable ( unknown );
    
  if( clonable.valid() )
  {
    // Clone the "template"
    Usul::Interfaces::ILayer::QueryPtr layer ( clonable->clone() );
    
    if( layer.valid() )
    {
      // Make sure that the favorite is shown.
      layer->showLayer( true );
    
      // Add the layer.
      Minerva::Core::Commands::AddLayer::RefPtr addLayer ( new Minerva::Core::Commands::AddLayer ( parent, layer.get() ) );
      addLayer->execute ( Usul::Documents::Manager::instance().activeDocument() );
      
      // Emit the layer added signal.
      emit layerAdded ( unknown );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the favorite.
//
///////////////////////////////////////////////////////////////////////////////

void Favorites::_removeFavoriteButtonClicked()
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
//  Add items from map.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class Map >
  inline void addItems ( const Map& map, QTreeWidgetItem* parent )
  {
    for ( typename Map::const_iterator iter = map.begin(); iter != map.end(); ++iter )
    {
      // Make the item.
      QTreeWidgetItem *item ( new QTreeWidgetItem ( parent ) );
      item->setText ( 0, iter->first.c_str() );
      parent->addChild ( item );
    }
  }
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

  // Make the item.
  QTreeWidgetItem *server ( new QTreeWidgetItem ( _favoritesTree ) );
  QTreeWidgetItem *user   ( new QTreeWidgetItem ( _favoritesTree ) );

  // Set the text.
  server->setText ( 0, "Server" );
  user->setText   ( 0, "User" );

  // Add the items.
  Helper::addItems ( _serverFavorites, server );
  Helper::addItems ( _favoritesMap, user );
  
  // Add items to tree.
  _favoritesTree->addTopLevelItem ( server );
  _favoritesTree->addTopLevelItem ( user );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Make the menu.
//
///////////////////////////////////////////////////////////////////////////////

MenuKit::Menu* Favorites::menu ( Usul::Interfaces::IUnknown *caller )
{
  // Make the menu.
  MenuKit::Menu::RefPtr menu ( new MenuKit::Menu );
  menu->append ( this->_buildMenu ( _serverFavorites, "Server", caller ) );
  menu->append ( this->_buildMenu ( _favoritesMap, "User", caller ) );
  return menu.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

MenuKit::Menu* Favorites::_buildMenu ( const FavoritesMap& map, const std::string& name, Usul::Interfaces::IUnknown *caller )
{
  // Shorten the lines.
  typedef MenuKit::Menu Menu;
  typedef MenuKit::Button Button;
  namespace UA = Usul::Adaptors;
  namespace UC = Usul::Commands;

  // Make the menu.
  Menu::RefPtr menu ( new Menu ( name ) );

  // Add buttons to the menu.
  for ( FavoritesMap::const_iterator iter = map.begin(); iter != map.end(); ++iter )
  {
    menu->append ( new Button ( UC::genericCommand ( iter->first, UA::bind2<void> ( caller, iter->second.get(), UA::memberFunction<void> ( this, &Favorites::_addLayer ) ), UC::TrueFunctor() ) ) );
  }

  return menu.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the context menu.
//
///////////////////////////////////////////////////////////////////////////////

void Favorites::_onContextMenuShow ( const QPoint& pos )
{
  // Get the current item.
  QTreeWidgetItem * item ( _favoritesTree->currentItem() );
  
  // Return if no layer.
  if ( 0x0 == item )
    return;

  QMenu menu;

  QAction action ( "Remove", 0x0 );
  connect ( &action, SIGNAL ( triggered() ), this, SLOT ( _removeFavoriteButtonClicked() ) );
  menu.addAction ( &action );

  menu.exec ( _favoritesTree->mapToGlobal ( pos ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read from server.
//
///////////////////////////////////////////////////////////////////////////////

void Favorites::_readFavoritesFromServer()
{
  const std::string server ( "www.minerva-gis.org" );
  const std::string file ( "gis_favorites.xml" );

  const std::string url ( "http://" + server + "/" + file );

  // File to download to.
	std::string name ( Usul::File::Temp::file() );
	Usul::Scope::RemoveFile remove ( name );
  
	// Download.
	{
		Usul::Network::Curl curl ( url, name );
		Usul::Functions::safeCallV1V2 ( Usul::Adaptors::memberFunction ( &curl, &Usul::Network::Curl::download ), static_cast<std::ostream*> ( 0x0 ), "", "3274576290" );
	}

  Serialize::XML::DataMemberMap map;
  map.addMember ( "favorites", _serverFavorites );

  XmlTree::XercesLife life;
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->load ( name );

  map.deserialize ( *document );
  
  // Make sure the tree is rebuilt.
  QMetaObject::invokeMethod ( this, "_buildTree", Qt::QueuedConnection );
}
