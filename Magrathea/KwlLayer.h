
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAGRATHEA_KWL_LAYER_H__
#define __MAGRATHEA_KWL_LAYER_H__

#include "Magrathea/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/IOssimPlanetLayer.h"
#include "Usul/Interfaces/ILayerExtents.h"
#include "Usul/Interfaces/ISerialize.h"

#include "Serialize/XML/Macros.h"

#include "ossimPlanet/ossimPlanetTextureLayer.h"

namespace Magrathea {

class MAGRATHEA_EXPORT KwlLayer : public Usul::Base::Referenced,
                                  public Usul::Interfaces::ILayer,
                                  public Usul::Interfaces::IOssimPlanetLayer,
                                  public Usul::Interfaces::ILayerExtents,
                                  public Usul::Interfaces::ISerialize
{
public:
  typedef Usul::Base::Referenced BaseClass;

  USUL_DECLARE_QUERY_POINTERS ( KwlLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  KwlLayer();
  KwlLayer( const std::string& );

  ossimPlanetTextureLayerStateCode   getStateCode() const;

  void                               open ( );

  /// Get/Set the filename.
  void                               filename ( const std::string& filename );
  const std::string&                 filename () const;

  /// Get the name.
  virtual const std::string&  name() const;

  /// Get/Set show layer
  virtual void                       showLayer( bool b );
  virtual bool                       showLayer() const;

  virtual std::string                guid() const;

protected:
  virtual ~KwlLayer();

  virtual ossimPlanetTextureLayer*           ossimPlanetLayer();

  virtual void                               layerExtents ( double &lat, double &lon, double& height ) const;

private:
  std::string                              _guid;
  std::string                              _filename;
  osg::ref_ptr < ossimPlanetTextureLayer > _layer;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_CLASS_NAME( KwlLayer )
  SERIALIZE_XML_SERIALIZE_FUNCTION
  virtual void deserialize ( const XmlTree::Node& node );
  SERIALIZE_XML_ADD_MEMBER_FUNCTION
};

}

#endif // __MAGRATHEA_KWL_LAYER_H__
