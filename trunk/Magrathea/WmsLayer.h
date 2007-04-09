
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

#include "ossimPlanet/ossimPlanetWmsImageLayer.h"

namespace Magrathea {

class MAGRATHEA_EXPORT WmsLayer : public Usul::Base::Referenced,
                                  public Usul::Interfaces::ILayer
{
public:
  typedef Usul::Base::Referenced BaseClass;
  WmsLayer();

  USUL_DECLARE_REF_POINTERS ( WmsLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  void                    server( const std::string& serverString );
  const std::string&      server() const;

  void                    imageType( const std::string& imageType );
  const std::string&      imageType() const;
  
  void                    cacheDirectory( const std::string& directory );
  const std::string&      cacheDirectory() const;

  void                    transparentFlag( bool flag );
  bool                    transparentFlag() const;

  void                    transparentColorFlag( bool flag );
  bool                    transparentColorFlag() const;
  void                    transparentColor( unsigned int r, unsigned int g, unsigned int b );

protected:
  virtual ~WmsLayer();

private:
  osg::ref_ptr < ossimPlanetWmsImageLayer > _wmsImageLayer;
};

}

#endif // __MAGRATHEA_WMS_LAYER_H__
