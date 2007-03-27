
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  All contained conditions have to be true.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_AND_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_AND_CONDITION_CLASS_H_

#include "AppFrameWork/Conditions/Group.h"


namespace AFW {
namespace Conditions {


class APP_FRAME_WORK_EXPORT And : public Group
{
public:

  // Typedefs.
  typedef Group BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( And );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( And );

  // Constructor
  And();

  // All contained conditions have to be true.
  virtual bool                    evaluate ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~And();

private:

  // No copying.
  And ( const And & );
  And &operator = ( const And & );
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_AND_CONDITION_CLASS_H_
