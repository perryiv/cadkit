
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class that maintains a set of delete-listeners.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_BASE_OBSERVED_CLASS_H_
#define _USUL_BASE_OBSERVED_CLASS_H_

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IDeleteNotify.h"
#include "Usul/Interfaces/IDeleteListener.h"
#include "Usul/Pointers/WeakPointer.h"

#include <set>


namespace Usul {
namespace Base {


class USUL_EXPORT Observed : public Usul::Base::Object,
                             public Usul::Interfaces::IDeleteNotify
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Pointers::WeakPointer < Observed > WeakPtr;
  typedef Usul::Interfaces::IDeleteListener IDeleteListener;
  typedef std::set < IDeleteListener::RefPtr > DeleteListeners;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Observed );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Observed );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Add the listener (IDeleteListener).
  virtual void            addDeleteListener ( Usul::Interfaces::IUnknown * );

  // Remove the listener (IDeleteListener).
  virtual void            removeDeleteListener ( Usul::Interfaces::IUnknown * );

protected:

  // Construction.
  Observed();

  // Copy construction.
  Observed ( const Observed & );

  // No assignment.
  Observed &operator = ( const Observed & );

  // Use reference counting.
  virtual ~Observed();

private:

  void                      _destroy();

  void                      _notifyDeleteListeners();

  DeleteListeners _deleteListeners;
};


} // namespace Base
} // namespace Usul


#endif // _USUL_BASE_OBSERVED_CLASS_H_
