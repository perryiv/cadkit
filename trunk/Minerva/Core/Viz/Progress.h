
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_VIZ_PROGRESS_H__
#define __MINERVA_CORE_VIZ_PROGRESS_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"


namespace Minerva
{
  namespace Core
  {
    namespace Viz
    {
      class Progress : public Usul::Base::Referenced,
	               public Usul::Interfaces::IProgressBar
      {
      public:
	typedef Usul::Base::Referenced BaseClass;
	typedef Usul::Interfaces::IUnknown IUnknown;

	USUL_DECLARE_REF_POINTERS ( Progress );

	USUL_DECLARE_IUNKNOWN_MEMBERS;

	Progress();

	// Show the progress bar
	virtual void showProgressBar();
	
	// Set the total of progress bar
	virtual void setTotalProgressBar ( unsigned int value );

	// Update the progress bar
	virtual void updateProgressBar ( unsigned int value );

	// Hide the progress bar
	virtual void hideProgressBar();

      protected:
	virtual ~Progress();
      };
    }
  }
}


#endif /// __MINERVA_CORE_VIZ_PROGRESS_H__
