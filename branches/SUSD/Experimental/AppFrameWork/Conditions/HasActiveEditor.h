
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Returns true if there is an active editor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_HAS_ACTIVE_EDITOR_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_HAS_ACTIVE_EDITOR_CONDITION_CLASS_H_

#include "AppFrameWork/Conditions/Condition.h"


namespace AFW {
namespace Conditions {


class APP_FRAME_WORK_EXPORT HasActiveEditor : public Condition
{
public:

  // Typedefs.
  typedef Condition BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( HasActiveEditor );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( HasActiveEditor );

  // Constructor
  HasActiveEditor ( bool want = true );

  // Returns true if there is an active editor.
  virtual bool                    evaluate ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~HasActiveEditor();

private:

  // No copying.
  HasActiveEditor ( const HasActiveEditor & );
  HasActiveEditor &operator = ( const HasActiveEditor & );
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_HAS_ACTIVE_EDITOR_CONDITION_CLASS_H_
