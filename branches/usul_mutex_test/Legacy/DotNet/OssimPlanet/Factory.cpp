
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Factory.h"
#include "ImageLayer.h"
#include "KwlLayer.h"
#include "WmsLayer.h"

#include "Magrathea/ImageTextureLayer.h"
#include "Magrathea/KwlLayer.h"
#include "Magrathea/WmsLayer.h"

using namespace CadKit::OSSIMPlanet::Glue;


CadKit::Interfaces::ILayer^ Factory::create( System::IntPtr pointer )
{
  Usul::Interfaces::IUnknown::QueryPtr unknown ( reinterpret_cast < Usul::Interfaces::IUnknown * > ( pointer.ToPointer() ) );

  if( unknown.valid() )
  {
    if( Magrathea::ImageTextureLayer* image = dynamic_cast < Magrathea::ImageTextureLayer*> ( unknown.get() ) )
      return gcnew ImageLayer ( image );
    if( Magrathea::KwlLayer* kwl = dynamic_cast < Magrathea::KwlLayer* > ( unknown.get() ) )
      return gcnew KwlLayer( kwl );
    if( Magrathea::WmsLayer* wms = dynamic_cast < Magrathea::WmsLayer* > ( unknown.get() ) )
      return gcnew WmsLayer( wms );
  }

  return nullptr;
}
