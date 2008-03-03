
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_GDAL_LAYER_H__
#define __MINERVA_GDAL_LAYER_H__

#include "Minerva/Core/Layers/RasterLayer.h"

#include "Usul/Interfaces/IRead.h"

class GDALDataset;


class GdalLayer : public Minerva::Core::Layers::RasterLayer,
                  public Usul::Interfaces::IRead
{
public:
  typedef Minerva::Core::Layers::RasterLayer BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  
  USUL_DECLARE_QUERY_POINTERS ( GdalLayer );
  
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  GdalLayer ();
  
  /// Clone.
  virtual IUnknown*     clone() const;
  
  /// Read.
  virtual void          read ( const std::string& filename, Usul::Interfaces::IUnknown* caller = 0x0, Usul::Interfaces::IUnknown* progrss = 0x0 );
  
  /// Get the texture.
  virtual ImagePtr      texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );
  
  /// Deserialize.
  virtual void          deserialize ( const XmlTree::Node &node );
  
protected:
  
  virtual ~GdalLayer();
  
  GdalLayer ( const GdalLayer& );
    
private:
  
  // No assignment.
  GdalLayer& operator= ( const GdalLayer& );
  
  GDALDataset *_data;
  std::string _filename;
  
  SERIALIZE_XML_CLASS_NAME( GdalLayer ) 
  SERIALIZE_XML_SERIALIZE_FUNCTION 
  SERIALIZE_XML_ADD_MEMBER_FUNCTION
};


#endif // __MINERVA_GDAL_LAYER_H__
