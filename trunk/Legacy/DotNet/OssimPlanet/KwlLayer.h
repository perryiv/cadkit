
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace Magrathea { class KwlLayer; }

namespace CadKit
{
  namespace OSSIMPlanet
  {
    namespace Glue
    {
      public ref class KwlLayer :  
        CadKit::Referenced::Base,
        CadKit::Interfaces::IOssimLayer,
        CadKit::Interfaces::ILayer,
        CadKit::Interfaces::INativePtr
      {
      public:
        KwlLayer( System::String^ );
        KwlLayer( Magrathea::KwlLayer* );
        ~KwlLayer();
        !KwlLayer();

        enum class TextureLayerStateCode
        {
           VALID          = 0,
           NO_SOURCE_DATA = 1,
           NO_GEOM        = 2,
           NO_OVERVIEWS   = 4,
           ALL = ( NO_SOURCE_DATA|
                   NO_GEOM|
                   NO_OVERVIEWS )
        };

        property TextureLayerStateCode StateCode
        {
          TextureLayerStateCode get();
        }

        property System::String^ Guid
        {
          virtual System::String^ get();
        }

        property System::String^ Name
        {
          virtual System::String^ get();
        }
        
        property bool Shown
        {
          virtual bool get();
        }

        virtual System::IntPtr           nativeIntPtr();
      private:
        Magrathea::KwlLayer* _kwlLayer;
      };
    }
  }
}
