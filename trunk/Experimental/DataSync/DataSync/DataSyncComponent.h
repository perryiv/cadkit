
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _RAY_CAST_VOL_VIZ_COMPONENT_CLASS_H_
#define _RAY_CAST_VOL_VIZ_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IPluginInitialize.h"
#include "Usul/Interfaces/IDocumentCreate.h"
#include "Usul/Interfaces/IDataSync.h"

#include "XmlTree/Document.h"

#include "VRV/Core/SharedBoolean.h"
#include "plugins/ApplicationDataManager/UserData.h"

#include <string>
#include <map>

class SceneDocument;


class DataSyncComponent : public Usul::Base::Object,
                          public Usul::Interfaces::IPlugin,
                          public Usul::Interfaces::IDataSync,
                          public Usul::Interfaces::IPluginInitialize
{
public:

  /// Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Documents::Document Document;
  typedef VRV::Core::SharedBoolean SharedBoolean;
  typedef std::pair< std::string, std::string > Machine;
  typedef std::vector< Machine > Machines;
  typedef std::map< std::string, cluster::UserData< SharedBoolean > > SharedBoolMap;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DataSyncComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  DataSyncComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string           getPluginName() const { return "Multi Process Data Synchronization"; }

  /// Usul::Interfaces::IDataSync
  virtual void                  setDataFlag( const std::string &machine, bool value );
  virtual bool                  queryDataState();
  virtual void                  resetData();
  
  /// Usul::Interfaces::IPluginInitialize
  virtual void                  initialize( Usul::Interfaces::IUnknown *caller );

  


protected: 

  // Do not copy.
  DataSyncComponent ( const DataSyncComponent & );
  DataSyncComponent &operator = ( const DataSyncComponent & );

  /// Use reference counting.
  virtual ~DataSyncComponent();

  void                          _readConfigFile( XmlTree::Node &node, Unknown *caller, Unknown *progress );
  void                          _parseMachine( XmlTree::Node &node, Unknown *caller, Unknown *progress );



private:
  SharedBoolMap                 _sharedBoolMap;
  Machines                      _machines;



};


#endif // _RAY_CAST_VOL_VIZ_COMPONENT_CLASS_H_
