

///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_MODE_STACK_H__
#define __USUL_INTERFACES_MODE_STACK_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {
    
template < class ModeType, unsigned int iid >
struct IModeStack : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IModeStack );

  enum { IID = iid };

  virtual void pushMode( ModeType ) = 0;
  virtual void popMode() = 0;

  /// Small struct to push/pop the stack.
  struct PushPop
  {
    PushPop ( IModeStack *s, const ModeType &m ) : _s ( s )
    {
      if ( _s.valid() )
        _s->pushMode ( m );
    }
    ~PushPop()
    {
      if ( _s.valid() )
        _s->popMode();
    }
  protected:
    typename IModeStack::RefPtr _s;
  };
}; //class IModeStack

typedef IModeStack< unsigned int, 1100797533u > IModeStackUint;

}; //namespace Interfaces
}; //namespace Usul

#endif // __USUL_INTERFACES_MODE_STACK_H__

