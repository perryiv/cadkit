
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Update according to the condition.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_NEEDS_NEW_MODEL_PLUGIN_UPDATE_ACTION_CLASS_H_
#define _APP_FRAME_WORK_NEEDS_NEW_MODEL_PLUGIN_UPDATE_ACTION_CLASS_H_

#include "AppFrameWork/Actions/UpdateAction.h"

#include "Usul/Pointers/Pointers.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT NeedsNewModelPlugin : public UpdateAction
{
public:

  // Typedefs.
  typedef UpdateAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( NeedsNewModelPlugin );

  // Constructor
  NeedsNewModelPlugin();

  // Get the proper state.
  virtual bool                        state();

protected:

  // Use reference counting.
  virtual ~NeedsNewModelPlugin();

private:

  // No copying.
  NeedsNewModelPlugin ( const NeedsNewModelPlugin & );
  NeedsNewModelPlugin &operator = ( const NeedsNewModelPlugin & );
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_NEEDS_NEW_MODEL_PLUGIN_UPDATE_ACTION_CLASS_H_
