
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_CONTAINER_H__
#define __MINERVA_LAYERS_CONTAINER_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Extents.h"
#include "Minerva/Core/Data/Feature.h"
#include "Minerva/Interfaces/IAddLayer.h"
#include "Minerva/Interfaces/IDirtyData.h"
#include "Minerva/Interfaces/IDirtyScene.h"
#include "Minerva/Interfaces/IElevationChangedListener.h"
#include "Minerva/Interfaces/IRemoveLayer.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/ILayerExtents.h"
#include "Usul/Interfaces/ITreeNode.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Math/Vector2.h"

#include "Serialize/XML/Macros.h"

#include "osg/Group"

#include <vector>

namespace Minerva {
namespace Core { 

  class Visitor;

namespace Data {

class MINERVA_EXPORT Container : public Minerva::Core::Data::Feature,
                                 public Usul::Interfaces::IBuildScene,
                                 public Usul::Interfaces::ILayer,
                                 public Usul::Interfaces::IUpdateListener,
                                 public Usul::Interfaces::ITreeNode,
                                 public Usul::Interfaces::ILayerExtents,
                                 public Usul::Interfaces::IBooleanState,
                                 public Minerva::Interfaces::IDirtyData,
                                 public Minerva::Interfaces::IDirtyScene,
                                 public Minerva::Interfaces::IElevationChangedListnerer,
                                 public Minerva::Interfaces::IAddLayer,
                                 public Minerva::Interfaces::IRemoveLayer
{
public:

  /// Typedefs.
  typedef Minerva::Core::Data::Feature              BaseClass;
  typedef Usul::Interfaces::ILayer                  ILayer;
  typedef Usul::Interfaces::IUnknown                IUnknown;
  typedef std::vector<IUnknown::QueryPtr>           Unknowns;
  typedef Minerva::Core::Extents<osg::Vec2d>        Extents;
  
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( Container );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  enum Flags
  {
    DATA_DIRTY    = 0x00000001,
    EXTENTS_DIRTY = 0x00000002,
    SCENE_DIRTY   = 0x00000004,
    ALL           = DATA_DIRTY | EXTENTS_DIRTY | SCENE_DIRTY
  };
  
  Container();
  
  /// Get this as an IUnknown.
	virtual IUnknown*           asUnknown();
  
  /// Accept the visitor.
  virtual void                accept ( Minerva::Core::Visitor& visitor );

  /// Add an object.
  void                        add ( IUnknown* layer, bool notify = true );

  /// Build the scene (IBuildScene).
  virtual osg::Node *         buildScene ( const Options &options, IUnknown *caller = 0x0 );

  /// Clear objects.
  void                        clear();

  /// Deserialize.
  virtual void                deserialize ( const XmlTree::Node &node );

  /// Get/Set the data dirty flag (IDirtyData).
  virtual bool                dirtyData() const;
  virtual void                dirtyData( bool );
  
  /// Get/Set the extents dirty flag.
  bool                        dirtyExtents() const;
  void                        dirtyExtents ( bool );
  
  /// Get/Set dirty scene flag (IDirtyScene).
  virtual bool                dirtyScene() const;
  virtual void                dirtyScene ( bool b, Usul::Interfaces::IUnknown* caller = 0x0 );

  /// Elevation has changed within given extents (IElevationChangedListnerer).
  virtual bool                elevationChangedNotify ( const Extents& extents, ImagePtr elevationData, IUnknown * caller = 0x0 );

  /// Set/get the extents.
  void                        extents ( const Extents& e );
  Extents                     extents() const;
  void                        extents ( Usul::Math::Vec2d& lowerLeft, Usul::Math::Vec2d& upperRight );

  /// Get/Set the flags.
  unsigned int                flags() const;
  void                        flags ( unsigned int );

  /// Get the guid (ILayer).
  virtual std::string         guid() const;

  /// See if the given level falls within this layer's range of levels.
  bool                        isInLevelRange ( unsigned int level ) const;

  /// Get the min latitude and min longitude (ILayerExtents).
  virtual double              minLon() const;
  virtual double              minLat() const;
  
  /// Get the max latitude and max longitude (ILayerExtents).
  virtual double              maxLon() const;
  virtual double              maxLat() const;

  /// Get/Set the name (ILayer).
  virtual std::string         name() const;
  virtual void                name( const std::string& );

  /// Get the number of data objects in this layer.
  virtual unsigned int        number() const;

  /// Remove an object.
  void                        remove ( IUnknown* layer );

  /// Get/Set show layer (ILayer).
  virtual void                showLayer ( bool b );
  virtual bool                showLayer() const;

  /// Traverse all DataObjects.
  virtual void                traverse ( Minerva::Core::Visitor& visitor );
  
  // Update.
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );
  
protected:

  virtual ~Container();
  
  Container ( const Container& rhs );
  
  /// Get the extents.
  virtual void                _calculateExtents ( Usul::Math::Vec2d& lowerLeft, Usul::Math::Vec2d& upperRight ) const;
  
  /// Build the scene.
  void                        _buildScene( Usul::Interfaces::IUnknown *caller );

  // Add a layer (IAddLayer).
  virtual void                addLayer ( Usul::Interfaces::IUnknown *layer );

  /// Remove a layer (IRemoveLayer).
  virtual void                removeLayer ( Usul::Interfaces::IUnknown * layer );
  
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
  Container& operator= ( const Container& rhs );

  // Register members for serialization.
  void                        _registerMembers();

  typedef Usul::Containers::Unknowns<IUpdateListener> UpdateListeners;
  typedef Usul::Containers::Unknowns<IBuildScene>     Builders;
  
  Unknowns _layers;
  UpdateListeners _updateListeners;
  Builders _builders;
  unsigned int _flags;
  mutable Extents _extents;
  osg::ref_ptr<osg::Group> _root;
  
  SERIALIZE_XML_CLASS_NAME( Container )
};


}
}
}

#endif // __MINERVA_LAYERS_CONTAINER_H__
