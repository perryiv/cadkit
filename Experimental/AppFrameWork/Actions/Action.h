
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all actions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_ACTION_CLASS_H_
#define _APP_FRAME_WORK_ACTION_CLASS_H_

#include "AppFrameWork/Core/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT Action : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Action );

  // Can we undo the action?
  virtual bool                        canUndo() const;

  // Perform the action.
  virtual void                        execute() = 0;

  // Undo the command.
  virtual void                        undo();

protected:

  // Constructor
  Action();

  // Use reference counting.
  virtual ~Action();

private:

  // No copying.
  Action ( const Action & );
  Action &operator = ( const Action & );
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_ACTION_CLASS_H_
