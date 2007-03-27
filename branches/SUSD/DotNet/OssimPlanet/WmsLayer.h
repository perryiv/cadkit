
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

class ossimPlanetWmsImageLayer;

namespace CadKit
{
  namespace OSSIMPlanet
  {
    namespace Glue
    {
      public ref class WmsLayer : 
        CadKit::Referenced::Base,
        CadKit::Interfaces::IOssimLayer,
        CadKit::Interfaces::ILayer,
        CadKit::Interfaces::INativePtr
      {
      public:
        WmsLayer( );
        ~WmsLayer();
        !WmsLayer();

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
        virtual void addToFavorites();

        virtual System::IntPtr           nativeIntPtr();

        property System::String^ Server
        {
          System::String^ get();
          void set ( System::String^ s );
        }

        property System::String^ CacheDirectory
        {
          System::String^ get();
          void set ( System::String^ s );
        }

        property System::String^ ImageType
        {
          System::String^ get();
          void set ( System::String^ s );
        }

      private:
        ossimPlanetWmsImageLayer *_wmsImageLayer;
      };
    }
  }
}
