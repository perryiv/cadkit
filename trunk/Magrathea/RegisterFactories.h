
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MAGRATHEA_REGISTER_FACTORIES_H__
#define __MAGRATHEA_REGISTER_FACTORIES_H__

#include "ImageTextureLayer.h"
#include "KwlLayer.h"
#include "WmsLayer.h"

#include "Serialize/XML/TypeCreator.h"
#include "Serialize/XML/Factory.h"

namespace Magrathea
{
  inline void registerFactories()
  {
    //Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<WmsLayer> ( "WmsLayer" ) );
    //Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<KwlLayer> ( "KwlLayer" ) );
    //Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<ImageTextureLayer> ( "ImageTextureLayer" ) );
  }
}


#endif // __MAGRATHEA_REGISTER_FACTORIES_H__
