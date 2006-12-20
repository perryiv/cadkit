
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"

namespace CadKit
{
  namespace OSG
  {
    namespace Glue
    {
      public ref class ReadFile
      {
        delegate void CallbackDelegate ( unsigned long bytes );
        typedef void (*NativeProgressCallback) ( unsigned long bytes );
        typedef CadKit::Interfaces::Filters Filters;

        CadKit::Interfaces::IProgressBar^ _progress;
        CallbackDelegate^ _delegate;
        System::Runtime::InteropServices::GCHandle _pin;

        ~ReadFile();

        NativeProgressCallback    _makeProgressCallback();

        void                      _progressNotify ( unsigned long bytes );

      public:

        ReadFile();

        void                      clear();

        static Filters^           filters();
        static bool               hasReader ( System::String^ file );

        CadKit::OSG::Glue::Node^  readNodeFile ( System::String^ file, System::Object^ caller );
      };
    }
  }
}
