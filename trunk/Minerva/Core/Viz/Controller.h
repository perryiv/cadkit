
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

#include "boost/archive/xml_iarchive.hpp"

#include "Minerva/Core/DB/Connection.h"
#include "Minerva/Core/Layers/Layer.h"
#include "Minerva/Core/DrawCallback.h"
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

  Controller();

  // Set the application connection.
  void applicationConnection( Minerva::Core::DB::Connection * );

  // Add a data connection.
  void addDataConnection( Minerva::Core::DB::Connection * );

  // Connect to a session.
  void connectToSession( const std::string& name );

  // Are there any events to process?
  bool hasEvents();

  // Update the scene.
  void updateScene( );  

  // Set the callback
  void callback ( DrawCallback * callback ) { _callback = callback; }

  void sceneManager ( Minerva::Core::Scene::SceneManager * manager ) { _sceneManager = manager; }

protected:
  /// Use reference counting
  virtual ~Controller();

  void _processEvents();

  void _processAddLayer    ( const std::string& tableName, int eventID );
  void _processRemoveLayer ( const std::string& tableName, int eventID );
  void _processAnimation   ( const std::string& tableName, int eventID );


  // Get the next event.
  void  _getNextEvent( int &type, std::string& tableName, int &eventId );

  Minerva::Core::Layers::Layer* _getLayer ( const std::string drawCommandTable, int eventID );

  bool _getAnimateCommand( const std::string animateTable, int eventID, bool &accumulate, bool &dateTimeStep, float &speed );

  // Update progress
  void _updateProgress();


private:
  Minerva::Core::Scene::SceneManager::RefPtr _sceneManager;
  DrawCallback::RefPtr _callback;
  Usul::Policies::TimeBased _update;

  Minerva::Core::DB::Connection::RefPtr _applicationConnection;
  unsigned int _sessionID;
  
  int _lastEventID;
};

    }
  }
}


#endif // __WNV_CONTROLLER_H__

