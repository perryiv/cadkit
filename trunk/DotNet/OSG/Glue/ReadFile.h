
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
        delegate void CallbackDelegate();
        typedef void (*NativeProgressCallback)();

        CadKit::Interfaces::IProgressBar^ _progress;
        CallbackDelegate^ _delegate;
        System::Runtime::InteropServices::GCHandle _pin;

        ~ReadFile();

        NativeProgressCallback    _makeProgressCallback();

        void                      _progressNotify();

      public:

        ReadFile();

        void                      clear();

        CadKit::OSG::Glue::Node^  readNodeFile ( System::String^ filename, System::Object^ caller );
      };
    }
  }
}
