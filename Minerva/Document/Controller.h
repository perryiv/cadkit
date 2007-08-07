
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WNV_CONTROLLER_H__
#define __WNV_CONTROLLER_H__

#ifndef NOMINMAX
# define NOMINMAX
#endif

#include "Minerva/Core/Export.h"

#include "Minerva/Core/DB/Connection.h"
#include "Minerva/Core/Layers/Layer.h"
#include "Minerva/Core/Scene/SceneManager.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Policies/Update.h"

#include "osg/Array"

#include <string>
#include <map>

namespace Minerva {
  namespace Core {
    namespace Viz {

class MINERVA_EXPORT Controller : public Usul::Base::Referenced
{
public:
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Controller );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Controller( Usul::Interfaces::IUnknown *caller );

  // Set the application connection.
  void applicationConnection( Minerva::Core::DB::Connection * );

  // Connect to a session.
  void connectToSession( const std::string& name );

  // Update the scene.
  void updateScene( );  

  // Set the SceneManager.  TODO: This should probably be an interface.
  void sceneManager ( Minerva::Core::Scene::SceneManager * manager ) { _sceneManager = manager; }

protected:
  /// Use reference counting
  virtual ~Controller();

  void _processEvents();
  void _processCommands();

  void _processAddLayer    ( const std::string& tableName, unsigned int eventID );
  void _processRemoveLayer ( const std::string& tableName, unsigned int eventID );
  void _processPlayMovie   ( const std::string& tableName, unsigned int eventID );

  // Get the tablename for given event type.
  std::string _getTablename( int type );

  Minerva::Core::Layers::Layer* _getLayer ( const std::string drawCommandTable, unsigned int eventID );

private:
  Minerva::Core::Scene::SceneManager::RefPtr _sceneManager;
  Minerva::Core::DB::Connection::RefPtr _applicationConnection;
  unsigned int _sessionID;
  unsigned int _lastEventID;
  unsigned int _lastCommandID;
  unsigned int _timeout;
  Usul::Interfaces::IUnknown::QueryPtr _caller;
};

    }
  }
}


#endif // __WNV_CONTROLLER_H__

