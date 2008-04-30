
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
#include "Usul/Containers/Unknowns.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/ISerialize.h"
#include "Usul/Interfaces/ITreeNode.h"
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
                              public Minerva::Interfaces::IDirtyScene,
                              public Usul::Interfaces::ITreeNode,
                              public Usul::Interfaces::IBooleanState
{
public:
  /// Typedefs.
  typedef Usul::Base::Object                        BaseClass;
  typedef Minerva::Core::DataObjects::DataObject    DataObject;
  typedef DataObject::RefPtr                        DataObjectPtr;
  typedef std::vector< DataObjectPtr >	            DataObjects;
  typedef Usul::Interfaces::ILayer                  ILayer;
  typedef Usul::Interfaces::IUnknown                IUnknown;
  typedef std::vector<ILayer::QueryPtr>             Unknowns;
  
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

  /// Deserialize.
  virtual void                deserialize ( const XmlTree::Node &node );
  
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
  
  /// Add an object.
  void                        add ( Usul::Interfaces::IUnknown* layer );
  
  /// Remove an object.
  void                        remove ( Usul::Interfaces::IUnknown* layer );
  
  /// Clear objects.
  void                        clear();
  
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
  
  // Get the number of children (ITreeNode).
  virtual unsigned int        getNumChildNodes() const;
  
  // Get the child node (ITreeNode).
  virtual ITreeNode *         getChildNode ( unsigned int which );
  
  // Set/get the name (ITreeNode).
  virtual void                setTreeNodeName ( const std::string & );
  virtual std::string         getTreeNodeName() const;
  
  // Set/get the state (IBooleanState).
  virtual void                setBooleanState ( bool );
  virtual bool                getBooleanState() const;
  
private:
  // Do not use.
  Vector& operator= ( const Vector& rhs );
  
  // Register members for serialization.
  void                        _registerMembers();

  typedef Usul::Containers::Unknowns<IUpdateListener> UpdateListeners;
  typedef Usul::Containers::Unknowns<IBuildScene>     Builders;
  
  Unknowns _layers;
  UpdateListeners _updateListeners;
  Builders _builders;
  std::string _name;
  std::string _guid;
  bool _shown;
  unsigned int _flags;
  Usul::Math::Vec2d _lowerLeft;
  Usul::Math::Vec2d _upperRight;
  osg::ref_ptr<osg::Group> _root;
  
  SERIALIZE_XML_CLASS_NAME( Vector )
  SERIALIZE_XML_SERIALIZE_FUNCTION
  SERIALIZE_XML_ADD_MEMBER_FUNCTION
  SERIALIZE_XML_DEFINE_MAP;
};


}
}
}

#endif // __MINERVA_LAYERS_VECTOR_H__
