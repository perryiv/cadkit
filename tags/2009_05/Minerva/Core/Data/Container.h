
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
#include "Minerva/Core/Data/Feature.h"
#include "Minerva/Interfaces/IAddLayer.h"
#include "Minerva/Interfaces/IContainer.h"
#include "Minerva/Interfaces/IDirtyScene.h"
#include "Minerva/Interfaces/IElevationChangedListener.h"
#include "Minerva/Interfaces/IIntersectNotify.h"
#include "Minerva/Interfaces/IRemoveLayer.h"
#include "Minerva/Interfaces/ITilesChangedListener.h"
#include "Minerva/Interfaces/IWithinExtents.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/ITileVectorData.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Math/Vector2.h"

#include "Serialize/XML/Macros.h"

#include "osg/Group"

#include <map>
#include <string>
#include <vector>

namespace Minerva {
namespace Core { 

class Visitor;

namespace Data {

class MINERVA_EXPORT Container : 
  public Minerva::Core::Data::Feature,
  public Usul::Interfaces::IBuildScene,
  public Usul::Interfaces::ILayer,
  public Usul::Interfaces::IUpdateListener,
  public Usul::Interfaces::IBooleanState,
  public Minerva::Interfaces::IDirtyScene,
  public Minerva::Interfaces::IAddLayer,
  public Minerva::Interfaces::IContainer,
  public Minerva::Interfaces::IRemoveLayer,
  public Minerva::Interfaces::IElevationChangedListener,
  public Minerva::Interfaces::ITilesChangedListener,
  public Usul::Interfaces::ITileVectorData,
  public Minerva::Interfaces::IWithinExtents,
  public Minerva::Interfaces::IIntersectNotify
{
public:

  /// Typedefs.
  typedef Minerva::Core::Data::Feature              BaseClass;
  typedef BaseClass::Extents                        Extents;
  typedef Usul::Interfaces::ILayer                  ILayer;
  typedef Usul::Interfaces::IUnknown                IUnknown;
  typedef std::vector<IUnknown::QueryPtr>           Unknowns;
  typedef osg::ref_ptr<osg::Image>                  ImagePtr;
  typedef Usul::Interfaces::ITileVectorData         ITileVectorData;
  typedef ITileVectorData::Jobs                     TileVectorJobs;
  typedef Minerva::Interfaces::IWithinExtents       IWithinExtents;
  typedef Minerva::Interfaces::IIntersectNotify     IIntersectNotify;
  typedef IIntersectNotify::Closest                 Closest;
  typedef std::vector<std::string> Comments;

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
  
  /// Accept the visitor.
  virtual void                accept ( Minerva::Core::Visitor& visitor );

  /// Add an object.
  void                        add ( IUnknown* layer, bool notify = true );

  /// Build the scene (IBuildScene).
  virtual osg::Node *         buildScene ( const Options &options, IUnknown *caller = 0x0 );

  /// Get the extents.
  virtual Extents             calculateExtents() const;
  
  /// Clear objects.
  void                        clear();

  /// Deserialize.
  virtual void                deserialize ( const XmlTree::Node &node );

  /// Get/Set the data dirty flag.
  bool                        dirtyData() const;
  void                        dirtyData( bool );
  
  /// Get/Set the extents dirty flag.
  bool                        dirtyExtents() const;
  void                        dirtyExtents ( bool );
  
  /// Get/Set dirty scene flag (IDirtyScene).
  virtual bool                dirtyScene() const;
  virtual void                dirtyScene ( bool b, Usul::Interfaces::IUnknown* caller = 0x0 );

  /// Elevation has changed within given extents (IElevationChangeListener).
  virtual bool                elevationChangedNotify ( const Extents& extents, unsigned int level, ElevationDataPtr elevationData, Usul::Interfaces::IUnknown * caller = 0x0 );
  
  /// Find unknown with given id.  The function will return null if not found.
  IUnknown::RefPtr            find ( const ObjectID& id ) const;
  
  /// Get/Set the flags.
  unsigned int                flags() const;
  void                        flags ( unsigned int );

  /// Get the guid (ILayer).
  virtual std::string         guid() const;

  /// Call this when there is an intersection.
  virtual void                intersectNotify ( double x, double y, double z, double lon, double lat, double elev, IUnknown::RefPtr tile, IUnknown::RefPtr body, IUnknown::RefPtr caller, Closest & );

  /// See if the given level falls within this layer's range of levels.
  bool                        isInLevelRange ( unsigned int level ) const;

  /// Launch the jobs to fetch vector data.
  virtual TileVectorJobs      launchVectorJobs ( double minLon, double minLat, double maxLon, double maxLat, unsigned int level, Usul::Jobs::Manager *manager, Usul::Interfaces::IUnknown::RefPtr caller );

  /// Get/Set the name (ILayer).
  virtual std::string         name() const;
  virtual void                name( const std::string& );

  /// Get the number of data objects in this layer.
  virtual unsigned int        size() const;

  /// Remove an object.
  void                        remove ( IUnknown* layer );

  /// Get/Set show layer (ILayer).
  virtual void                showLayer ( bool b );
  virtual bool                showLayer() const;
  
  /// Notifications of change in tile state.
  virtual void                tileAddNotify     ( IUnknown::RefPtr child, IUnknown::RefPtr parent );
  virtual void                tileRemovedNotify ( IUnknown::RefPtr child, IUnknown::RefPtr parent );

  /// Traverse all DataObjects.
  virtual void                traverse ( Minerva::Core::Visitor& visitor );
  
  /// Update.
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  /// Minerva::Interfaces::IWithinExtents
  IUnknown::RefPtr            getItemsWithinExtents ( double minLon, double minLat, double maxLon, double maxLat, IUnknown::RefPtr caller = IUnknown::RefPtr ( 0x0 ) ) const;

protected:

  virtual ~Container();
  
  Container ( const Container& rhs );
  
  // Build the scene.
  void                        _buildScene ( Usul::Interfaces::IUnknown *caller );

  // Add a layer (IAddLayer).
  virtual void                addLayer ( Usul::Interfaces::IUnknown *layer );

  // Minerva::Interfaces::IContainer.
  virtual Container *         container();

  /// Remove a layer (IRemoveLayer).
  virtual void                removeLayer ( Usul::Interfaces::IUnknown * layer );
  
  // Get the number of children (ITreeNode).
  virtual unsigned int        getNumChildNodes() const;
  
  // Get the child node (ITreeNode).
  virtual ITreeNode::RefPtr   getChildNode ( unsigned int which );
  
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
  typedef std::map<ObjectID,IUnknown::RefPtr>         UnknownMap;
  
  Unknowns _layers;
  UpdateListeners _updateListeners;
  Builders _builders;
  unsigned int _flags;
  osg::ref_ptr<osg::Group> _root;
  UnknownMap _unknownMap;
  Comments _comments;
  
  SERIALIZE_XML_CLASS_NAME( Container )
};


}
}
}

#endif // __MINERVA_LAYERS_CONTAINER_H__
