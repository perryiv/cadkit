
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbOsgDatabase: OpenSceneGraph database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbOsgPrecompiled.h"
#include "DbOsgDatabase.h"
#include "DbOsgFunctions.h"
#include "DbOsgInline.h"
#include "DbOsgArraySetters.h"

#include "Interfaces/IInstanceQuery.h"
#include "Interfaces/IShapeQuery.h"
#include "Interfaces/IQueryVertices.h"
#include "Interfaces/IQueryNormals.h"
#include "Interfaces/IQueryTexCoords.h"
#include "Interfaces/IClientData.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlQueryPtr.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlMessageIds.h"

#include "osg/LOD"             // Keep these in here because they bring in 
#include "osg/Geode"           // <cmath> which fauls up <math.h> in 
#include "osg/Geometry"        // "Standard/SlTemplateSupport.h"
#include "osgDB/WriteFile"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif

// To help shorten up the lines.
#undef  ERROR
#define ERROR    this->_notifyError
#define PROGRESS this->_notifyProgress
#define WARNING  this->_notifyWarning
#define FORMAT   CadKit::getString

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( DbOsgDatabase, DbBaseTarget );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( DbOsgDatabase, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbOsgDatabase::DbOsgDatabase() : DbBaseTarget(),
  _root ( new osg::Group ),
  _groupStack ( new GroupStack ),
  _groupMap ( new GroupMap )
{
  SL_PRINT2 ( "In DbOsgDatabase::DbOsgDatabase(), this = %X\n", this );
  SL_ASSERT ( NULL != _groupStack.get() );
  SL_ASSERT ( NULL != _groupMap.get() );
  SL_ASSERT ( NULL != _root );

  // Push the root onto the stack.
  this->_pushGroup ( _root );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbOsgDatabase::~DbOsgDatabase()
{
  SL_PRINT2 ( "In DbOsgDatabase::~DbOsgDatabase(), this = %X\n", this );

  // The stack should just be holding the root.
  SL_ASSERT ( _root == _groupStack->top() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbOsgDatabase::queryInterface ( const unsigned long &iid )
{
  SL_PRINT2 ( "In DbOsgDatabase::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case IAssemblyNotify::IID:
    return static_cast<IAssemblyNotify *>(this);
  case IPartNotify::IID:
    return static_cast<IPartNotify *>(this);
  case IInstanceNotify::IID:
    return static_cast<IInstanceNotify *>(this);
  case ILodNotify::IID:
    return static_cast<ILodNotify *>(this);
  case IShapeNotify::IID:
    return static_cast<IShapeNotify *>(this);
  case ISetNotify::IID:
    return static_cast<ISetNotify *>(this);
  default:
    return DbBaseTarget::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file extension.
//
///////////////////////////////////////////////////////////////////////////////

std::string DbOsgDatabase::getFileExtension() const
{
  SL_PRINT2 ( "In DbOsgDatabase::getFileExtension(), this = %X\n", this );
  return "osg";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Store the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::storeData ( const std::string &filename )
{
  SL_PRINT3 ( "In DbOsgDatabase::storeData(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );
  SL_ASSERT ( NULL != _root );

  // Write the _root to file.
  return osgDB::writeNodeFile ( *_root, filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::startEntity ( AssemblyHandle assembly, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::startEntity(), this = %X, assembly = %X, caller = %X\n", this, assembly, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // If our stack is just holding the root (i.e., this is the first assembly) 
  // then clear the group map. Take this out when you add a traversal-start/end interface.
  if ( _root == _groupStack->top() )
    _groupMap->clear();

  // Get the interface we need from the caller.
  SlQueryPtr<IAssemblyQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Create a group.
  SlRefPtr<osg::Group> group = CadKit::createGroup ( assembly, query.getValue() );
  if ( group.isNull() )
    return ERROR ( "Failed to create osg::Group for assembly.", NO_INTERFACE );

  // Add this group to the scene.
  _groupStack->top()->addChild ( group );

  // Put the group into our map.
  (*_groupMap)[assembly] = group;

  // Make it the new current group.
  this->_pushGroup ( group );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::endEntity ( AssemblyHandle assembly, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::endEntity(), this = %X, assembly = %X, caller = %X\n", this, assembly, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Done with this group.
  this->_popGroup();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::startEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::startEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // If there is no assembly then there must be only one part. So clear the
  // group map. Take this out when you add a traversal-start/end interface.
  if ( _groupStack->empty() )
    _groupMap->clear();

  // Get the interface we need from the caller.
  SlQueryPtr<IPartQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the interface we need from the caller.
  SlQueryPtr<IPartClientData> clientData ( caller );
  if ( clientData.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Create a group.
  SlRefPtr<osg::Group> group = CadKit::createGroup ( part, query.getValue() );
  if ( group.isNull() )
    return ERROR ( "Failed to create osg::Group for part.", NO_INTERFACE );

  // Add this group to the scene.
  _groupStack->top()->addChild ( group );

  // Put the group into our map.
  (*_groupMap)[part] = group;

  // Set the client data for this part.
  clientData->setClientData ( part, group.getValue() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::endEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::endEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // TODO, set the LOD's ranges.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::startEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::startEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Get the interface we need from the caller.
  SlQueryPtr<IInstanceQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Create a group.
  SlRefPtr<osg::Group> group = CadKit::createGroup ( instance, query.getValue() );
  if ( group.isNull() )
    return ERROR ( "Failed to create osg::Group for instance.", FAILED );

  // Get the corresponding part or assembly.
  HierarchyHandle hierarchy = query->getCorresponding ( instance );
  if ( NULL == hierarchy )
    return ERROR ( "Failed to get corresponding part or assembly from the instance.", FAILED );

  // Find the group that corresponds with this part or assembly.
  SlRefPtr<osg::Group> original = this->_findGroup ( hierarchy );
  if ( original.isNull() )
    return ERROR ( "Failed to find group associated with instance's corresponding part or assembly.", FAILED );

  // Add all of the original group's children to the instance's group.
  unsigned int numChildren = original->getNumChildren();
  for ( unsigned int i = 0; i < numChildren; ++i )
    group->addChild ( original->getChild ( i ) );

  // Add this group to the scene.
  _groupStack->top()->addChild ( group );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::endEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::endEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the lod.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::startEntity ( LodHandle lod, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::startEntity(), this = %X, lod = %d, caller = %X\n", this, lod, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Get the interface we need from the caller.
  SlQueryPtr<ILodQuery> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the interface we need from the caller.
  SlQueryPtr<IPartClientData> partClientData ( caller );
  if ( partClientData.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the interface we need from the caller.
  SlQueryPtr<ILodClientData> lodClientData ( caller );
  if ( lodClientData.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Create a LOD group.
//  SlRefPtr<osg::LOD> osgLod = new osg::LOD; // TODO, make LODs work.
  SlRefPtr<osg::Group> osgLod = new osg::Group;
  if ( osgLod.isNull() )
    return ERROR ( "Failed to create osg::LOD for given lod handle.", FAILED );

  // Create a Geode.
  SlRefPtr<osg::Geode> geode = new osg::Geode;
  if ( geode.isNull() )
    return ERROR ( "Failed to create osg::Geode for given lod handle.", FAILED );

  // Get the osg::Group associated with the lod's parent part.
  SlRefPtr<osg::Object> object = (osg::Object *) ( partClientData->getClientData ( query->getParent ( lod ) ) );
  SlRefPtr<osg::Group> group = dynamic_cast<osg::Group *> ( object.getValue() );
  if ( group.isNull() )
    return ERROR ( "Failed to find group to add lod to.", FAILED );

  // Add this Geode to the LOD.
  osgLod->addChild ( geode );

  // Add this LOD to the Group.
  group->addChild ( osgLod );

  // Set the client data for this lod.
  lodClientData->setClientData ( lod, geode.getValue() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the lod.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::endEntity ( LodHandle lod, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::endEntity(), this = %X, lod = %d, caller = %X\n", this, lod, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the shape.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::startEntity ( ShapeHandle shape, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::startEntity(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Get the interface we need from the caller.
  SlQueryPtr<IShapeQueryFloatUchar> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the interface we need from the caller.
  SlQueryPtr<ILodClientData> lodClientData ( caller );
  if ( lodClientData.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the osg::Geode we need.
  SlRefPtr<osg::Object> object = (osg::Object *) ( lodClientData->getClientData ( query->getParent ( shape ) ) );
  SlRefPtr<osg::Geode> geode = dynamic_cast<osg::Geode *> ( object.getValue() );
  if ( geode.isNull() )
    return ERROR ( "Failed to find geode to add shape geometry to.", FAILED );

  // Create a Geometry drawable.
  SlRefPtr<osg::Geometry> geometry = new osg::Geometry;
  if ( geometry.isNull() )
    return ERROR ( "Failed to create osg::Geometry for given shape handle.", FAILED );

  // Greate a StateSet.
  SlRefPtr<osg::StateSet> state = new osg::StateSet;
  if ( state.isNull() )
    return ERROR ( "Failed to create osg::StateSet for given shape handle.", FAILED );

  // Add the vertices, normals, etc.
  if ( false == this->_addDataSets ( caller, shape, geometry ) )
    return ERROR ( "Failed to add shape sets for given shape.", FAILED );

  // Add the material, texture, etc.
  if ( false == this->_addAttributes ( caller, shape, state ) )
    return ERROR ( "Failed to add shape sets for given shape.", FAILED );

  // Set the state of the geometry.
  geometry->setStateSet ( state );

  // Add the Geometry to the Geode.
  geode->addDrawable ( geometry );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's attributes to the given state.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::_addAttributes ( IUnknown *caller, ShapeHandle shape, osg::StateSet *state )
{
  SL_PRINT5 ( "In DbOsgDatabase::_addAttributes(), this = %X, caller = %X, shape = %d, state = %X\n", this, caller, shape, state );
  SL_ASSERT ( caller );
  SL_ASSERT ( state );
  
  // Get the interface we need from the caller.
  SlQueryPtr<IShapeQueryFloatUchar> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the material from the shape, part, or assembly.
  SlMaterialf material;
  if ( true == query->getMaterial ( shape, material, true ) )
    CadKit::setMaterial ( material, state );

  // TODO, texture.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's vertices, normals, etc, to the given geometry.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::_addDataSets ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbOsgDatabase::_addDataSets(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry );
  
  // Add the vertices. If we don't add any then we failed.
  if ( false == this->_addVertices ( caller, shape, geometry ) )
    return ERROR ( FORMAT ( "Failed to get vertices for shape %X.", shape ), FAILED );

  // Add the normals, colors, and texture coordinates, if there are any.
  // It is ok if these fail.
  this->_addNormals   ( caller, shape, geometry );
  this->_addColors    ( caller, shape, geometry );
  this->_addTexCoords ( caller, shape, geometry );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's vertices.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::_addVertices ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbOsgDatabase::_addVertices(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry );
  
  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeVerticesVec3f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the primitive type.
  IQueryShapeVerticesVec3f::Type type;
  if ( false == query->getVertexSetType ( shape, type ) )
    return ERROR ( "Failed to obtain primitive type.", FAILED );

  // Should be true.
  SL_ASSERT ( IQueryShapeVerticesVec3f::UNKNOWN != type );

  // Get the vertices.
  DbOsgVertexSetter setter ( type );
  if ( query->getVertices ( shape, setter ) )
  {
    // Set the vertices and the primitives.
    geometry->setVertexArray ( setter.getVertices() );
    geometry->setPrimitiveList ( setter.getPrimitives() );

    // It worked.
    return true;
  }

  // It didn't work.
  return ERROR ( FORMAT ( "Failed to get vertices for shape %X.", shape ), FAILED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's normals.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::_addNormals ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbOsgDatabase::_addNormals(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry );
  
  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeNormalsVec3f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the normal binding.
  VertexBinding binding;
  if ( false == query->getNormalBinding ( shape, binding ) )
    return ERROR ( "Failed to obtain normal binding.", FAILED );

  // Get the normals.
  DbOsgNormalSetter setter ( binding );
  if ( query->getNormals ( shape, setter ) )
  {
    // Set the normals and the binding.
    geometry->setNormalArray ( setter.getNormals() );
    geometry->setNormalBinding ( setter.getBinding() );

    // It worked.
    return true;
  }

  // It didn't work. Don't report an error, there may not be any.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's colors.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::_addColors ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbOsgDatabase::_addColors(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry );
  
  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeColorsVec4f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the color binding.
  VertexBinding binding;
  if ( false == query->getColorBinding ( shape, binding ) )
    return ERROR ( "Failed to obtain color binding.", FAILED );

  // Get the colors.
  DbOsgColorSetter setter ( binding );
  if ( query->getColors ( shape, setter ) )
  {
    // Set the colors and the binding.
    geometry->setColorArray ( setter.getColors() );
    geometry->setColorBinding ( setter.getBinding() );

    // It worked.
    return true;
  }

  // It didn't work. Don't report an error, there may not be any.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's texture coordinates.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::_addTexCoords ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbOsgDatabase::_addTexCoords(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry );
  
  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeTexCoordsVec2f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the texture coordinates.
  DbOsgTextureCoordSetter texCoords;
  if ( query->getTextureCoords ( shape, texCoords ) )
  {
    // Set the texture coordinates.
    geometry->setTexCoordArray ( 0, texCoords.getTextureCoords() );

    // It worked.
    return true;
  }

  // It didn't work. Don't report an error, there may not be any.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the shape.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::endEntity ( ShapeHandle shape, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::endEntity(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the set.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::startEntity ( SetHandle set, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::startEntity(), this = %X, set = %d, caller = %X\n", this, set, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // We ignore this because we build the geometry in the shape function above.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the set.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::endEntity ( SetHandle set, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::endEntity(), this = %X, set = %d, caller = %X\n", this, set, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the group.
//
///////////////////////////////////////////////////////////////////////////////

void DbOsgDatabase::_pushGroup ( osg::Group *group )
{
  SL_PRINT3 ( "In DbOsgDatabase::_pushGroup(), this = %X, group = %X\n", this, group );
  SL_ASSERT ( group );

  // Push it onto our stack.
  _groupStack->push ( group );

  // Reference it.
  group->ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the group.
//
///////////////////////////////////////////////////////////////////////////////

void DbOsgDatabase::_popGroup()
{
  SL_PRINT2 ( "In DbOsgDatabase::_popGroup(), this = %X\n", this );
  SL_ASSERT ( false == _groupStack->empty() );

  // Unreference the top one.
  _groupStack->top()->unref();

  // Pop it off of our stack.
  _groupStack->pop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the group associated with the key.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *DbOsgDatabase::_findGroup ( const void *key ) const
{
  SL_PRINT3 ( "In DbOsgDatabase::_findGroup(), this = %X, key = %X\n", this, key );
  SL_ASSERT ( NULL != _groupMap.get() );

  // See if it's in our map.
	GroupMap::iterator i = _groupMap->find ( key );

  // Return the result (which may be null).
  return ( i != _groupMap->end() ) ? i->second : NULL;
}
