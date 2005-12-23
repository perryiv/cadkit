
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Displays text and icons.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_LOG_WINDOW_CLASS_H_
#define _APP_FRAME_WORK_LOG_WINDOW_CLASS_H_

#include "AppFrameWork/Core/Window.h"


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT LogWindow : public Window
{
public:

  // Typedefs.
  typedef Window BaseClass;

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

protected:

  // Constructor
  LogWindow();

  // Use reference counting.
  virtual ~LogWindow();

private:

  // No copying.
  LogWindow ( const LogWindow & );
  LogWindow &operator = ( const LogWindow & );

  AFW_DECLARE_OBJECT ( LogWindow );
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_LOG_WINDOW_CLASS_H_
