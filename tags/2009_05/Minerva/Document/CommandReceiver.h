
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_COMMAND_RECEIVER_H__
#define __MINERVA_COMMAND_RECEIVER_H__

#include "Minerva/DataSources/Connection.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IUnknown.h"

#include <string>
#include <map>

namespace Minerva {
  namespace Document {

class CommandReceiver : public Usul::Base::Referenced
{
public:
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( CommandReceiver );

  CommandReceiver();

  // Set the application connection.
  void connection ( Minerva::DataSources::Connection * );

  /// Are we connected to the session?
  bool connected () const;

  // Connect to a session.
  void connectToSession( const std::string& name );

  /// Process any new commands.
  void processCommands ( Usul::Interfaces::IUnknown *caller );  

protected:
  /// Use reference counting
  virtual ~CommandReceiver();

  void _processCommands( Usul::Interfaces::IUnknown *caller );

private:
  Minerva::DataSources::Connection::RefPtr _connection;
  unsigned int _sessionID;
  unsigned int _lastCommandID;
  unsigned int _timeout;
  bool _connected;
};


  }
}


#endif // __MINERVA_COMMAND_RECEIVER_H__

