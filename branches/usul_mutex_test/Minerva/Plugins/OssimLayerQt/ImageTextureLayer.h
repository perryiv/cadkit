
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAGRATHEA_IMAGE_TEXTURE_LAYER_H__
#define __MAGRATHEA_IMAGE_TEXTURE_LAYER_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/IOssimPlanetLayer.h"
#include "Usul/Interfaces/ISerialize.h"

#include "Serialize/XML/Macros.h"

#include "ossimPlanet/ossimPlanetOssimImageLayer.h"


class ImageTextureLayer : public Usul::Base::Referenced,
                          public Usul::Interfaces::ILayer,
                          public Usul::Interfaces::IOssimPlanetLayer,
                          public Usul::Interfaces::ISerialize
{
public:
  typedef Usul::Base::Referenced BaseClass;

  USUL_DECLARE_QUERY_POINTERS ( ImageTextureLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  ImageTextureLayer();
  ImageTextureLayer( const std::string& filename );

  ossimPlanetTextureLayerStateCode         getStateCode() const;

  const std::string&                       filename() const;

  /// Get/Set the name.
  virtual std::string                      name() const;
  virtual void                             name ( const std::string& name );

  /// Get/Set show layer
  virtual void                             showLayer( bool b );
  virtual bool                             showLayer() const;

  virtual std::string                      guid() const;

protected:
  virtual ~ImageTextureLayer();

  virtual ossimPlanetTextureLayer*         ossimPlanetLayer();

private:
  std::string _guid;
  std::string _filename;
  std::string _name;
  osg::ref_ptr < ossimPlanetOssimImageLayer > _ossimTextureLayer;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_CLASS_NAME( ImageTextureLayer )
  SERIALIZE_XML_SERIALIZE_FUNCTION
  virtual void deserialize ( const XmlTree::Node& node );
  SERIALIZE_XML_ADD_MEMBER_FUNCTION
};


#endif // __MAGRATHEA_IMAGE_TEXTURE_LAYER_H__
