
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Callback to display the meta data for item.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_GEO_RSS_CLICKED_CALLBACK_H__
#define __MINERVA_LAYERS_GEO_RSS_CLICKED_CALLBACK_H__

#include "Minerva/Core/Data/DataObject.h"

#include "Usul/Math/Vector2.h"

namespace Minerva {
namespace Layers {
namespace GeoRSS {  
 

class GeoRSSCallback : public Minerva::Core::Data::ClickedCallback
{
public:
  typedef Minerva::Core::Data::ClickedCallback BaseClass;
  typedef Minerva::Core::Data::DataObject DataObject;
  typedef OsgTools::Widgets::Item Item;
  typedef Usul::Math::Vec2ui Size;
  
  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( GeoRSSCallback );
  
  GeoRSSCallback();
  virtual ~GeoRSSCallback();
  
  virtual Item*       operator() ( const DataObject& object, Usul::Interfaces::IUnknown* caller ) const;
  
  /// Set/get the date.
  void                date ( const std::string& );
  const std::string&  date() const;
  
  /// Set/get the image filename.
  void                imageFilename ( const std::string& );
  const std::string&  imageFilename() const;
  
  /// Set/get the image size.
  void                imageSize ( unsigned int width, unsigned int height );
  Size                imageSize() const;
  
private:
  std::string _date;
  std::string _imageFilename;
  Size _imageSize;
};


}
}
}

#endif // __MINERVA_LAYERS_GEO_RSS_CLICKED_CALLBACK_H__
