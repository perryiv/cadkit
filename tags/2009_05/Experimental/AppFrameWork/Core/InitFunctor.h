
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor called during program initialization.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_INITIALIZATION_FUNCTOR_CLASS_H_
#define _APP_FRAME_WORK_INITIALIZATION_FUNCTOR_CLASS_H_

#include "AppFrameWork/Core/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

namespace AFW { namespace Core { class Application; } }


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT InitFunctor : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( InitFunctor );

  // Operator.
  virtual void                        operator () ( Application * );

protected:

  // Constructor, destructor.
  InitFunctor();
  virtual ~InitFunctor();

private:

  // No copying.
  InitFunctor ( const InitFunctor & );
  InitFunctor &operator = ( const InitFunctor & );
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_INITIALIZATION_FUNCTOR_CLASS_H_
