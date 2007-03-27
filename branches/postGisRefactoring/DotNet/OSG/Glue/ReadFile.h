
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
        delegate void CallbackDelegate ( const std::string &, unsigned long, unsigned long );
        typedef void (*NativeProgressCallback) ( const std::string &, unsigned long, unsigned long );
        typedef CadKit::Interfaces::Filters Filters;

        CadKit::Interfaces::IProgressBar^ _progress;
        CallbackDelegate^ _delegate;
        System::Runtime::InteropServices::GCHandle _pin;
        System::DateTime _startTime;

        ~ReadFile();

        NativeProgressCallback    _makeProgressCallback();

        void                      _progressNotify ( const std::string &file, unsigned long bytes, unsigned long total );

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
