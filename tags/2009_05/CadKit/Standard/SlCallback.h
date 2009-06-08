
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  SlCallback.h: A generic callback class.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_CALLBACK_H_
#define _CADKIT_STANDARD_LIBRARY_CALLBACK_H_

#include "SlTuple.h"


namespace CadKit
{
template<class FT, class DT, class CT> class SlCallback : public SlTriplet<FT,DT,CT>
{
public:

  SlCallback() : SlTriplet<FT,DT,CT>(){}
  SlCallback ( const FT &function, const DT &clientData, const CT &callData ) : SlTriplet<FT,DT,CT> ( function, clientData, callData ){}

  // Get/set the function.
  const FT &      getFunction() const { return this->getFirst(); }
  void            setFunction ( const FT &function ) { this->setFirst ( function ); }

  // Get/set the client data.
  const DT &      getClientData() const { return this->getSecond(); }
  void            setClientData ( const DT &clientData ) { this->setSecond ( clientData ); }

  // Get/set the call data.
  const CT &      getCallData() const { return this->getThird(); }
  void            setCallData ( const CT &callData ) { this->setThird ( callData ); }
};


}; // namespace CadKit

#endif
