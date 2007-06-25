
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Transform pair.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_TRANFORM_PAIR_CALLBACK_H_
#define _CV_TRANFORM_PAIR_CALLBACK_H_

#include "CadViewer/Functors/Transform.h"


namespace CV {
namespace Functors {


class TransformPair : public VRV::Functors::BaseFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( TransformPair );
  typedef VRV::Functors::BaseFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;

  TransformPair ( 
    Unknown *unknown, 
    Transform *c1, 
    Transform *c2,
    unsigned int id );
  TransformPair ( const TransformPair &tp );

  // Have the transforms do their thing.
  virtual void          operator()();

protected:

  virtual ~TransformPair();

  Transform::ValidRefPtr _c1;
  Transform::ValidRefPtr _c2;
};


}; // namespace Functors
}; // namespace CV


#endif // _CV_TRANFORM_PAIR_CALLBACK_H_
