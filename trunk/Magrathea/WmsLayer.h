
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAGRATHEA_WMS_LAYER_H__
#define __MAGRATHEA_WMS_LAYER_H__

#include "Magrathea/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/IOssimPlanetLayer.h"
#include "Usul/Interfaces/ILayerExtents.h"
#include "Usul/Interfaces/ISerialize.h"

#include "Serialize/XML/Macros.h"

#include "ossimPlanet/ossimPlanetWmsImageLayer.h"

namespace Magrathea {

class MAGRATHEA_EXPORT WmsLayer : public Usul::Base::Referenced,
                                  public Usul::Interfaces::ILayer,
                                  public Usul::Interfaces::IOssimPlanetLayer,
                                  public Usul::Interfaces::ILayerExtents,
                                  public Usul::Interfaces::ISerialize
{
public:
  typedef Usul::Base::Referenced BaseClass;
  WmsLayer();

  USUL_DECLARE_QUERY_POINTERS ( WmsLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  void                    server( const std::string& serverString );
  const std::string&      server() const;

  void                    imageType( const std::string& imageType );
  const std::string&      imageType() const;
  
  void                    cacheDirectory( const std::string& directory );
  std::string             cacheDirectory() const;

  void                    transparentFlag( bool flag );
  bool                    transparentFlag() const;

  void                    transparentColorFlag( bool flag );
  bool                    transparentColorFlag() const;
  void                    transparentColor( unsigned int r, unsigned int g, unsigned int b );

  /// Get the name.
  virtual const std::string&  name() const;

  /// Get/Set show layer
  virtual void            showLayer( bool b );
  virtual bool            showLayer() const;

  virtual std::string     guid() const;

protected:
  virtual ~WmsLayer();

  virtual ossimPlanetTextureLayer*         ossimPlanetLayer();

  virtual void                             layerExtents ( double &lat, double &lon, double& height ) const;

private:
  std::string _guid;
  std::string _server;
  std::string _imageType;
  std::string _cacheDirectory;
  bool _transparentFlag;
  bool _transparentColorFlag;
  unsigned int _r, _g, _b;
  osg::ref_ptr < ossimPlanetWmsImageLayer > _wmsImageLayer;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_CLASS_NAME( WmsLayer )
  SERIALIZE_XML_SERIALIZE_FUNCTION
  virtual void deserialize ( const XmlTree::Node& node );
  SERIALIZE_XML_ADD_MEMBER_FUNCTION
};

}

#endif // __MAGRATHEA_WMS_LAYER_H__
