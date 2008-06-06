
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all window-creating functors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_NEW_WINDOW_FUNCTOR_CLASS_H_
#define _APP_FRAME_WORK_NEW_WINDOW_FUNCTOR_CLASS_H_

#include "AppFrameWork/Core/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

namespace AFW { namespace Core { class Object; } }


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT NewObjectFunctor : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( NewObjectFunctor );

  // Type information.
  USUL_DECLARE_TYPE_ID ( NewObjectFunctor );

  // Create a new window.
  virtual AFW::Core::Object *operator()() = 0;

protected:

  // Constructor
  NewObjectFunctor();

  // Use reference counting.
  virtual ~NewObjectFunctor();

private:

  // No copying.
  NewObjectFunctor ( const NewObjectFunctor & );
  NewObjectFunctor &operator = ( const NewObjectFunctor & );
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_NEW_WINDOW_FUNCTOR_CLASS_H_
