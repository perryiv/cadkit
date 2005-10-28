
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Returns true of the window is enabled.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_IS_ENABLED_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_IS_ENABLED_CONDITION_CLASS_H_

#include "AppFrameWork/Conditions/Group.h"


namespace AFW {
namespace Conditions {


class APP_FRAME_WORK_EXPORT IsEnabled : public Group
{
public:

  // Typedefs.
  typedef Group BaseClass;
  typedef std::vector < Condition::ValidRefPtr > Components;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( IsEnabled );

  // Constructor
  IsEnabled();

  // Returns true of the window is enabled.
  virtual bool                    evaluate();

protected:

  // Use reference counting.
  virtual ~IsEnabled();

private:

  // No copying.
  IsEnabled ( const IsEnabled & );
  IsEnabled &operator = ( const IsEnabled & );
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_IS_ENABLED_CONDITION_CLASS_H_
