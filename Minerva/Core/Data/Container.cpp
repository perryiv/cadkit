
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Data/Container.h"
#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Visitor.h"
#include "Minerva/Interfaces/IFeature.h"

#include "OsgTools/Group.h"

#include "Usul/Bits/Bits.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Threads/Safe.h"

#include "osg/Group"

#include <limits>

using namespace Minerva::Core::Data;


///////////////////////////////////////////////////////////////////////////////
//
//  Register creators.
//
///////////////////////////////////////////////////////////////////////////////

USUL_FACTORY_REGISTER_CREATOR_WITH_NAME ( "Vector", Container );
USUL_FACTORY_REGISTER_CREATOR_WITH_NAME ( "Container", Container );

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Container, Container::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Container::Container() : 
  BaseClass(),
  _layers(),
  _updateListeners(),
  _builders(),
  _flags ( Container::ALL ),
  _root ( new osg::Group ),
  _unknownMap(),
  _comments()
{
  USUL_TRACE_SCOPE;
  this->_registerMembers();

  // Default comments.
  _comments.push_back ( "This comment will get read and written again" );
  _comments.push_back ( "There can be more than one comment" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Container::Container( const Container& rhs ) : 
  BaseClass ( rhs ),
  _layers( rhs._layers ),
  _updateListeners ( rhs._updateListeners ),
  _builders ( rhs._builders ),
  _flags ( rhs._flags | Container::SCENE_DIRTY ), // Make sure scene gets rebuilt.
  _root ( new osg::Group ),
  _unknownMap ( rhs._unknownMap ),
  _comments ( rhs._comments )
{
  USUL_TRACE_SCOPE;
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Container::~Container()
{
  USUL_TRACE_SCOPE;

  _layers.clear();
  _updateListeners.clear();
  _builders.clear();
  _unknownMap.clear();
  _comments.clear();
  _root = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members for serialization.
//
///////////////////////////////////////////////////////////////////////////////

void Container::_registerMembers()
{
  USUL_TRACE_SCOPE;
  this->_addMember ( "layers", _layers );
  this->_addMember ( "comments", _comments );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query Interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Container::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;
  switch ( iid )
  {
  case Usul::Interfaces::ILayer::IID:
    return static_cast < Usul::Interfaces::ILayer* > ( this );
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::ISerialize::IID:
    return static_cast < Usul::Interfaces::ISerialize* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener* > ( this );
  case Minerva::Interfaces::IDirtyScene::IID:
    return static_cast < Minerva::Interfaces::IDirtyScene* > ( this );
  case Usul::Interfaces::IBooleanState::IID:
    return static_cast < Usul::Interfaces::IBooleanState* > ( this );
  case Minerva::Interfaces::IAddLayer::IID:
    return static_cast < Minerva::Interfaces::IAddLayer* > ( this );
  case Minerva::Interfaces::IContainer::IID:
    return static_cast < Minerva::Interfaces::IContainer* > ( this );
  case Minerva::Interfaces::IRemoveLayer::IID:
    return static_cast < Minerva::Interfaces::IRemoveLayer* > ( this );
  case Usul::Interfaces::IDataChangedNotify::IID:
    return static_cast < Usul::Interfaces::IDataChangedNotify* > ( this );
  case Minerva::Interfaces::IElevationChangedListener::IID:
    return static_cast<Minerva::Interfaces::IElevationChangedListener*> ( this );
  case Minerva::Interfaces::ITilesChangedListener::IID:
    return static_cast<Minerva::Interfaces::ITilesChangedListener*> ( this );
  case Usul::Interfaces::ITileVectorData::IID:
    return static_cast<Usul::Interfaces::ITileVectorData*> ( this );
  case Minerva::Interfaces::IWithinExtents::IID:
    return static_cast<Minerva::Interfaces::IWithinExtents*> ( this );
  case Minerva::Interfaces::IIntersectNotify::IID:
    return static_cast<Minerva::Interfaces::IIntersectNotify*> ( this );
  default:
    return BaseClass::queryInterface ( iid );
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Container::accept ( Minerva::Core::Visitor& visitor )
{
  USUL_TRACE_SCOPE;
  visitor.visit ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse all DataObjects.
//
///////////////////////////////////////////////////////////////////////////////

void Container::traverse ( Minerva::Core::Visitor& visitor )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  for ( Unknowns::iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
  {
    Minerva::Interfaces::IFeature::QueryPtr f ( *iter );
    Minerva::Core::Data::Feature::RefPtr feature ( f.valid() ? f->feature() : 0x0 );
    if ( feature.valid() )
    {
      feature->accept ( visitor );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the guid.
//
///////////////////////////////////////////////////////////////////////////////

std::string Container::guid() const
{
  USUL_TRACE_SCOPE;
  return this->objectId();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show label.
//
///////////////////////////////////////////////////////////////////////////////

void Container::showLayer ( bool b )
{
  USUL_TRACE_SCOPE;

  // Set the visibility state.
  this->visibility ( b );

  // The scene needs rebuilt.
  this->dirtyScene ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show label.
//
///////////////////////////////////////////////////////////////////////////////

bool Container::showLayer() const
{
  return this->visibility();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a data object.
//
///////////////////////////////////////////////////////////////////////////////

void Container::add ( Usul::Interfaces::IUnknown* unknown, bool notify )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != unknown )
  {
    Guard guard ( this );
    _layers.push_back ( unknown );
  }

  // Add the update listener.
  _updateListeners.add ( unknown );

  // Add the builder.
  _builders.add ( unknown );
  
  // Update the extents.
  this->_updateExtents ( unknown );
  
  // If we can get a GUID, save the mapping for quick lookup later.
  Minerva::Interfaces::IFeature::QueryPtr iFeature ( unknown );
  Minerva::Core::Data::Feature::RefPtr feature ( iFeature.valid() ? iFeature->feature() : 0x0 );
  if ( feature.valid() )
  {
    Guard guard ( this->mutex() );
    _unknownMap.insert ( UnknownMap::value_type ( feature->objectId(), unknown ) );
  }

  // Our scene needs rebuilt.
  this->dirtyScene ( true );
  
  // Notify any listeners that the data has changed.
  if ( notify )
    this->_notifyDataChangedListeners();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void Container::remove ( Usul::Interfaces::IUnknown* unknown )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  Unknowns::iterator doomed ( std::find( _layers.begin(), _layers.end(), Unknowns::value_type ( unknown ) ) );
  if( doomed != _layers.end() )
    _layers.erase( doomed );

  // Remove the update listener.
  _updateListeners.remove ( unknown );

  // Remove the builder.
  _builders.remove ( unknown );
  
  // If we can get a GUID, remove the mapping.
  Minerva::Interfaces::IFeature::QueryPtr iFeature ( unknown );
  Minerva::Core::Data::Feature::RefPtr feature ( iFeature.valid() ? iFeature->feature() : 0x0 );
  if ( feature.valid() )
  {
    Guard guard ( this->mutex() );
    _unknownMap.erase ( feature->objectId() );
  }
  
  // Our scene needs rebuilt.
  this->dirtyScene( true );
  
  // Notify any listeners that the data has changed.
  this->_notifyDataChangedListeners();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear data objects.
//
///////////////////////////////////////////////////////////////////////////////

void Container::clear()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _unknownMap.clear();
  _layers.clear();
  _builders.clear();
  _updateListeners.clear();

  // Our scene needs to be rebuilt.
  this->dirtyScene ( true );
  
  // Notify any listeners that the data has changed.
  this->_notifyDataChangedListeners();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of data objects in this layer.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Container::size() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _layers.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene (IBuildScene).
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * Container::buildScene ( const Options &options, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Container::_buildScene ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard( this->mutex() );
  
  if ( true == _root.valid() || this->dirtyScene() )
  {
    // For debugging...
    _root->setName ( this->name() );
    
    // Remove all children.
    OsgTools::Group::removeAllChildren ( _root.get() );

    // Add to the scene if we are shown.
    if ( this->showLayer() )
    {
      Builders::Guard guard ( _builders.mutex() );
      for ( Builders::iterator iter = _builders.begin(); iter != _builders.end(); ++iter )
      {
        // Query for needed interfaces.
        ILayer::QueryPtr layer ( *iter );
        Usul::Interfaces::IBuildScene::QueryPtr build ( *iter );

        // Should the layer be shown?
        const bool show ( layer.valid() ? layer->showLayer() : true );

        // Should we build the scene?
        if ( show && build.valid() )
        {
          // Build the scene. Handle possible null return.
          osg::ref_ptr<osg::Node> node ( build->buildScene ( Usul::Interfaces::IBuildScene::Options(), caller ) );
          if ( true == node.valid() )
          {
            _root->addChild ( node.get() );
          }
        }
      }
    }
    
    // Our scene is no longer dirty.
    this->dirtyScene ( false );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flags.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Container::flags() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _flags;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flags.
//
///////////////////////////////////////////////////////////////////////////////

void Container::flags( unsigned int f )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _flags = f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Container::dirtyData() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return Usul::Bits::has<unsigned int, unsigned int> ( _flags, Container::DATA_DIRTY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void Container::dirtyData( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _flags = Usul::Bits::set<unsigned int, unsigned int> ( _flags, Container::DATA_DIRTY, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Container::dirtyExtents() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return Usul::Bits::has<unsigned int, unsigned int> ( _flags, Container::EXTENTS_DIRTY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the extents dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void Container::dirtyExtents( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _flags = Usul::Bits::set<unsigned int, unsigned int> ( _flags, Container::EXTENTS_DIRTY, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get dirty scene flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Container::dirtyScene() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return Usul::Bits::has<unsigned int, unsigned int> ( _flags, Container::SCENE_DIRTY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set dirty scene flag.
//
///////////////////////////////////////////////////////////////////////////////

void Container::dirtyScene( bool b, Usul::Interfaces::IUnknown* caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _flags = Usul::Bits::set<unsigned int, unsigned int> ( _flags, Container::SCENE_DIRTY, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

Container::Extents Container::calculateExtents() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  Extents extents;
  for ( Unknowns::const_iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
  {
    Usul::Interfaces::ILayerExtents::QueryPtr le ( (*iter).get() );
    if ( le.valid() )
    {
      Extents e ( le->minLon(), le->minLat(), le->maxLon(), le->maxLat() );
      extents.expand ( e );
    }
  }
  
  return extents;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void Container::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  // No need to guard _builders or _updateListeners because they have thier own mutex.

  osg::ref_ptr<osg::Group> root ( Usul::Threads::Safe::get ( this->mutex(), _root ) );

  // Check to see if the number of children in the root is the same as the data objects. 
  // This is a hack from before dirtyScene was accurate.
  const bool needsBuild ( root.valid() ? root->getNumChildren() != _builders.size() : false );

  // Build if we need to...
  if ( this->dirtyScene() || needsBuild )
    this->_buildScene ( caller );

  // Ask each one to update.
  _updateListeners.for_each ( std::bind2nd ( std::mem_fun ( &IUpdateListener::updateNotify ), caller ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Container::getNumChildNodes() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _layers.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child node (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode::RefPtr Container::getChildNode ( unsigned int which )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return Usul::Interfaces::ITreeNode::RefPtr ( Usul::Interfaces::ITreeNode::QueryPtr ( _layers.at ( which ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state (IBooleanState).
//
///////////////////////////////////////////////////////////////////////////////

void Container::setBooleanState ( bool b )
{
  USUL_TRACE_SCOPE;
  this->showLayer ( b );

  // Set the state of our children.
  Unknowns unknowns ( Usul::Threads::Safe::get ( this->mutex(), _layers ) );
  for ( Unknowns::iterator iter = unknowns.begin(); iter != unknowns.end(); ++iter )
  {
    Usul::Interfaces::IBooleanState::QueryPtr state ( *iter );
    if ( state.valid() )
      state->setBooleanState ( b );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the state (IBooleanState).
//
///////////////////////////////////////////////////////////////////////////////

bool Container::getBooleanState() const
{
  USUL_TRACE_SCOPE;
  return this->showLayer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void Container::deserialize ( const XmlTree::Node &node )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _dataMemberMap.deserialize ( node );

  // Add layers.
  for ( Unknowns::iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
  {
    // Add the update listener.
    _updateListeners.add ( *iter );

    // Add the builder.
    _builders.add ( *iter );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Elevation has changed within given extents (IElevationChangeListener).
//
///////////////////////////////////////////////////////////////////////////////

bool Container::elevationChangedNotify ( const Extents& extents, unsigned int level, ElevationDataPtr elevationData, Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;
  
  bool handled ( false );
  
  Unknowns unknowns ( Usul::Threads::Safe::get ( this->mutex(), _layers ) );
  {
    for ( Unknowns::iterator iter = unknowns.begin(); iter != unknowns.end(); ++iter )
    {
      Minerva::Interfaces::IElevationChangedListener::QueryPtr ecl ( *iter );
      if ( ecl.valid() )
        handled = ecl->elevationChangedNotify ( extents, level, elevationData, caller );
    }
  }
  
  if ( handled )
    this->dirtyScene ( true );
  
  return handled;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer (IAddLayer).
//
///////////////////////////////////////////////////////////////////////////////

void Container::addLayer ( Usul::Interfaces::IUnknown *layer )
{
  USUL_TRACE_SCOPE;
  this->add ( layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer (IRemoveLayer).
//
///////////////////////////////////////////////////////////////////////////////

void Container::removeLayer ( Usul::Interfaces::IUnknown * layer )
{
  USUL_TRACE_SCOPE;
  this->remove ( layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given level falls within this layer's range of levels.
//
///////////////////////////////////////////////////////////////////////////////

bool Container::isInLevelRange ( unsigned int level ) const
{
  USUL_TRACE_SCOPE;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Container::name() const
{
  USUL_TRACE_SCOPE;
  return BaseClass::name();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void Container::name ( const std::string& name )
{
  USUL_TRACE_SCOPE;
  BaseClass::name ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A tile has been added.
//
///////////////////////////////////////////////////////////////////////////////

void Container::tileAddNotify ( Usul::Interfaces::IUnknown::RefPtr child, Usul::Interfaces::IUnknown::RefPtr parent )
{
  USUL_TRACE_SCOPE;

  Unknowns unknowns ( Usul::Threads::Safe::get ( this->mutex(), _layers ) );
  {
    for ( Unknowns::iterator iter = unknowns.begin(); iter != unknowns.end(); ++iter )
    {
      Minerva::Interfaces::ITilesChangedListener::QueryPtr tcl ( *iter );
      if ( tcl.valid() )
      {
        tcl->tileAddNotify ( child, parent );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  A tile has been removed.
//
///////////////////////////////////////////////////////////////////////////////

void Container::tileRemovedNotify ( Usul::Interfaces::IUnknown::RefPtr child, Usul::Interfaces::IUnknown::RefPtr parent ) 
{
  USUL_TRACE_SCOPE;

  Unknowns unknowns ( Usul::Threads::Safe::get ( this->mutex(), _layers ) );
  {
    for ( Unknowns::iterator iter = unknowns.begin(); iter != unknowns.end(); ++iter )
    {
      Minerva::Interfaces::ITilesChangedListener::QueryPtr tcl ( *iter );
      if ( tcl.valid() )
      {
        tcl->tileRemovedNotify ( child, parent );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Launch the jobs to fetch vector data.
//
///////////////////////////////////////////////////////////////////////////////

Container::TileVectorJobs Container::launchVectorJobs ( double minLon, 
                                                        double minLat, 
                                                        double maxLon, 
                                                        double maxLat, 
                                                        unsigned int level, 
                                                        Usul::Jobs::Manager *manager, 
                                                        Usul::Interfaces::IUnknown::RefPtr caller )
{
  USUL_TRACE_SCOPE;

  TileVectorJobs answer;

  Unknowns unknowns ( Usul::Threads::Safe::get ( this->mutex(), _layers ) );
  {
    for ( Unknowns::iterator i = unknowns.begin(); i != unknowns.end(); ++i )
    {
      Usul::Interfaces::ITileVectorData::QueryPtr tileVectorData ( *i );
      if ( true == tileVectorData.valid() )
      {
        TileVectorJobs jobs ( tileVectorData->launchVectorJobs ( minLon, minLat, maxLon, maxLat, level, manager, caller ) );
        answer.insert ( answer.end(), jobs.begin(), jobs.end() );
      }
    }
  }

  return answer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find unknown with given id.  The function will return null if not found.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr Container::find ( const ObjectID& id ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  UnknownMap::const_iterator iter ( _unknownMap.find ( id ) );
  if ( iter != _unknownMap.end() )
  {
    return iter->second;
  }
  
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the items within the extents.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr Container::getItemsWithinExtents ( double minLon, double minLat, double maxLon, double maxLat, IUnknown::RefPtr caller ) const
{
  USUL_TRACE_SCOPE;

  // Initialize.
  Container::RefPtr answer ( new Container );
  Extents givenExtents ( minLon, minLat, maxLon, maxLat );

  // Get a copy of the layers.
  const Unknowns layers ( Usul::Threads::Safe::get ( this->mutex(), _layers ) );

  // Loop through the layers.
  for ( Unknowns::const_iterator i = layers.begin(); i != layers.end(); ++i )
  {
    // Does this layer contain items?
    const Minerva::Interfaces::IWithinExtents::QueryPtr w ( i->get() );
    if ( true == w.valid() )
    {
      IUnknown::RefPtr contained ( w->getItemsWithinExtents ( minLon, minLat, maxLon, maxLat, caller ) );
      if ( true == contained.valid() )
      {
        answer->add ( contained.get(), false );
      }
    }
    else
    {
      // Does this layer have extents?
      const Usul::Interfaces::ILayerExtents::QueryPtr e ( i->get() );
      if ( true == e.valid() )
      {
        // Calculate the center.
        const Extents layerExtents ( e->minLon(), e->minLat(), e->maxLon(), e->maxLat() );
        const Extents::Vertex center ( layerExtents.center() );

        // Is the center in the extents?
        if ( true == givenExtents.contains ( center ) )
        {
          answer->add ( e.get(), false );
        }
      }
    }
  }

  // Return the answer.
  return IUnknown::QueryPtr ( ( answer->size() > 0 ) ? answer.get() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get this tile.
//
///////////////////////////////////////////////////////////////////////////////

Container *Container::container()
{
  USUL_TRACE_SCOPE;
  return this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call to notify of an intersection.
//
///////////////////////////////////////////////////////////////////////////////

void Container::intersectNotify ( double x, double y, double z, double lon, double lat, double elev, 
                                  IUnknown::RefPtr tile, IUnknown::RefPtr body, IUnknown::RefPtr caller, Closest &answer )
{
  USUL_TRACE_SCOPE;
  typedef IIntersectNotify::Path Path;
  typedef IIntersectNotify::Point Point;
  typedef IIntersectNotify::PointAndDistance PointAndDistance;

  // Get a copy of the layers.
  Unknowns layers ( Usul::Threads::Safe::get ( this->mutex(), _layers ) );

  // Loop through the layers.
  for ( Unknowns::iterator i = layers.begin(); i != layers.end(); ++i )
  {
    IIntersectNotify::QueryPtr notify ( i->get() );
    if ( true == notify.valid() )
    {
      Closest closest ( Path(), PointAndDistance ( Point(), std::numeric_limits<double>::max() ) );
      notify->intersectNotify ( x, y, z, lon, lat, elev, tile, body, caller, closest );

      if ( ( closest.second.second < answer.second.second ) && ( false == closest.first.empty() ) )
      {
        answer.first.insert ( answer.first.end(), Usul::Interfaces::IUnknown::QueryPtr ( this ) );
        answer.first.insert ( answer.first.end(), closest.first.begin(), closest.first.end() );
        answer.second = closest.second;
      }
    }
  }
}
