
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Status bar class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_STATUS_BAR_CLASS_H_
#define _APP_FRAME_WORK_STATUS_BAR_CLASS_H_

#include "AppFrameWork/Windows/Group.h"


namespace AFW {
namespace Windows {


class APP_FRAME_WORK_EXPORT StatusBar : public Group
{
public:

  // Typedefs.
  typedef Group BaseClass;

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

protected:

  // Constructor
  StatusBar();

  // Use reference counting.
  virtual ~StatusBar();

private:

  // No copying.
  StatusBar ( const StatusBar & );
  StatusBar &operator = ( const StatusBar & );

  AFW_DECLARE_OBJECT ( StatusBar );
};


} // namespace Windows
} // namespace AFW


#endif //_APP_FRAME_WORK_STATUS_BAR_CLASS_H_
