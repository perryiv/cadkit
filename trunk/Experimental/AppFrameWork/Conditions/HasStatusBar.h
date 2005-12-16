
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Returns true if there is a status bar.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_HAS_STATUS_BAR_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_HAS_STATUS_BAR_CONDITION_CLASS_H_

#include "AppFrameWork/Conditions/Condition.h"


namespace AFW {
namespace Conditions {


class APP_FRAME_WORK_EXPORT HasStatusBar : public Condition
{
public:

  // Typedefs.
  typedef Condition BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( HasStatusBar );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( HasStatusBar );

  // Constructor
  HasStatusBar ( bool want = true );

  // Returns true if there is a status bar.
  virtual bool                    evaluate ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~HasStatusBar();

private:

  // No copying.
  HasStatusBar ( const HasStatusBar & );
  HasStatusBar &operator = ( const HasStatusBar & );
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_HAS_STATUS_BAR_CONDITION_CLASS_H_
