
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"

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
          typedef void (*managedCallback) (int num);

          class UnmanagedProgress : public Usul::Base::Referenced,
                                    public Usul::Interfaces::IProgressBar
          {
          public:
            typedef Usul::Base::Referenced BaseClass;

            // Smart-pointer definitions.
            USUL_DECLARE_QUERY_POINTERS ( UnmanagedProgress );

            // Usul::Interfaces::IUnknown members.
            USUL_DECLARE_IUNKNOWN_MEMBERS;

            UnmanagedProgress();
            virtual ~UnmanagedProgress();

            // Show the progress bar
            virtual void showProgressBar();

            // Set the total of progress bar
            virtual void setTotalProgressBar ( unsigned int value );

            // Update the progress bar
            virtual void updateProgressBar ( unsigned int value );

            // Hide the progress bar
            virtual void hideProgressBar();

            void setUpdateCallback(managedCallback callback);
            void setTotalCallback(managedCallback callback);

          private:

            managedCallback _updateCallback;
            managedCallback _totalCallback;
          };
        }
      }
    }
  }
}
