
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbSgDatabase: SceneGraph database class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbSgPrecompiled.h"
#include "DbSgDatabase.h"
#include "DbSgFunctions.h"
#include "DbSgInline.h"
/*
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

#include "SceneGraph/Core/SgLod.h"
#include "SceneGraph/Core/SgMaterial.h"
#include "SceneGraph/Core/SgBaseColor.h"
#include "SceneGraph/Core/SgTransformation.h"
#include "osg/Geometry"
#include "osgDB/WriteFile"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif

// To help shorten up the lines.
#undef  ERROR
#define ERROR    this->_notifyError
#define PROGRESS this->_notifyProgress
#define WARNING  this->_notifyWarning
#define FORMAT   CadKit::getString

#define LAST_LOD_RANGE            1e7
#define MAX_LOD_DISTANCE_FACTOR   20

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( DbSgDatabase, DbBaseTarget );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( DbSgDatabase, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbSgDatabase::DbSgDatabase() : DbBaseTarget(),
  _root ( new osg::Group ),
  _groupStack ( new GroupStack ),
  _groupMap ( new GroupMap )
{
  SL_PRINT2 ( "In DbSgDatabase::DbSgDatabase(), this = %X\n", this );
  SL_ASSERT ( NULL != _groupStack.get() );
  SL_ASSERT ( NULL != _groupMap.get() );
  SL_ASSERT ( NULL != _root );

  // Push the root onto the stack. This will reference it.
  this->_pushGroup ( _root );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbSgDatabase::~DbSgDatabase()
{
  SL_PRINT2 ( "In DbSgDatabase::~DbSgDatabase(), this = %X\n", this );

  // The stack should just be holding the root.
  SL_ASSERT ( _root == _groupStack->top() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbSgDatabase::queryInterface ( unsigned long iid )
{
  SL_PRINT2 ( "In DbSgDatabase::queryInterface(), this = %X\n", this );

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

std::string DbSgDatabase::getFileExtension() const
{
  SL_PRINT2 ( "In DbSgDatabase::getFileExtension(), this = %X\n", this );
  return "osg";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Store the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbSgDatabase::storeData ( const std::string &filename )
{
  SL_PRINT3 ( "In DbSgDatabase::storeData(), this = %X, filename = %s\n", this, filename.c_str() );
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

bool DbSgDatabase::startEntity ( AssemblyHandle assembly, IUnknown *caller )
{
  SL_PRINT4 ( "In DbSgDatabase::startEntity(), this = %X, assembly = %X, caller = %X\n", this, assembly, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // If our stack is just holding the root (i.e., this is the first assembly) 
  // then clear the group map. Take this out when you add a traversal-start/end interface.
  if ( _root == _groupStack->top() )
    _groupMap->clear();

  // Get the interface we need from the caller.
  SlQueryPtr<IAssemblyQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Create a group (really a MatrixTransform).
  SlRefPtr<osg::Group> mt = CadKit::createGroup ( assembly, query.getValue() );
  if ( mt.isNull() )
    return ERROR ( "Failed to create osg::MatrixTransform for assembly.", CadKit::NO_INTERFACE );

  // Add this group to the scene.
  _groupStack->top()->addChild ( mt );

  // Put the group into our map.
  (*_groupMap)[assembly] = mt;

  // Make it the new current group.
  this->_pushGroup ( mt );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbSgDatabase::endEntity ( AssemblyHandle assembly, IUnknown *caller )
{
  SL_PRINT4 ( "In DbSgDatabase::endEntity(), this = %X, assembly = %X, caller = %X\n", this, assembly, caller );
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

bool DbSgDatabase::startEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbSgDatabase::startEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // If there is no assembly then there must be only one part. So clear the
  // group map. Take this out when you add a traversal-start/end interface.
  if ( _groupStack->empty() )
    _groupMap->clear();

  // Get the interface we need from the caller.
  SlQueryPtr<IPartQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the interface we need from the caller.
  SlQueryPtr<IPartClientData> clientData ( caller );
  if ( clientData.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Create a group (really a MatrixTransform).
  SlRefPtr<osg::Group> mt = CadKit::createGroup ( part, query.getValue() );
  if ( mt.isNull() )
    return ERROR ( "Failed to create osg::MatrixTransform for assembly.", CadKit::NO_INTERFACE );

  // Create a LOD group.
  SlRefPtr<osg::LOD> lod = new osg::LOD;
  if ( lod.isNull() )
    return ERROR ( "Failed to create osg::LOD for given lod handle.", CadKit::FAILED );

  // Add this MatrixTransform to the scene.
  _groupStack->top()->addChild ( mt );

  // Add this LOD to the MatrixTransform.
  mt->addChild ( lod );

  // Put the MatrixTransform into our map.
  (*_groupMap)[part] = mt;

  // Set the client data for this part.
  clientData->setClientData ( part, lod.getValue() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbSgDatabase::endEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbSgDatabase::endEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Get the interface we need from the caller.
  SlQueryPtr<IPartClientData> partClientData ( caller );
  if ( partClientData.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the osg::LOD associated with the lod's parent part.
  SlRefPtr<osg::Object> object = (osg::Object *) ( partClientData->getClientData ( part ) );
  SlRefPtr<osg::LOD> lod = dynamic_cast<osg::LOD *> ( object.getValue() );
  if ( lod.isNull() )
    return ERROR ( "Failed to find lod group to set ranges for.", CadKit::FAILED );

  // If there are no children then just return.
  if ( 0 == lod->getNumChildren() )
  {
    SL_ASSERT ( 0 ); // Should this have happened?
    return true;
  }

  // Get the bounding sphere for the first child.
  const osg::BoundingSphere &boundingSphere = lod->getChild ( 0 )->getBound();

  // The maximum distance for the lod ranges.
  float maxDist = MAX_LOD_DISTANCE_FACTOR * boundingSphere.radius();

  // Set the first range.
  lod->setRange ( 0, 0.0f );

  // Loop through all of the children, starting at the second one.
  unsigned int numChildren = lod->getNumChildren();
  for ( unsigned int i = 1; i < numChildren; ++i )
  {
    // Get the i'th child node.
    osg::Node *child = lod->getChild ( i );
    SL_ASSERT ( child );

    // Set the center of this lod to be the center of the bounding sphere.
    lod->setCenter ( child->getBound().center() );

    // Set the range.
    float dist = ( ( (float) i ) * maxDist ) / ( (float) ( numChildren - 1 ) );
    lod->setRange ( i, dist );
  }

  // There should be one more range value than the number of children.
  lod->setRange ( numChildren, LAST_LOD_RANGE );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbSgDatabase::startEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbSgDatabase::startEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // TODO.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbSgDatabase::endEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbSgDatabase::endEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
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

bool DbSgDatabase::startEntity ( LodHandle lod, IUnknown *caller )
{
  SL_PRINT4 ( "In DbSgDatabase::startEntity(), this = %X, lod = %d, caller = %X\n", this, lod, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Get the interface we need from the caller.
  SlQueryPtr<ILodQuery> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the interface we need from the caller.
  SlQueryPtr<IPartClientData> partClientData ( caller );
  if ( partClientData.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the interface we need from the caller.
  SlQueryPtr<ILodClientData> lodClientData ( caller );
  if ( lodClientData.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Create a Geode.
  SlRefPtr<osg::Geode> geode = new osg::Geode;
  if ( geode.isNull() )
    return ERROR ( "Failed to create osg::Geode for given lod handle.", CadKit::FAILED );

  // Get the osg::LOD associated with the lod's parent part.
  SlRefPtr<osg::Object> object = (osg::Object *) ( partClientData->getClientData ( query->getParent ( lod ) ) );
  SlRefPtr<osg::LOD> osgLod = dynamic_cast<osg::LOD *> ( object.getValue() );
  if ( osgLod.isNull() )
    return ERROR ( "Failed to find lod group to add geode to.", CadKit::FAILED );

  // Add this Geode to the LOD.
  osgLod->addChild ( geode );

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

bool DbSgDatabase::endEntity ( LodHandle lod, IUnknown *caller )
{
  SL_PRINT4 ( "In DbSgDatabase::endEntity(), this = %X, lod = %d, caller = %X\n", this, lod, caller );
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

bool DbSgDatabase::startEntity ( ShapeHandle shape, IUnknown *caller )
{
  SL_PRINT4 ( "In DbSgDatabase::startEntity(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Get the interface we need from the caller.
  SlQueryPtr<IShapeQueryFloatUchar> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the interface we need from the caller.
  SlQueryPtr<ILodClientData> lodClientData ( caller );
  if ( lodClientData.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the osg::Geode we need.
  SlRefPtr<osg::Object> object = (osg::Object *) ( lodClientData->getClientData ( query->getParent ( shape ) ) );
  SlRefPtr<osg::Geode> geode = dynamic_cast<osg::Geode *> ( object.getValue() );
  if ( geode.isNull() )
    return ERROR ( "Failed to find geode to add shape geometry to.", CadKit::FAILED );

  // Create a Geometry drawable.
  SlRefPtr<osg::Geometry> geometry = new osg::Geometry;
  if ( geometry.isNull() )
    return ERROR ( "Failed to create osg::Geometry for given shape handle.", CadKit::FAILED );

  // Create a StateSet.
  SlRefPtr<osg::StateSet> state = new osg::StateSet;
  if ( state.isNull() )
    return ERROR ( "Failed to create osg::StateSet for given shape handle.", CadKit::FAILED );

  // Add the vertices, normals, etc. We have to call this before we add the 
  // attributes (like material) because in OSG, materials take precedence.
  if ( false == this->_addDataSets ( caller, shape, geometry ) )
    return ERROR ( "Failed to add shape sets for given shape.", CadKit::FAILED );

  // Add the material, texture, etc.
  if ( false == this->_addAttributes ( caller, shape, state ) )
    return ERROR ( "Failed to add shape sets for given shape.", CadKit::FAILED );

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

bool DbSgDatabase::_addAttributes ( IUnknown *caller, ShapeHandle shape, osg::StateSet *state )
{
  SL_PRINT5 ( "In DbSgDatabase::_addAttributes(), this = %X, caller = %X, shape = %d, state = %X\n", this, caller, shape, state );
  SL_ASSERT ( caller );
  SL_ASSERT ( state );
  
  // Get the interface we need from the caller.
  SlQueryPtr<IShapeQueryFloatUchar> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // If there are no color attributes then look for a material. We do this 
  // because OSG will use the material first.
  if ( false == this->_hasColorAttribute ( caller, shape ) )
  {
    // Get the material from the shape, part, or assembly.
    SlMaterialf material;
    if ( true == query->getMaterial ( shape, material, true ) )
      CadKit::setMaterial ( material, state );
  }

  // TODO, texture.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's vertices, normals, etc, to the given geometry.
//
///////////////////////////////////////////////////////////////////////////////

bool DbSgDatabase::_addDataSets ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbSgDatabase::_addDataSets(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry );
  
  // Add the vertices. If we don't add any then we failed.
  if ( false == this->_addVertices ( caller, shape, geometry ) )
    return ERROR ( FORMAT ( "Failed to get vertices for shape %X.", shape ), CadKit::FAILED );

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

bool DbSgDatabase::_addVertices ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbSgDatabase::_addVertices(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeVerticesVec3f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the primitive type.
  VertexSetType type;
  if ( false == query->getVertexSetType ( shape, type ) )
    return ERROR ( "Failed to obtain primitive type.", CadKit::FAILED );

  // Should be true.
  SL_ASSERT ( CadKit::UNKNOWN != type );

  // Get the vertices.
  DbSgVertexSetter setter ( type );
  if ( query->getVertices ( shape, setter ) )
  {
    // Set the vertices and the primitives.
    geometry->setVertexArray ( setter.getVertices() );
    geometry->setPrimitiveList ( setter.getPrimitives() );

    // It worked.
    return true;
  }

  // It didn't work.
  return ERROR ( FORMAT ( "Failed to get vertices for shape %X.", shape ), CadKit::FAILED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the shape's normals.
//
///////////////////////////////////////////////////////////////////////////////

bool DbSgDatabase::_addNormals ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbSgDatabase::_addNormals(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeNormalsVec3f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the normal binding.
  VertexBinding binding;
  if ( false == query->getNormalBinding ( shape, binding ) )
    return ERROR ( "Failed to obtain normal binding.", CadKit::FAILED );

  // Get the normals.
  DbSgNormalSetter setter ( binding );
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

bool DbSgDatabase::_addColors ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbSgDatabase::_addColors(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeColorsVec4f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the color binding.
  VertexBinding binding;
  if ( false == query->getColorBinding ( shape, binding ) )
    return ERROR ( "Failed to obtain color binding.", CadKit::FAILED );

  // Get the colors.
  DbSgColorSetter setter ( binding );
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

bool DbSgDatabase::_addTexCoords ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbSgDatabase::_addTexCoords(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeTexCoordsVec2f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the texture coordinates.
  DbSgTextureCoordSetter texCoords;
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
//  See if the given shape has any color attributes.
//
///////////////////////////////////////////////////////////////////////////////

bool DbSgDatabase::_hasColorAttribute ( IUnknown *caller, ShapeHandle shape ) const
{
  SL_PRINT4 ( "In DbSgDatabase::_hasColorAttribute(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeColorsVec4f> query ( caller );
  if ( query.isNull() )
    return false;

  // Get the color binding.
  VertexBinding binding;
  if ( false == query->getColorBinding ( shape, binding ) )
    return false;

  // Does it have colors?
  return BINDING_OFF != binding;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the shape.
//
///////////////////////////////////////////////////////////////////////////////

bool DbSgDatabase::endEntity ( ShapeHandle shape, IUnknown *caller )
{
  SL_PRINT4 ( "In DbSgDatabase::endEntity(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
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

bool DbSgDatabase::startEntity ( SetHandle set, IUnknown *caller )
{
  SL_PRINT4 ( "In DbSgDatabase::startEntity(), this = %X, set = %d, caller = %X\n", this, set, caller );
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

bool DbSgDatabase::endEntity ( SetHandle set, IUnknown *caller )
{
  SL_PRINT4 ( "In DbSgDatabase::endEntity(), this = %X, set = %d, caller = %X\n", this, set, caller );
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

void DbSgDatabase::_pushGroup ( osg::Group *group )
{
  SL_PRINT3 ( "In DbSgDatabase::_pushGroup(), this = %X, group = %X\n", this, group );
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

void DbSgDatabase::_popGroup()
{
  SL_PRINT2 ( "In DbSgDatabase::_popGroup(), this = %X\n", this );
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

osg::Group *DbSgDatabase::_findGroup ( const void *key ) const
{
  SL_PRINT3 ( "In DbSgDatabase::_findGroup(), this = %X, key = %X\n", this, key );
  SL_ASSERT ( NULL != _groupMap.get() );

  // See if it's in our map.
	GroupMap::iterator i = _groupMap->find ( key );

  // Return the result (which may be null).
  return ( i != _groupMap->end() ) ? i->second : NULL;
}
*/
