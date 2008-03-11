
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

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/RasterLayer.h"

#include "Usul/Interfaces/IRead.h"

class ossimImageData;
class ossimImageHandler;
class ossimImageRenderer;
class ossimEquDistCylProjection;
class ossimViewInterface;

namespace Minerva {
namespace Core {
namespace Layers {


class MINERVA_EXPORT RasterLayerOssim : public RasterLayer,
                                        public Usul::Interfaces::IRead
{
public:
  typedef RasterLayer BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;

  USUL_DECLARE_QUERY_POINTERS ( RasterLayerOssim );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  RasterLayerOssim();
  
  virtual IUnknown*     clone() const;

  /// Read.
  virtual void          read ( const std::string& filename, Usul::Interfaces::IUnknown* caller = 0x0, Usul::Interfaces::IUnknown* progress = 0x0 );
  
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

  void                  _open ( const std::string& );

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


} // namespace Layers
} // namespace Core
} // namespace Minerva

#endif // __STAR_SYSTEM_RASTER_LAYER_OSSIM_H__
