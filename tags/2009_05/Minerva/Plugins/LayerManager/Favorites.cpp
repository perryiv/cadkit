
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
#include "Minerva/Core/Utilities/Download.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Bind.h"
#include "Usul/App/Application.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Make.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IClonable.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Registry/Database.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/User/Directory.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

#include "QtGui/QHeaderView"
#include "QtGui/QMenu"


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs with file scope.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Threads::Guard<Usul::Threads::Mutex> Guard;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Favorites::Favorites(Usul::Interfaces::IUnknown* caller, QWidget* parent ) : BaseClass( parent ),
  _caller ( caller ),
  _serverFavorites(),
  _favoritesMap(),
  _downloadJob ( 0x0 ),
  _mutex(),
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

  // We want extended selection.
  //_favoritesTree->selectionMode ( QAbstractItemView::ExtendedSelection );

  // Read from server.
  // Note: passing null for the caller because, if you pass the variable 
  // "caller" then the job will get an interface to IProgressBar which points 
  // to an CadKit::Helios::Core::ProgressBarDock::ProgressBar object. 
  // However, if the user shuts down the app before this job gets set to 
  // null, then it's likely the object will have a dangling pointer to 
  // "_progressBar" to a Qt widget that was already destroyed. The progess 
  // bar factor and supporting book-keeping code needs to be re-visited.
  _downloadJob = Usul::Jobs::create ( Usul::Adaptors::memberFunction ( this, &Favorites::_readFavoritesFromServer ), 0x0 );
  _downloadJob->name ( "Favorites Download Job" );

  // Add the job to the manager.
  Usul::Jobs::Manager::instance().addJob ( _downloadJob );

  this->_buildTree();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Favorites::~Favorites()
{
  this->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

void Favorites::clear()
{
  // Copy the job pointer.
  Usul::Jobs::Job::RefPtr job ( 0x0 );
  Guard guard ( _mutex );
  {
    job = _downloadJob;
  }

  // Done with the job member.
  _downloadJob = 0x0;

  // Wait for the job.
  if ( true == job.valid() )
  {
    Usul::Jobs::Manager::instance().cancel ( job );
    job->wait();
    job = 0x0;
  }

  // Save favorites.
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Favorites::_saveState ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to check the cancelled state.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  bool isCancelled ( Usul::Jobs::Job::RefPtr &job, Usul::Threads::Mutex &mutex )
  {
    Guard guard ( mutex );

    // Has the job been cancelled in one way or another?
    const bool invalid ( false == job.valid() );
    const bool cancelled ( ( true == job.valid() ) ? job->canceled() : false );
    return ( invalid || cancelled );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to throw if cancelled.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void checkCancelled ( Usul::Jobs::Job::RefPtr &job, Usul::Threads::Mutex &mutex )
  {
    if ( true == Helper::isCancelled ( job, mutex ) )
    {
      throw Usul::Exceptions::Canceled();
    }
  }
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
      //emit layerAdded ( unknown );
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

  // Check for the cancelled state. This matters when the server is slow 
  // and the user shuts down right away (before this job has finished).
  Helper::checkCancelled ( _downloadJob, _mutex );

  // File to download to.
  std::string name ( Usul::File::Temp::file() );
  Usul::Scope::RemoveFile remove ( name );

  Helper::checkCancelled ( _downloadJob, _mutex );

  // Attempt to download the file.
  if ( false == Minerva::Core::Utilities::downloadToFile ( url, name ) )
    return;

  Helper::checkCancelled ( _downloadJob, _mutex );

  Serialize::XML::DataMemberMap map;
  map.addMember ( "favorites", _serverFavorites );

  XmlTree::XercesLife life;
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->load ( name );

  Helper::checkCancelled ( _downloadJob, _mutex );

  map.deserialize ( *document );
  
  Helper::checkCancelled ( _downloadJob, _mutex );

  // Make sure the tree is rebuilt.
  QMetaObject::invokeMethod ( this, "_buildTree", Qt::QueuedConnection );

  // We're done with this member.
  {
    Guard guard ( _mutex );
    _downloadJob = 0x0;
  }
}
