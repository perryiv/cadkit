
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_H__
#define __MINERVA_LAYERS_KML_H__

#include "Minerva/Layers/Kml/Link.h"

#include "Minerva/Core/Data/Container.h"
#include "Minerva/Core/Data/Geometry.h"
#include "Minerva/Core/Data/Style.h"
#include "Minerva/Interfaces/IRefreshData.h"

#include "Usul/Interfaces/IRead.h"
#include "Usul/Interfaces/ITimerNotify.h"

#include <vector>

namespace Minerva { namespace Core { namespace Data { class DataObject; class ModelCache; } } }
namespace XmlTree { class Node; }

namespace Minerva {
namespace Layers {
namespace Kml {
  
  class NetworkLink;
  class Feature;

class KmlLayer : public Minerva::Core::Data::Container,
                 public Usul::Interfaces::IRead,
                 public Minerva::Interfaces::IRefreshData,
                 public Usul::Interfaces::ITimerNotify
{
public:

  /// Typedefs.
  typedef Minerva::Core::Data::Container             BaseClass;
  typedef Minerva::Core::Data::DataObject            DataObject;
  typedef Minerva::Core::Data::Geometry              Geometry;
  typedef Minerva::Core::Data::ModelCache            ModelCache;
  typedef Minerva::Core::Data::Style                 Style;
  typedef std::map<std::string,Style::RefPtr>        Styles;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( KmlLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  KmlLayer();

  // Creation functions.
  static KmlLayer*            create ( const XmlTree::Node& node, const std::string& filename, const std::string& directory, const Styles& styles, ModelCache * );
  static KmlLayer*            create ( Link* link, const Styles& styles, ModelCache* );

  // Read the file.
  virtual void                read ( const std::string &filename, Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );
  void                        read ( Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );

  // Update.
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  // Deserialize.
  virtual void                deserialize( const XmlTree::Node &node );

  // Get/Set downloading flag.
  bool                        isDownloading() const;
  void                        downloading( bool b );
  
  // Get the model cache.
  ModelCache*                 modelCache() const;
  
  // Get/Set reading flag.
  bool                        isReading() const;
  void                        reading( bool b );
  
  /// Force a refresh of data (IRefreshData).
  virtual void                refreshData();

  void                        parseFolder ( const XmlTree::Node& node );

  /// Serialize.
  virtual void                serialize ( XmlTree::Node &parent ) const;

protected:

  KmlLayer ( Link* link, const Styles& styles, ModelCache* );
  KmlLayer ( const std::string& filename, const std::string& directory, const Styles& styles, ModelCache* );
  virtual ~KmlLayer();
  
  // Add a timer callback.
  void                        _addTimer();

  // Filename from link.  Will download if needed.
  std::string                 _buildFilename ( Link *link ) const;
  
  // Launch a job to update link.
  void                        _launchUpdateLinkJob();
  
  // Read.
  void                        _read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress );
  
  // Load a kml file.
  void                        _parseKml ( const std::string& filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress );
  void                        _parseKml ( const XmlTree::Node& node, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress );
  
  // Parse xml nodes.
  void                        _parseNode         ( const XmlTree::Node& node );
  void                        _parseStyle        ( const XmlTree::Node& node );
  void                        _parsePlacemark    ( const XmlTree::Node& node );
  Geometry*                   _parseModel        ( const XmlTree::Node& node, Style *style );
  Geometry*                   _parsePoint        ( const XmlTree::Node& node, Style *style );
  Geometry*                   _parsePolygon      ( const XmlTree::Node& node, Style *style );
  Geometry*                   _parseLineString   ( const XmlTree::Node& node, Style *style );
  Geometry*                   _parseLineRing     ( const XmlTree::Node& node, Style *style );
  void                        _parseMultiGeometry ( const XmlTree::Node& node, Style *style, DataObject& object );

	Style*                      _style ( const std::string& name ) const;
  
  // Update link.
  void                        _updateLink ( Usul::Interfaces::IUnknown* caller = 0x0 );
  
  // Called when the timer fires.
  virtual void                timerNotify ( TimerID );

private:
  
  enum STATUS_FLAGS
  {
    DOWNLOADING = 0x00000001,
    READING     = 0x00000002
  };
  
  std::string _filename;
  std::string _directory;
  Link::RefPtr _link;
  double _lastUpdate;
  unsigned int _flags;
	Styles _styles;
  std::pair<ModelCache*,bool> _modelCache;
  TimerID _timerId;
  
  SERIALIZE_XML_CLASS_NAME ( KmlLayer );
};
  
}
}
}


#endif // __MINERVA_LAYERS_KML_H__
