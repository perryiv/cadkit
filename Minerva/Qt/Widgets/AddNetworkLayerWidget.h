
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADD_NETWORK_LAYER_WIDGET_H__
#define __ADD_NETWORK_LAYER_WIDGET_H__

#include "Minerva/Qt/Widgets/AddWmsLayerWidget.h"
#include "Minerva/Qt/Widgets/WmsLayerItem.h"

#include "Minerva/Core/Layers/LayerInfo.h"
#include "Minerva/Core/Layers/RasterGroup.h"
#include "Minerva/Interfaces/IAddLayer.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Network/Names.h"
#include "Usul/Registry/Convert.h"
#include "Usul/Registry/Database.h"
#include "Usul/Registry/Qt.h"
#include "Usul/Strings/Split.h"

#include "QtGui/QWidget"
#include "QtGui/QStringListModel"

#include <algorithm>
#include <vector>
#include <string>

class QButtonGroup;
class QStringListModel;

namespace Usul { namespace Interfaces { struct IUnknown; } }


namespace Minerva {
namespace Widgets {


template<class Layer>
class AddNetworkLayerWidget : public BaseAddNetworkLayerWidget
{
public:
  typedef BaseAddNetworkLayerWidget BaseClass;
  
  AddNetworkLayerWidget ( QWidget *parent = 0x0 );
  virtual ~AddNetworkLayerWidget();
  
  virtual void apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown * caller );
  
private:
  
  typedef typename Layer::Extents Extents;
  typedef QList<QTreeWidgetItem *> Items;
  typedef Minerva::Core::Layers::RasterLayer RasterLayer;
  
