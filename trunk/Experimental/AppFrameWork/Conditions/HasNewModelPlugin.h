
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Returns true if there is an appropriate plugin.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_HAS_NEW_MODEL_PLUGIN_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_HAS_NEW_MODEL_PLUGIN_CONDITION_CLASS_H_

#include "AppFrameWork/Conditions/Condition.h"


namespace AFW {
namespace Conditions {


class APP_FRAME_WORK_EXPORT HasNewModelPlugin : public Condition
{
public:

  // Typedefs.
  typedef Condition BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( HasNewModelPlugin );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( HasNewModelPlugin );

  // Constructor
  HasNewModelPlugin ( bool want = true );

  // Returns true if there is an appropriate plugin.
  virtual bool                    evaluate ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~HasNewModelPlugin();

private:

  // No copying.
  HasNewModelPlugin ( const HasNewModelPlugin & );
  HasNewModelPlugin &operator = ( const HasNewModelPlugin & );
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_HAS_NEW_MODEL_PLUGIN_CONDITION_CLASS_H_