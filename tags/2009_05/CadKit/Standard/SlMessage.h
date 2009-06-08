
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
  this->setValue ( id, text, clientData, callData );
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
