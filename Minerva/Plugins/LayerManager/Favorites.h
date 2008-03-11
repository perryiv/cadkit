
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

#include "Serialize/XML/Macros.h"

#include "QtGui/QWidget"

#include <map>

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

signals:
  
  void             layerAdded ( Usul::Interfaces::IUnknown* layer );
  
public slots:
  
  void             addLayer( Usul::Interfaces::IUnknown* layer );

private slots:
  
  void             on_addLayer_clicked();
  void             on_removeFavoriteButton_clicked();

private:
  
  /// Save/Restore state.
  void             _saveState();
  void             _restoreState();
  
  /// Get the filename.
  std::string      _filename() const;
  
  /// Build the tree.
  void             _buildTree();
  
  Usul::Interfaces::IUnknown::RefPtr _caller;
  FavoritesMap _favoritesMap;
  
  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( gis_favorites );
};


#endif // __MINERVA_FAVORITES_H__
