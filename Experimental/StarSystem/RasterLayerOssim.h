
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_RASTER_LAYER_OSSIM_H__
#define __STAR_SYSTEM_RASTER_LAYER_OSSIM_H__

#include "StarSystem/Export.h"
#include "StarSystem/RasterLayer.h"

class ossimImageData;
class ossimImageHandler;
class ossimImageRenderer;
class ossimEquDistCylProjection;
class ossimViewInterface;

namespace StarSystem {


class STAR_SYSTEM_EXPORT RasterLayerOssim : public RasterLayer
{
public:
  typedef RasterLayer BaseClass;

  USUL_DECLARE_REF_POINTERS ( RasterLayerOssim );

  RasterLayerOssim();
  
  virtual IUnknown*     clone() const;

  void                  open ( const std::string& );

  /// Get the texture
  virtual ImagePtr      texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );

  /// Deserialize.
  virtual void          deserialize ( const XmlTree::Node &node );

  /// Get filename.
  std::string           filename() const;

protected:
  virtual ~RasterLayerOssim();

  RasterLayerOssim ( const RasterLayerOssim& );
  
  virtual void          _convert ( const ossimImageData& data, osg::Image& image );
  void                  _updateExtents();

private:
  // Do not use.
  RasterLayerOssim& operator= ( const RasterLayerOssim& );
  
  void                  _destroy();
  
  // Register members for serialization.
  void                  _registerMembers();

  std::string        _filename;
  ossimImageHandler *_handler;
  ossimImageRenderer *_renderer;
  ossimViewInterface *_viewInterface;

  ossimEquDistCylProjection *_projection;

  SERIALIZE_XML_CLASS_NAME( RasterLayerOssim ) 
  SERIALIZE_XML_SERIALIZE_FUNCTION 
  SERIALIZE_XML_ADD_MEMBER_FUNCTION
};


}

#endif // __STAR_SYSTEM_RASTER_LAYER_OSSIM_H__
