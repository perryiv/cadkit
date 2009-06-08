
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_FAVORITES_H__
#define __MINERVA_FAVORITES_H__

// Just include the .h file here.  
// I haven't figured out how to have cmake generate the .ui file in the local directory.
// CMake places the file in it's build directory.
#include "ui_Favorites.h"

#include "Usul/Interfaces/ILayer.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Threads/Mutex.h"

#include "Serialize/XML/Macros.h"

#include "QtGui/QWidget"

#include <map>

namespace MenuKit { class Menu; }

class Favorites : public QWidget,
                  private Ui::Favorites
{
  Q_OBJECT
public:
  typedef QWidget BaseClass;
  typedef std::map < std::string, Usul::Interfaces::ILayer::QueryPtr > FavoritesMap;
  
  /// Construction/Destruction.
  Favorites( Usul::Interfaces::IUnknown* caller, QWidget* parent = 0x0 );
  virtual ~Favorites();

  void             clear();

  MenuKit::Menu*   menu( Usul::Interfaces::IUnknown *caller );

public slots:
  
  void             addLayer( Usul::Interfaces::IUnknown* layer );

private slots:
  
  void             _removeFavoriteButtonClicked();
  void             _onContextMenuShow ( const QPoint& pos );

  /// Build the tree.
  void             _buildTree();
  
private:

  /// Add a layer.
  void             _addLayer ( Usul::Interfaces::IUnknown *parent, Usul::Interfaces::IUnknown* layer );

  /// Build the menu.
  MenuKit::Menu*   _buildMenu ( const FavoritesMap& map, const std::string& name, Usul::Interfaces::IUnknown *caller );

  /// Save/Restore state.
  void             _saveState();
  void             _restoreState();
  
  /// Get the filename.
  std::string      _filename() const;
  
  /// Read from server.
  void             _readFavoritesFromServer();

  Usul::Interfaces::IUnknown::RefPtr _caller;
  FavoritesMap _serverFavorites;
  FavoritesMap _favoritesMap;
  Usul::Jobs::Job::RefPtr _downloadJob;
  Usul::Threads::Mutex _mutex;
  
  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( gis_favorites );
};


#endif // __MINERVA_FAVORITES_H__
