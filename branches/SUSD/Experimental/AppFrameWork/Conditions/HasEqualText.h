
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Returns true if the text is equal.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_HAS_EQUAL_TEXT_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_HAS_EQUAL_TEXT_CONDITION_CLASS_H_

#include "AppFrameWork/Conditions/Condition.h"

#include <string>


namespace AFW {
namespace Conditions {


class APP_FRAME_WORK_EXPORT HasEqualText : public Condition
{
public:

  // Typedefs.
  typedef Condition BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( HasEqualText );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( HasEqualText );

  // Constructor
  HasEqualText ( const std::string &text, bool want = true );

  // Returns true if there is an active editor.
  virtual bool                    evaluate ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~HasEqualText();

private:

  // No copying.
  HasEqualText ( const HasEqualText & );
  HasEqualText &operator = ( const HasEqualText & );

  std::string _text;
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_HAS_EQUAL_TEXT_CONDITION_CLASS_H_
