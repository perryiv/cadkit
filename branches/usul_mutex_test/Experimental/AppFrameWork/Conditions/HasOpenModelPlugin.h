
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

#ifndef _APP_FRAME_WORK_HAS_OPEN_MODEL_PLUGIN_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_HAS_OPEN_MODEL_PLUGIN_CONDITION_CLASS_H_

#include "AppFrameWork/Conditions/Condition.h"


namespace AFW {
namespace Conditions {


class APP_FRAME_WORK_EXPORT HasOpenModelPlugin : public Condition
{
public:

  // Typedefs.
  typedef Condition BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( HasOpenModelPlugin );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( HasOpenModelPlugin );

  // Constructor
  HasOpenModelPlugin ( bool want = true );

  // Returns true if there is an appropriate plugin.
  virtual bool                    evaluate ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~HasOpenModelPlugin();

private:

  // No copying.
  HasOpenModelPlugin ( const HasOpenModelPlugin & );
  HasOpenModelPlugin &operator = ( const HasOpenModelPlugin & );
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_HAS_OPEN_MODEL_PLUGIN_CONDITION_CLASS_H_
