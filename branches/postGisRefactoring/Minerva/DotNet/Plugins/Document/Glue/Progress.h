
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "UnmanagedProgress.h"


namespace DT
{
  namespace Minerva
  {
    namespace Plugins
    {
      namespace Document
      {
        namespace Glue 
        {
          public delegate void CallbackDelegate(int value);

          public ref class Progress
          {
          public:
            Progress(CadKit::Threads::Jobs::Progress ^progress);
            ~Progress();

            void update( int i );
            void total( int i );

            UnmanagedProgress* unmanagedProgress();

          private:
            CallbackDelegate ^_updateDelegate;
            CallbackDelegate ^_totalDelegate;

            System::Runtime::InteropServices::GCHandle _updateDelegateHandle;
            System::Runtime::InteropServices::GCHandle _totalDelegateHandle;

            UnmanagedProgress *_unmanagedProgress;
            CadKit::Threads::Jobs::Progress ^_progress;
          };

        }
      }
    }
  }
}
