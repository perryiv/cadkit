
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for functors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_BASE_FUNCTOR_CLASS_H_
#define _CV_BASE_FUNCTOR_CLASS_H_

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"


namespace CV {
namespace Functors {


class BaseFunctor : public Usul::Base::Referenced
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( BaseFunctor );
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  // Return/set the id.
  unsigned int          id() const { return _id; }
  void                  id ( unsigned int n ) { _id = n; }

  // Exact functionality determined by inheriting class.
  virtual void          operator()() = 0;

protected:

  BaseFunctor ( Unknown *unknown, unsigned int id );
  BaseFunctor ( const BaseFunctor &cb );
  virtual ~BaseFunctor();

  // Return the unknown pointer.
  const Unknown *       _unknown() const;
  Unknown *             _unknown();

private:

  Unknown::ValidQueryPtr _iUnknown;
  unsigned int _id;
};


}; // namespace Functors
}; // namespace CV


#endif // _CV_BASE_FUNCTOR_CLASS_H_
