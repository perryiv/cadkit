
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

#include "Minerva/Qt/Widgets/BaseAddNetworkLayerWidget.h"

#include "Minerva/Core/Layers/LayerInfo.h"
#include "Minerva/Core/Layers/RasterGroup.h"
#include "Minerva/Interfaces/IAddLayer.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Network/Names.h"
#include "Usul/Strings/Split.h"

#include <algorithm>
#include <vector>
#include <string>


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
  
  LayerInfoList _getCapabilities();
  RasterLayer* _makeGroup ( const LayerInfoList& items, const std::string& format ) const;
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
  const std::string format ( this->imageFormat() );
  
  // Check for valid state...
  if ( true == server.empty () || true == cacheDirectory.empty()|| true == format.empty() )
    return;
    
  const bool addAllAsGroup      ( this->addAllAsGroup() );
  const bool addSelectedAsGroup ( this->addSelectedAsGroup() );
  
  if ( addAllAsGroup )
  {
    LayerInfoList items;
    this->layers ( items );
    al->addLayer ( Usul::Interfaces::IUnknown::QueryPtr ( this->_makeGroup ( items, format ) ) );
  }
  else if ( addSelectedAsGroup )
  {    
    LayerInfoList items;
    this->selectedLayers ( items );
    al->addLayer ( Usul::Interfaces::IUnknown::QueryPtr ( this->_makeGroup ( items, format ) ) );
  }
  else
  {
    Extents extents ( 0, 0, 0, 0 );
    
    LayerInfoList items;
    this->selectedLayers ( items );
    
    QStringList layers;
    QStringList styles;
    
    // Get the layers and styles.
    for ( LayerInfoList::const_iterator iter = items.begin(); iter != items.end(); ++iter )
    {
      layers.push_back( iter->name.c_str() );
      styles.push_back( iter->style.c_str() );
      
      extents.expand ( iter->extents );
    }
    
    // Make the layer.
    typename Layer::ValidRefPtr layer ( this->_makeLayer ( extents, format, layers.join(",").toStdString(), styles.join(",").toStdString() ) );
    
    // Get the name
    const std::string name ( this->name() );
    
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
inline BaseAddNetworkLayerWidget::LayerInfoList AddNetworkLayerWidget<Layer>::_getCapabilities()
{
  const std::string server ( this->server() );
  
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
  Options options ( this->options() );
  
  options[Usul::Network::Names::LAYERS] = layers;
  options[Usul::Network::Names::STYLES] = styles;
  
  // Set the format.
  options[Usul::Network::Names::FORMAT] = format;

  // Get the server.
  std::string server ( this->server() ); 

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
  const std::string cacheDirectory ( this->cacheDirectory() );
  layer->baseCacheDirectory ( cacheDirectory, this->makeCacheDirectoryDefault() );
  
  return layer.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a raster group.
//
///////////////////////////////////////////////////////////////////////////////

template < class Layer >
inline Minerva::Core::Layers::RasterLayer* AddNetworkLayerWidget<Layer>::_makeGroup ( const LayerInfoList& items, const std::string& format ) const
{
  // Get the name
  const std::string name ( this->name() );
  
  // Get the server.
  const std::string server ( this->server() );
  
  // Make a group.
  Minerva::Core::Layers::RasterGroup::RefPtr group ( new Minerva::Core::Layers::RasterGroup );
  group->name ( false == name.empty() ? name : server );
  
  // Possibly used below in the loop.
  unsigned int count ( 0 );
  
  for ( LayerInfoList::const_iterator iter = items.begin(); iter != items.end(); ++iter )
  {
    typename Layer::ValidRefPtr layer ( this->_makeLayer ( iter->extents, format, iter->name, iter->style ) );

    std::string t ( iter->title );
    std::string n ( iter->name );
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

  return group.release();
}


}
}

#endif // __ADD_WMS_LAYER_WIDGET_H__
