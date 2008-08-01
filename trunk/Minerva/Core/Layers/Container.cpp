
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/Container.h"
#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Visitor.h"

#include "OsgTools/Group.h"

#include "Usul/Bits/Bits.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Functions/GUID.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Threads/Safe.h"

#include "osg/Group"

using namespace Minerva::Core::Layers;


///////////////////////////////////////////////////////////////////////////////
//
//  Register creators.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  Usul::Factory::RegisterCreator < Usul::Factory::TypeCreator < Container > > _creator_for_Container0 ( "Vector" );
  Usul::Factory::RegisterCreator < Usul::Factory::TypeCreator < Container > > _creator_for_Container1 ( "Container" );
}

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( Container, Container::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Container::Container() : 
  BaseClass (),
  _layers(),
  _updateListeners(),
  _builders(),
  _dataChangedListeners(),
  _name(),
  _guid ( Usul::Functions::GUID::generate() ),
  _shown ( true ),
  _flags ( Container::ALL ),
  _extents(),
  _root ( new osg::Group ),
SERIALIZE_XML_INITIALIZER_LIST
{
  USUL_TRACE_SCOPE;
  this->_registerMembers();
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
  _dataChangedListeners ( rhs._dataChangedListeners ),
  _name( rhs._name ),
  _guid( Usul::Functions::GUID::generate() ),
  _shown ( rhs._shown ),
  _flags ( rhs._flags | Container::SCENE_DIRTY ), // Make sure scene gets rebuilt.
  _extents ( rhs._extents ),
  _root ( new osg::Group ),
  SERIALIZE_XML_INITIALIZER_LIST
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members for serialization.
//
///////////////////////////////////////////////////////////////////////////////

void Container::_registerMembers()
{
  USUL_TRACE_SCOPE;
  SERIALIZE_XML_ADD_MEMBER ( _name );
  SERIALIZE_XML_ADD_MEMBER ( _guid );
  SERIALIZE_XML_ADD_MEMBER ( _shown );
  this->_addMember ( "layers", _layers );
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
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ILayer::IID:
    return static_cast < Usul::Interfaces::ILayer* > ( this );
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::ISerialize::IID:
    return static_cast < Usul::Interfaces::ISerialize* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener* > ( this );
  case Minerva::Interfaces::IDirtyData::IID:
    return static_cast < Minerva::Interfaces::IDirtyData* > ( this );
  case Minerva::Interfaces::IDirtyScene::IID:
    return static_cast < Minerva::Interfaces::IDirtyScene* > ( this );
  case Usul::Interfaces::ITreeNode::IID:
    return static_cast < Usul::Interfaces::ITreeNode* > ( this );
  case Usul::Interfaces::IBooleanState::IID:
    return static_cast < Usul::Interfaces::IBooleanState* > ( this );
  case Minerva::Interfaces::IElevationChangedListnerer::IID:
    return static_cast < Minerva::Interfaces::IElevationChangedListnerer* > ( this );
  case Minerva::Interfaces::IAddLayer::IID:
    return static_cast < Minerva::Interfaces::IAddLayer* > ( this );
  case Minerva::Interfaces::IRemoveLayer::IID:
    return static_cast < Minerva::Interfaces::IRemoveLayer* > ( this );
  case Usul::Interfaces::ILayerExtents::IID:
    return static_cast < Usul::Interfaces::ILayerExtents* > ( this );
  case Usul::Interfaces::IDataChangedNotify::IID:
    return static_cast < Usul::Interfaces::IDataChangedNotify* > ( this );
  default:
    return 0x0;
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
    if ( Minerva::Core::Layers::Container *Container = dynamic_cast< Minerva::Core::Layers::Container*> ( (*iter).get() ) )
    {
      Container->accept ( visitor );
    }

    if ( Minerva::Core::Data::DataObject *object = dynamic_cast< Minerva::Core::Data::DataObject*> ( (*iter).get() ) )
    {
      object->accept ( visitor );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void Container::name ( const std::string& name )
{
  USUL_TRACE_SCOPE;
  
  // Set the name.
  Usul::Threads::Safe::set ( this->mutex(), name, _name );
  
  // Notify any listeners that the data has changed.
  this->_notifyDataChnagedListeners();
  
  //Guard guard ( this->mutex() );
  //_name = name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Container::name() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the guid.
//
///////////////////////////////////////////////////////////////////////////////

std::string Container::guid() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _guid;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show label.
//
///////////////////////////////////////////////////////////////////////////////

void Container::showLayer( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _shown = b;
  this->dirtyScene ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show label.
//
///////////////////////////////////////////////////////////////////////////////

bool Container::showLayer() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _shown;
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

  // Our scene needs rebuilt.
  this->dirtyScene ( true );
  
  // Notify any listeners that the data has changed.
  if ( notify )
    this->_notifyDataChnagedListeners();
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
  
  // Our scene needs rebuilt.
  this->dirtyScene( true );
  
  // Notify any listeners that the data has changed.
  this->_notifyDataChnagedListeners();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear data objects.
//
///////////////////////////////////////////////////////////////////////////////

void Container::clear ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _layers.clear();
  _builders.clear();
  _updateListeners.clear();

  // Our scene needs rebuilt.
  this->dirtyScene ( true );
  
  // Notify any listeners that the data has changed.
  this->_notifyDataChnagedListeners();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of data objects in this layer.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Container::number() const
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

void Container::_buildScene( Usul::Interfaces::IUnknown *caller )
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

        // Build the scene.
        if ( show && build.valid() )
          _root->addChild ( build->buildScene ( Usul::Interfaces::IBuildScene::Options(), caller ) );
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
//  Set the extents.
//
///////////////////////////////////////////////////////////////////////////////

void Container::extents ( const Extents& e )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _extents = e;
  this->dirtyExtents ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

Container::Extents Container::extents() const
{
  USUL_TRACE_SCOPE;
  
  if ( this->dirtyExtents() )
  {
    Usul::Math::Vec2d ll, ur;
    
    // Calculate new extents.
    this->_calculateExtents ( ll, ur );
    
    Extents extents ( ll[0], ll[1], ur[0], ur[1] );
    Usul::Threads::Safe::set ( this->mutex(), extents, _extents );
  }
  
  // Return the extents.
  return Usul::Threads::Safe::get ( this->mutex(), _extents );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

void Container::extents ( Usul::Math::Vec2d& lowerLeft, Usul::Math::Vec2d& upperRight )
{
  USUL_TRACE_SCOPE;
  Extents e ( this->extents() );
  lowerLeft.set  ( e.minLon(), e.minLat() );
  upperRight.set ( e.maxLon(), e.maxLat() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

void Container::_calculateExtents ( Usul::Math::Vec2d& lowerLeft, Usul::Math::Vec2d& upperRight ) const
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
  
  lowerLeft.set  ( extents.minLon(), extents.minLat() );
  upperRight.set ( extents.maxLon(), extents.maxLat() );
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

  // Check to see if the number of children in the root is the same as the data objects.  This is a hack before dirtyScene isn't alway accurate.
  const bool needsBuild ( root.valid() ? root->getNumChildren() != _builders.size() : false );

  // Build if we need to...
  if ( this->dirtyScene() || needsBuild )
    this->_buildScene( caller );

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

Usul::Interfaces::ITreeNode * Container::getChildNode ( unsigned int which )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return Usul::Interfaces::ITreeNode::QueryPtr ( _layers.at ( which ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

void Container::setTreeNodeName ( const std::string & s )
{
  USUL_TRACE_SCOPE;
  this->name ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

std::string Container::getTreeNodeName() const
{
  USUL_TRACE_SCOPE;
  return this->name();
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
//  Get this as an IUnknown.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Container::asUnknown()
{
  USUL_TRACE_SCOPE;
  return this->queryInterface( Usul::Interfaces::IUnknown::IID );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Elevation has changed within given extents.
//
///////////////////////////////////////////////////////////////////////////////

bool Container::elevationChangedNotify ( const Extents& extents, ImagePtr elevationData, IUnknown * caller )
{
  USUL_TRACE_SCOPE;

  bool handled ( false );

  Unknowns unknowns ( Usul::Threads::Safe::get ( this->mutex(), _layers ) );
  {
    for ( Unknowns::iterator iter = unknowns.begin(); iter != unknowns.end(); ++iter )
    {
      Minerva::Interfaces::IElevationChangedListnerer::QueryPtr ecl ( *iter );
      if ( ecl.valid() )
        handled = ecl->elevationChangedNotify ( extents, elevationData, caller );
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

void Container::addLayer ( Usul::Interfaces::ILayer *layer )
{
  USUL_TRACE_SCOPE;
  this->add ( layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer (IRemoveLayer).
//
///////////////////////////////////////////////////////////////////////////////

void Container::removeLayer ( Usul::Interfaces::ILayer * layer )
{
  USUL_TRACE_SCOPE;
  this->remove ( layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the min longitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double Container::minLon() const
{
  USUL_TRACE_SCOPE;
  return this->extents().minLon();
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the min latitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double Container::minLat() const
{
  USUL_TRACE_SCOPE;
  return this->extents().minLat();
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the max longitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double Container::maxLon() const
{
  USUL_TRACE_SCOPE;
  return this->extents().maxLon();
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the max latitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double Container::maxLat() const
{
  USUL_TRACE_SCOPE;
  return this->extents().maxLat();
}


/////////////////////////////////////////////////////////////////////////////////
//
//  Add the listener.  Note: No need to guard, _dataChangedListeners has it's own mutex.
//
///////////////////////////////////////////////////////////////////////////////

void Container::addDataChangedListener ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  _dataChangedListeners.add ( caller );
}


/////////////////////////////////////////////////////////////////////////////////
//
//  Remove the listener.  Note: No need to guard, _dataChangedListeners has it's own mutex.
//
///////////////////////////////////////////////////////////////////////////////

void Container::removeDataChangedListener ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  _dataChangedListeners.remove ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//   Notify data changed listeners.  Note: No need to guard, _dataChangedListeners has it's own mutex.
//
///////////////////////////////////////////////////////////////////////////////

void Container::_notifyDataChnagedListeners()
{
  USUL_TRACE_SCOPE;
  Usul::Interfaces::IUnknown::QueryPtr me ( this );
  _dataChangedListeners.for_each ( std::bind2nd ( std::mem_fun ( &IDataChangedListener::dataChangedNotify ), me.get() ) );
}
