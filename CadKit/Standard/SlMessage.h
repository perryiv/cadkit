
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
//  SlMessage.h: A generic message class.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_MESSAGE_H_
#define _CADKIT_STANDARD_LIBRARY_MESSAGE_H_

#define SL_MCA class IdT, class StringT, class ClientDataT, class CallDataT
#define SL_MFA IdT, StringT, ClientDataT, CallDataT


namespace CadKit
{
template<SL_MCA> class SlMessage
{
public:

  typedef IdT         IdType;
  typedef StringT     StringType;
  typedef ClientDataT ClientDataType;
  typedef CallDataT   CallDataType;

  SlMessage();
  SlMessage ( const SlMessage<SL_MFA> &message );
  SlMessage ( const IdT &id, 
              const StringT &text, 
              ClientDataT clientData, 
              CallDataT callData );

  const IdT &               getId() const { return _id; }
  IdT &                     getId()       { return _id; }

  const StringT &           getText() const { return _text; }
  StringT &                 getText()       { return _text; }

  const ClientDataT &       getClientData() const { return _clientData; }
  ClientDataT &             getClientData()       { return _clientData; }

  const CallDataT &         getCallData() const { return _callData; }
  CallDataT &               getCallData()       { return _callData; }

  SlMessage<SL_MFA> &       operator = ( const SlMessage<SL_MFA> &message );

  void                      setValue ( const SlMessage<SL_MFA> &message );
  void                      setValue ( const IdT &id, 
                                       const StringT &text, 
                                       const ClientDataT &clientData, 
                                       const CallDataT &callData );

  void                      setId ( const IdT &id ) { _id = id; }
  void                      setText ( const StringT &text ) { _text = text; }
  void                      setClientData ( const ClientDataT &clientData ) { _clientData = clientData; }
  void                      setCallData ( const CallDataT &callData ) { _callData = callData; }

protected:

  IdT         _id;         // The message id.
  StringT     _text;       // The message text.
  ClientDataT _clientData; // The client's data.
  CallDataT   _callData;   // Generic data.
};



//////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
//////////////////////////////////////////////////////////////////////////

template<SL_MCA> inline SlMessage<SL_MFA>::SlMessage() :
  _id ( 0 ),
  _text ( "" ), // Don't pass null.
  _clientData ( 0x0 ),
  _callData ( 0x0 )
{
  // Empty.
}


//////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
//////////////////////////////////////////////////////////////////////////

template<SL_MCA> inline SlMessage<SL_MFA>::SlMessage ( 
  const IdT &id, 
  const StringT &text, 
  ClientDataT clientData,
  CallDataT callData )
{
  this->setValue ( message._id, message._text, message._clientData, message._callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
//////////////////////////////////////////////////////////////////////////

template<SL_MCA> inline SlMessage<SL_MFA>::SlMessage ( const SlMessage<SL_MFA> &message )
{
  this->setValue ( message );
}


//////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
//////////////////////////////////////////////////////////////////////////

template<SL_MCA> inline void SlMessage<SL_MFA>::setValue ( 
  const IdT &id, 
  const StringT &text, 
  const ClientDataT &clientData, 
  const CallDataT &callData )
{
  _id = id;
  _text = text;
  _clientData = clientData;
  _callData = callData;
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
//////////////////////////////////////////////////////////////////////////

template<SL_MCA> inline void SlMessage<SL_MFA>::setValue ( const SlMessage<SL_MFA> &message )
{
  this->setValue ( message._id, message._text, message._clientData, message._callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
//////////////////////////////////////////////////////////////////////////

template<SL_MCA> inline SlMessage<SL_MFA> &SlMessage<SL_MFA>::operator = ( const SlMessage<SL_MFA> &message )
{
  this->setValue ( message );
  return *this;
}


}; // namespace CadKit

#endif
