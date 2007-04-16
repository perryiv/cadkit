
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace Magrathea { class WmsLayer; }

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
        WmsLayer( Magrathea::WmsLayer * );
        ~WmsLayer();
        !WmsLayer();

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
        Magrathea::WmsLayer *_wmsImageLayer;
      };
    }
  }
}
