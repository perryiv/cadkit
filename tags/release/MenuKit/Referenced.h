
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all other classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_REFERENCED_H_
#define _MENUKIT_REFERENCED_H_

#include "MenuKit/Export.h"

#include <boost/intrusive_ptr.hpp>


namespace MenuKit {


class MENUKIT_EXPORT Referenced
{
public:

  // Reference and unreference this instance.
  void ref();                                               // reference this instance, increment _refcount
  void unref();                                             // unreference this instance, decrement _refcount ...
                                                            // ... delete this if _refcount = 0

protected:

  // Construction and destruction.
  Referenced();                                             // constructor, init _refcount = 0
  Referenced ( const Referenced & );                        // copy constructor, init _refcount = 0
  virtual ~Referenced();                                    // destructor, empty in .cpp

  // Assignment.
  Referenced& operator = ( const Referenced & );            // {return *this} in .cpp

private:

  unsigned int _refCount;
};


}; // namespace MenuKit


namespace boost
{
  MENUKIT_EXPORT void intrusive_ptr_add_ref ( MenuKit::Referenced * );
  MENUKIT_EXPORT void intrusive_ptr_release ( MenuKit::Referenced * );
};


#define MENUKIT_DECLARE_POINTER(classname) \
  typedef boost::intrusive_ptr<classname> Ptr


#endif // _MENUKIT_REFERENCED_H_
