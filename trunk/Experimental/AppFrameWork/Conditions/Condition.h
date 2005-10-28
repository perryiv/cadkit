
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all conditions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_BASE_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_BASE_CONDITION_CLASS_H_

#include "AppFrameWork/Core/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"


namespace AFW {
namespace Conditions {


class APP_FRAME_WORK_EXPORT Condition : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Condition );

  // See if the condition is met.
  virtual bool                    evaluate() = 0;

protected:

  // Constructor
  Condition();

  // Use reference counting.
  virtual ~Condition();

private:

  // No copying.
  Condition ( const Condition & );
  Condition &operator = ( const Condition & );
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_BASE_CONDITION_CLASS_H_
