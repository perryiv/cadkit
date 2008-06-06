
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for outputing feedback to std::cout.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_CONSOLE_FEEDBACK_H__
#define __USUL_CONSOLE_FEEDBACK_H__

#include "Usul/Export/Export.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"


namespace Usul {
namespace Console {

class USUL_EXPORT Feedback : public Usul::Base::Referenced,
							               public Usul::Interfaces::IStatusBar,
							               public Usul::Interfaces::IProgressBar
{
public:

	// Typedef(s).
	typedef Usul::Base::Referenced BaseClass;

	USUL_DECLARE_QUERY_POINTERS ( Feedback );
	USUL_DECLARE_IUNKNOWN_MEMBERS;
	
	Feedback();
	
protected:
	virtual ~Feedback();

	// Show the progress bar
  virtual void showProgressBar();

  // Set the total of progress bar
  virtual void setTotalProgressBar ( unsigned int value );

  // Update the progress bar
  virtual void updateProgressBar ( unsigned int value );

  // Hide the progress bar
  virtual void hideProgressBar();
  
  // Set the status bar text.
  virtual void setStatusBarText ( const std::string &text, bool force );
  
private:
	unsigned int _total;
};

} // namespace Console
} // namespace Usul

#endif // __USUL_CONSOLE_FEEDBACK_H__
