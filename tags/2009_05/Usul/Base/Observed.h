
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
#include "Usul/Pointers/WeakPointer.h"

#include <set>


namespace Usul {
namespace Base {


class USUL_EXPORT Observed : public Usul::Base::Object
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Pointers::WeakPointer < Observed > WeakPtr;
  typedef Usul::Pointers::DeleteListener DeleteListener;
  typedef std::set < DeleteListener * > DeleteListeners;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Observed );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Observed );

  // Add the listener.
  virtual void            addDeleteListener ( DeleteListener * );

  // Remove the listener.
  virtual void            removeDeleteListener ( DeleteListener * );

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
