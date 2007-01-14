
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

class ossimPlanetTextureLayer;

namespace CadKit
{
  namespace OSSIMPlanet
  {
    namespace Glue
    {
      public ref class ImageLayer : CadKit::Interfaces::ILayer
      {
      public:
        ImageLayer( System::String^ );
        ~ImageLayer();
        !ImageLayer();

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

        property System::String^ Name
        {
          virtual System::String^ get();
          virtual void set ( System::String^ s );
        }
        
        property bool Shown
        {
          virtual bool get();
        }

        property bool NeedsUpdate 
        { 
          virtual bool get(); 
          virtual void set( bool b ); 
        }

        virtual void show();
        virtual void modify();
        virtual void hide();

        ossimPlanetTextureLayer* nativePtr();
        System::IntPtr           intPtr();
      private:
        ossimPlanetTextureLayer* _layer;

        System::String^ _name;
      };
    }
  }
}
