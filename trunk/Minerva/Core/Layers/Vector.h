
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_VECTOR_H__
#define __MINERVA_LAYERS_VECTOR_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/DataObjects/DataObject.h"
#include "Minerva/Interfaces/IDirtyScene.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/ISerialize.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Math/Vector2.h"

#include "Serialize/XML/Macros.h"

#include "osg/Group"

#include <vector>

namespace Minerva {
namespace Core { 
  
  class Visitor;
  
namespace Layers {

class MINERVA_EXPORT Vector : public Usul::Base::Object,
                              public Usul::Interfaces::IBuildScene,
                              public Usul::Interfaces::ILayer,
                              public Usul::Interfaces::ISerialize,
                              public Usul::Interfaces::IUpdateListener,
                              public Minerva::Interfaces::IDirtyScene
{
public:
  /// Typedefs.
  typedef Usul::Base::Object                        BaseClass;
  typedef Minerva::Core::DataObjects::DataObject    DataObject;
  typedef DataObject::RefPtr                        DataObjectPtr;
  typedef std::vector< DataObjectPtr >	            DataObjects;
  
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( Vector );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  enum Flags
  {
    DATA_DIRTY    = 0x00000001,
    EXTENTS_DIRTY = 0x00000002,
    SCENE_DIRTY   = 0x00000004,
    ALL           = DATA_DIRTY | EXTENTS_DIRTY | SCENE_DIRTY
  };
  
  Vector();
  
  /// Accept the visitor.
  virtual void                accept ( Minerva::Core::Visitor& visitor );
  
  /// Traverse all DataObjects.
  virtual void                traverse ( Minerva::Core::Visitor& visitor );
  
  /// Build the scene (IBuildScene).
  virtual osg::Node *         buildScene ( const Options &options, Usul::Interfaces::IUnknown *caller = 0x0 );
  
  /// Get/Set the name (ILayer).
  virtual void                name( const std::string& name );
  virtual std::string         name() const;
  
  /// Get the guid (ILayer).
  virtual std::string         guid() const;
  
  /// Get/Set show layer (ILayer).
  virtual void                showLayer( bool b );
  virtual bool                showLayer() const;
  
  /// Data object functions.
  void                        addDataObject ( DataObject *dataObject );
  void                        clearDataObjects ();
  
  /// Get the number of data objects in this layer.
  virtual unsigned int        number() const;
  
  /// Get/Set the flags.
  unsigned int                flags() const;
  void                        flags( unsigned int );
  
  /// Get/Set the data dirty flag.
  bool                        dirtyData() const;
  void                        dirtyData( bool );
  
  /// Get/Set the extents dirty flag.
  bool                        dirtyExtents() const;
  void                        dirtyExtents( bool );
  
  /// Get/Set dirty scene flag.
  virtual bool                dirtyScene() const;
  virtual void                dirtyScene( bool b, Usul::Interfaces::IUnknown* caller = 0x0 );
  
  /// Get the extents.
  void                        extents ( Usul::Math::Vec2d& lowerLeft, Usul::Math::Vec2d& upperRight );
  
  // Update.
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );
  
protected:
  virtual ~Vector();
  
  Vector ( const Vector& rhs );
  
  /// Get the extents.
  virtual void                _calculateExtents ( Usul::Math::Vec2d& lowerLeft, Usul::Math::Vec2d& upperRight ) const;
  
  /// Build the scene.
  void                        _buildScene( Usul::Interfaces::IUnknown *caller );
private:
  // Do not use.
  Vector& operator= ( const Vector& rhs );
  
  // Register members for serialization.
  void                        _registerMembers();
  
  DataObjects _dataObjects;
  std::string _name;
  std::string _guid;
  bool _shown;
  unsigned int _flags;
  Usul::Math::Vec2d _lowerLeft;
  Usul::Math::Vec2d _upperRight;
  osg::ref_ptr<osg::Group> _root;
  
  SERIALIZE_XML_DEFINE_MEMBERS ( Vector );
  SERIALIZE_XML_DEFINE_MAP;
};


}
}
}

#endif // __MINERVA_LAYERS_VECTOR_H__