  LayerInfos   _getCapabilities();
  RasterLayer* _makeGroup ( const Items& items, const std::string& format ) const;
  Layer*       _makeLayer ( const Extents& e, const std::string& format, const std::string& layers, const std::string& styles ) const;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template < class Layer >
inline AddNetworkLayerWidget<Layer>::AddNetworkLayerWidget ( QWidget *parent ) : BaseClass ( Layer::defaultOptions(), Layer::defaultCacheDirectory(), parent )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

template < class Layer >
inline AddNetworkLayerWidget<Layer>::~AddNetworkLayerWidget()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add images to caller.
//
///////////////////////////////////////////////////////////////////////////////

template < class Layer >
inline void AddNetworkLayerWidget<Layer>::apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown * caller )
{
  Minerva::Interfaces::IAddLayer::QueryPtr al ( parent );
  
  // Make sure we have a valid interface.
  if ( false == al.valid() )
    return;
  
  const std::string server ( this->server() );
  const std::string cacheDirectory ( this->cacheDirectory() );
  
  // Check for valid state...
  if ( true == server.empty () || true == cacheDirectory.empty() )
    return;
  
  // Get the checked button.
  QAbstractButton *button ( _imageTypes->checkedButton () );
  std::string format ( 0x0 != button ? button->text().toStdString() : "image/jpeg" );
  
  const bool addAllAsGroup      ( Qt::Checked == _addAllAsGroup->checkState() );
  const bool addSelectedAsGroup ( Qt::Checked == _addSelectedAsGroup->checkState() );
  
  if ( addAllAsGroup || addSelectedAsGroup )
  {    
    // Get all the items.
    Items items ( addAllAsGroup ? _layersTree->findItems ( "*", Qt::MatchWildcard ) : _layersTree->selectedItems() );
    
    al->addLayer ( Usul::Interfaces::IUnknown::QueryPtr ( this->_makeGroup ( items, format ) ) );
  }
  else
  {
    Extents extents ( 0, 0, 0, 0 );
    
    Items items ( _layersTree->selectedItems() );
    
    QStringList layers;
    QStringList styles;
    
    // Get the layers and styles.
    for ( Items::const_iterator iter = items.begin(); iter != items.end(); ++iter )
    {
      if ( WmsLayerItem *item = dynamic_cast<WmsLayerItem*> ( *iter ) )
      {
        layers.push_back( item->name().c_str() );
        styles.push_back( item->style().c_str() );
        
        extents.expand ( item->extents() );
      }
    }
    
    // Make the layer.
    typename Layer::ValidRefPtr layer ( this->_makeLayer ( extents, format, layers.join(",").toStdString(), styles.join(",").toStdString() ) );
    
    // Get the name
    const std::string name ( _name->text().toStdString() );
    
    // Set the name.
    layer->name ( false == name.empty() ? name : server );
    
    al->addLayer ( Usul::Interfaces::IUnknown::QueryPtr ( layer.get () ) );
  }
  
  // Save the server names.
  this->_addRecentServer ( server );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the capabilities.
//
///////////////////////////////////////////////////////////////////////////////

template < class Layer >
inline BaseAddNetworkLayerWidget::LayerInfos AddNetworkLayerWidget<Layer>::_getCapabilities()
{
  const std::string server ( _server->text().toStdString() );
  
  typedef typename Layer::LayerInfos LayerInfos;
  LayerInfos infos ( Layer::availableLayers ( server ) );
  
  return infos;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a WMS layer.
//
///////////////////////////////////////////////////////////////////////////////

template < class Layer >
inline Layer* AddNetworkLayerWidget<Layer>::_makeLayer ( const Extents& extents, const std::string& format, const std::string& layers, const std::string& styles ) const
{  
  // Get the current options.
  Options options ( _options );
  
  options[Usul::Network::Names::LAYERS] = layers;
  options[Usul::Network::Names::STYLES] = styles;
  
  // Set the format.
  options[Usul::Network::Names::FORMAT] = format;

  // Get the server.
  std::string server ( _server->text().toStdString() ); 

  // Sometimes the '?' character is needed in the url to make the GetCapabilities 
  // query. It should not become part of the url; it's an option.
  std::string::iterator i ( std::find ( server.begin(), server.end(), '?' ) );
  if ( server.end() != i )
  {
    // Split the sub-string to the right of the '?'
    typedef std::vector<std::string> StringList;
    StringList arguments;
    Usul::Strings::split ( std::string ( i + 1, server.end() ), '&', false, arguments );
    for ( StringList::const_iterator j = arguments.begin(); j != arguments.end(); ++j )
    {
      StringList argument;
      Usul::Strings::split ( *j, '=', false, argument );
      if ( 2 == argument.size() )
      {
        options[argument.at(0)] = argument.at(1);
      }
    }

    // Reset the server url.
    server = std::string ( server.begin(), i );
  }

  // Make a new layer.
  typename Layer::RefPtr layer ( new Layer );

  // Set the options.
  layer->options ( options );
  
  // Set the extents.
  layer->extents ( extents );
  
  // Set the base url.
  layer->urlBase ( server );
  
  // Set the cache directory.
  const std::string cacheDirectory ( _cacheDirectory->text().toStdString() );
  layer->baseCacheDirectory ( cacheDirectory, Qt::Checked == _makeDefaultDirectory->checkState() );
  
  return layer.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a raster group.
//
///////////////////////////////////////////////////////////////////////////////

template < class Layer >
inline Minerva::Core::Layers::RasterLayer* AddNetworkLayerWidget<Layer>::_makeGroup ( const Items& items, const std::string& format ) const
{
  // Get the name
  const std::string name ( _name->text().toStdString() );
  
  // Get the server.
  const std::string server ( _server->text().toStdString() );
  
  // Make a group.
  Minerva::Core::Layers::RasterGroup::RefPtr group ( new Minerva::Core::Layers::RasterGroup );
  group->name ( false == name.empty() ? name : server );
  
  // Possibly used below in the loop.
  unsigned int count ( 0 );
  
  for ( Items::const_iterator iter = items.begin(); iter != items.end(); ++iter )
  {
    if ( WmsLayerItem *item = dynamic_cast<WmsLayerItem*> ( *iter ) )
    {
      typename Layer::ValidRefPtr layer ( this->_makeLayer ( item->extents(), format, item->name().c_str(), item->style().c_str()  ) );

      std::string t ( item->title() );
      std::string n ( item->name() );
      if ( ( false == t.empty() ) && ( false == n.empty() ) && ( t != n ) )
      {
        n = Usul::Strings::format ( n, ": ", t );
      }
      else if ( false == t.empty() )
      {
        n = t;
      }

      layer->name ( ( false == n.empty() ) ? n : Usul::Strings::format ( group->name(), ": ", ++count ) );

      layer->showLayer ( false );
      group->append ( layer.get() );
    }
  }

  return group.release();
}


}
}

#endif // __ADD_WMS_LAYER_WIDGET_H__
