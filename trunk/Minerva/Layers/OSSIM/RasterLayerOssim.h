
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_RASTER_LAYER_OSSIM_H__
#define __MINERVA_CORE_RASTER_LAYER_OSSIM_H__

#include "Minerva/Core/Layers/RasterLayer.h"

#include "Usul/Interfaces/IRead.h"

class ossimImageData;
class ossimImageHandler;

namespace Minerva {
namespace Layers {
namespace Ossim {


class RasterLayerOssim : public Minerva::Core::Layers::RasterLayer,
                         public Usul::Interfaces::IRead
{
public:
  typedef Minerva::Core::Layers::RasterLayer BaseClass;
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

  void                  _buildImagePyramids ( const std::string &file ) const;

  virtual std::string   _cacheFileExtension() const;
  virtual std::string   _cacheDirectory() const;
  virtual ImagePtr      _convert ( const ossimImageData& data ) const;

  void                  _open ( const std::string& );
  
  virtual ImagePtr      _readImageFile ( const std::string & ) const;

  void                  _updateExtents();
  
  static void           _writeImageFile ( const std::string& filename, ossimImageData* data );

private:
  // Do not use.
  RasterLayerOssim& operator= ( const RasterLayerOssim& );
  
  void                  _destroy();
  
  // Register members for serialization.
  void                  _registerMembers();

  std::string        _filename;
  ossimImageHandler *_handler;

  SERIALIZE_XML_CLASS_NAME( RasterLayerOssim ) 
  SERIALIZE_XML_SERIALIZE_FUNCTION 
  SERIALIZE_XML_ADD_MEMBER_FUNCTION
};


} // namespace Layers
} // namespace Core
} // namespace Minerva

#endif // __MINERVA_CORE_RASTER_LAYER_OSSIM_H__
