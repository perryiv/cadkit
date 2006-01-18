
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Returns true of the window is visible.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_IS_VISIBLE_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_IS_VISIBLE_CONDITION_CLASS_H_

#include "AppFrameWork/Conditions/Group.h"


namespace AFW {
namespace Conditions {


class APP_FRAME_WORK_EXPORT IsVisible : public Group
{
public:

  // Typedefs.
  typedef Group BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( IsVisible );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( IsVisible );

  // Constructor
  IsVisible ( AFW::Core::Object *, bool want = true );

  // Returns true of the window is visible.
  virtual bool                    evaluate ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~IsVisible();

private:

  // No copying.
  IsVisible ( const IsVisible & );
  IsVisible &operator = ( const IsVisible & );

  AFW::Core::Object::RefPtr _object;
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_IS_VISIBLE_CONDITION_CLASS_H_
