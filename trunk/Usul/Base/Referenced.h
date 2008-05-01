
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Reference-counting base class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_REFERENCED_BASE_CLASS_H_
#define _USUL_REFERENCED_BASE_CLASS_H_

#if 0
#include "vld.h"
#endif

#include "Usul/Base/Typed.h"

#include "Usul/Pointers/Intrusive.h"

namespace Usul { namespace Threads { class Mutex; } }
namespace Usul { namespace Interfaces { struct IUnknown; } }

namespace Usul {
namespace Base {


class USUL_EXPORT Referenced : public Typed
{
public:

  typedef Typed BaseClass;
  USUL_DECLARE_TYPE_ID ( Referenced );

	/// Get this referenced as an IUnknown.  May return null.
	virtual Usul::Interfaces::IUnknown*   asUnknown() const;

  /// Reference the instance.
  void                        ref();

  /// Unreference the instance.
  void                        unref ( bool allowDeletion = true );

  /// Get the reference count.
  unsigned long               refCount() const;

protected:

  explicit Referenced();
  Referenced ( const Referenced &r );
  Referenced &operator = ( const Referenced &r );
  virtual ~Referenced();

private:

  unsigned long _refCount;
  Usul::Threads::Mutex *_rcMutex;
};


} // namespace Base
} // namespace Usul


#endif // _USUL_REFERENCED_BASE_CLASS_H_
