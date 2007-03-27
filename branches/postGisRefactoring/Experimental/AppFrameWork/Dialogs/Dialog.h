
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all dialogs.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_DIALOG_CLASS_H_
#define _APP_FRAME_WORK_DIALOG_CLASS_H_

#include "AppFrameWork/Windows/Window.h"


namespace AFW {
namespace Dialogs {


class APP_FRAME_WORK_EXPORT Dialog : public AFW::Windows::Window
{
public:

  // Typedefs.
  typedef AFW::Windows::Window BaseClass;

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

protected:

  // Constructor
  Dialog();

  // Use reference counting.
  virtual ~Dialog();

private:

  // No copying.
  Dialog ( const Dialog & );
  Dialog &operator = ( const Dialog & );

  AFW_DECLARE_OBJECT ( Dialog );
};


} // namespace Dialogs
} // namespace AFW


#endif //_APP_FRAME_WORK_DIALOG_CLASS_H_
