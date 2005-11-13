
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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
