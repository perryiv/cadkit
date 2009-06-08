
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for listeners.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DYNAMIC_LOAD_LIBRARY_LISTENER_CLASS_H_
#define _USUL_DYNAMIC_LOAD_LIBRARY_LISTENER_CLASS_H_

#include "Usul/Base/Referenced.h"

#include "Usul/Pointers/Pointers.h"

#include <string>


namespace Usul {
namespace DLL {


///////////////////////////////////////////////////////////////////////////////
//
//  Base class for listeners.
//
///////////////////////////////////////////////////////////////////////////////

class USUL_EXPORT Listener : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart pointers.
  USUL_DECLARE_REF_POINTERS ( Listener );

  // Call when there is plugin or dynamic library activity.
  virtual void operator () ( const std::string &name ) = 0;

protected:

  Listener();
  virtual ~Listener();

private:

  Listener ( const Listener & );
  Listener &operator = ( const Listener & );
};


} // namespace DLL
} // namespace Usul


#endif // _USUL_DYNAMIC_LOAD_LIBRARY_LISTENER_CLASS_H_
