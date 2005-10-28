
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  One of the contained conditions has to be true.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_OR_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_OR_CONDITION_CLASS_H_

#include "AppFrameWork/Conditions/Group.h"


namespace AFW {
namespace Conditions {


class APP_FRAME_WORK_EXPORT Or : public Group
{
public:

  // Typedefs.
  typedef Group BaseClass;
  typedef std::vector < Condition::ValidRefPtr > Components;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Or );

  // Constructor
  Or();

  // One of the contained conditions has to be true.
  virtual bool                    evaluate();

protected:

  // Use reference counting.
  virtual ~Or();

private:

  // No copying.
  Or ( const Or & );
  Or &operator = ( const Or & );
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_OR_CONDITION_CLASS_H_
