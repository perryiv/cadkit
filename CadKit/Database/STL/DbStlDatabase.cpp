
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbStlDatabase: STL database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbStlPrecompiled.h"
#include "DbStlDatabase.h"
#include "DbStlFunctions.h"
#include "DbStlInline.h"
#include "DbStlArraySetters.h"

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

#define LAST_LOD_RANGE            1e7
#define MAX_LOD_DISTANCE_FACTOR   30

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( DbStlDatabase, DbBaseTarget );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( DbStlDatabase, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbStlDatabase::DbStlDatabase() : DbBaseTarget(),
  _groupStack ( new GroupStack )
{
  SL_PRINT2 ( "In DbStlDatabase::DbStlDatabase(), this = %X\n", this );
  SL_ASSERT ( NULL != _groupStack.get() );

  // Push a new group onto stack.
  this->_pushGroup ( new osg::Group );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbStlDatabase::~DbStlDatabase()
{
  SL_PRINT2 ( "In DbStlDatabase::~DbStlDatabase(), this = %X\n", this );

  // The stack should just be holding the root.
  SL_ASSERT ( 1 == _groupStack->size() );

  // The client may have referenced the root.
  SL_ASSERT ( this->_getRoot()->referenceCount() >= 1 );

  // Pop the root off the stack.
  this->_popGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell the target it is about to receive data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::dataTransferStart ( IUnknown *caller )
{
  SL_PRINT3 ( "In DbStlDatabase::dataTransferStart(), this = %X, caller = %X\n", this, caller );

  // Should be true.
  SL_ASSERT ( 1 == _groupStack->size() );

  // Clear the group stack.
  this->_clearGroupStack();

  // Push a group onto the stack.
  this->_pushGroup ( new osg::Group );

  // Clear the client-data maps.
  this->_clearClientDataMaps();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell the target it is done receiving data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::dataTransferEnd ( IUnknown *caller )
{
  SL_PRINT3 ( "In DbStlDatabase::dataTransferEnd(), this = %X, caller = %X\n", this, caller );

  // Should be true.
  SL_ASSERT ( 1 == _groupStack->size() );
  SL_ASSERT ( this->_getRoot()->referenceCount() >= 1 );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the group stack.
//
///////////////////////////////////////////////////////////////////////////////

void DbStlDatabase::_clearGroupStack()
{
  SL_PRINT2 ( "In DbStlDatabase::_clearGroupStack(), this = %X\n", this );

  // Pop all the groups off the stack.
  while ( _groupStack->size() )
    this->_popGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbStlDatabase::queryInterface ( const unsigned long &iid )
{
  SL_PRINT2 ( "In DbStlDatabase::queryInterface(), this = %X\n", this );

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

std::string DbStlDatabase::getFileExtension() const
{
  SL_PRINT2 ( "In DbStlDatabase::getFileExtension(), this = %X\n", this );
  return "stl";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Store the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::storeData ( const std::string &filename )
{
  SL_PRINT3 ( "In DbStlDatabase::storeData(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );
  SL_ASSERT ( NULL != this->_getRoot() );

  // Write the root to file.
  return osgDB::writeNodeFile ( *(this->_getRoot()), filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::startEntity ( AssemblyHandle assembly, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::startEntity(), this = %X, assembly = %X, caller = %X\n", this, assembly, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Get the interface we need from the caller.
  SlQueryPtr<IAssemblyQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Create a group (really a MatrixTransform).
  SlRefPtr<osg::Group> mt = CadKit::createGroup ( assembly, query.getValue(), (osg::Group *) NULL );
  if ( mt.isNull() )
    return ERROR ( "Failed to create osg::MatrixTransform for assembly.", FAILED );

  // Add this group to the scene.
  _groupStack->top()->addChild ( mt );

  // Make it the new current group.
  this->_pushGroup ( mt );

  // Set the client data for this assembly.
  this->setClientData ( assembly, mt.getValue() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::endEntity ( AssemblyHandle assembly, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::endEntity(), this = %X, assembly = %X, caller = %X\n", this, assembly, caller );
  SL_ASSERT ( caller );

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

bool DbStlDatabase::startEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::startEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Get the interface we need from the caller.
  SlQueryPtr<IPartQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Create a group (really a MatrixTransform).
  SlRefPtr<osg::Group> mt = CadKit::createGroup ( part, query.getValue(), (osg::Group *) NULL );
  if ( mt.isNull() )
    return ERROR ( "Failed to create osg::MatrixTransform for assembly.", FAILED );

  // Create a LOD.
  SlRefPtr<osg::LOD> lod ( new osg::LOD );
  if ( lod.isNull() )
    return ERROR ( "Failed to create osg::LOD for given lod handle.", FAILED );

  // Add this MatrixTransform to the scene.
  _groupStack->top()->addChild ( mt );

  // Add this LOD to the MatrixTransform.
  mt->addChild ( lod );

  // Set the client data for this part.
  this->setClientData ( part, mt.getValue() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::endEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::endEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );

  // Get the osg::Group associated with the part.
  SlRefPtr<osg::Object> object ( (osg::Object *) ( this->getClientData ( part ) ) );
  SlRefPtr<osg::Group> group ( dynamic_cast<osg::Group *> ( object.getValue() ) );
  if ( group.isNull() )
    return ERROR ( "Failed to find group for part.", FAILED );

  // Should be true.
  SL_ASSERT ( 1 == group->getNumChildren() );

  // Get the lod from the group.
  SlRefPtr<osg::LOD> lod ( dynamic_cast<osg::LOD *> ( group->getChild ( 0 ) ) );
  if ( lod.isNull() )
    return ERROR ( "Failed to find lod to set ranges for.", FAILED );

  // Set the lod center and ranges.
  this->_setLodParameters ( lod );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lod center and ranges.
//
///////////////////////////////////////////////////////////////////////////////

void DbStlDatabase::_setLodParameters ( osg::LOD *lod ) const
{
  SL_PRINT3 ( "In DbStlDatabase::_setLodParameters(), this = %X, lod = %X\n", this, lod );

  // If there are no children then just return. Do not assert, this has 
  // happened with some big files.
  if ( 0 == lod->getNumChildren() )
    return;

  // Get the first child.
  osg::Node *child = lod->getChild ( 0 );

  // Get the bounding sphere for the first child.
  const osg::BoundingSphere &boundingSphere = child->getBound();
  SL_VERIFY ( boundingSphere.radius() > 0.0f );

  // The maximum distance for the lod ranges.
  float maxDist = MAX_LOD_DISTANCE_FACTOR * boundingSphere.radius();

  // Get the center of the bounding sphere.
  const osg::Vec3 &center = boundingSphere.center();

  // Set the center of this lod to be the center of the bounding sphere.
  lod->setCenter ( center );

  // Set the first range.
  lod->setRange ( 0, 0.0f );

  // Loop through all of the children, starting at the second one.
  unsigned int numChildren = lod->getNumChildren();
  for ( unsigned int i = 1; i < numChildren; ++i )
  {
    // Get the i'th child node.
    child = lod->getChild ( i );
    SL_ASSERT ( child );

    // As of now, this should be true. However, it isn't important to the 
    // algorithm. Take this out if/when this is no longer true.
    SL_ASSERT ( NULL != dynamic_cast<osg::Geode *> ( child ) );

    // Set the range.
    float dist = ( ( (float) i ) * maxDist ) / ( (float) ( numChildren - 1 ) );
    lod->setRange ( i, dist );
  }

  // There should be one more range value than the number of children.
  lod->setRange ( numChildren, LAST_LOD_RANGE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::startEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::startEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<IInstanceQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Find the group associated with the corresponding part or assembly.
  osg::Group *group = this->_findGroup ( instance, query );
  if ( NULL == group )
    return ERROR ( "Failed to find group associated with the corresponding part or assembly.", FAILED );

  // Create a group (really a MatrixTransform) by making a shallow copy of 
  // the given group. The new group contains all the same children as the given group.
  SlRefPtr<osg::Group> mt = CadKit::createGroup ( instance, query.getValue(), group );
  if ( mt.isNull() )
    return ERROR ( "Failed to create osg::MatrixTransform for instance.", FAILED );

  // Add this MatrixTransform to the scene.
  _groupStack->top()->addChild ( mt );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::endEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::endEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the lod.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::startEntity ( LodHandle lod, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::startEntity(), this = %X, lod = %d, caller = %X\n", this, lod, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<ILodQuery> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Create a Geode.
  SlRefPtr<osg::Geode> geode ( new osg::Geode );
  if ( geode.isNull() )
    return ERROR ( "Failed to create osg::Geode for given lod handle.", FAILED );

  // Get the osg::Group associated with the lod's parent part.
  SlRefPtr<osg::Object> object ( (osg::Object *) ( this->getClientData ( query->getParent ( lod ) ) ) );
  SlRefPtr<osg::Group> group ( dynamic_cast<osg::Group *> ( object.getValue() ) );
  if ( group.isNull() )
    return ERROR ( "Failed to find lod to add geode to.", FAILED );

  // Should be true.
  SL_ASSERT ( 1 == group->getNumChildren() );

  // The lod should be the only child of the group.
  SlRefPtr<osg::LOD> osgLod ( dynamic_cast<osg::LOD *> ( group->getChild ( 0 ) ) );
  if ( osgLod.isNull() )
    return ERROR ( "Failed to find lod to add geode to.", FAILED );

  // Add this Geode to the LOD.
  osgLod->addChild ( geode );

  // Set the client data for this lod.
  this->setClientData ( lod, geode.getValue() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the lod.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::endEntity ( LodHandle lod, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::endEntity(), this = %X, lod = %d, caller = %X\n", this, lod, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the shape.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::startEntity ( ShapeHandle shape, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::startEntity(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<IShapeQueryFloatUchar> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the osg::Geode we need.
  SlRefPtr<osg::Object> object ( (osg::Object *) ( this->getClientData ( query->getParent ( shape ) ) ) );
  SlRefPtr<osg::Geode> geode ( dynamic_cast<osg::Geode *> ( object.getValue() ) );
  if ( geode.isNull() )
    return ERROR ( "Failed to find geode to add shape geometry to.", FAILED );

  // Create a Geometry drawable.
  SlRefPtr<osg::Geometry> geometry ( new osg::Geometry );
  if ( geometry.isNull() )
    return ERROR ( "Failed to create osg::Geometry for given shape handle.", FAILED );

  // Create a StateSet.
  SlRefPtr<osg::StateSet> state ( new osg::StateSet );
  if ( state.isNull() )
    return ERROR ( "Failed to create osg::StateSet for given shape handle.", FAILED );

  // Add the vertices, normals, etc. We have to call this before we add the 
  // attributes (like material) because in OSG, materials take precedence.
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

bool DbStlDatabase::_addAttributes ( IUnknown *caller, ShapeHandle shape, osg::StateSet *state )
{
  SL_PRINT5 ( "In DbStlDatabase::_addAttributes(), this = %X, caller = %X, shape = %d, state = %X\n", this, caller, shape, state );
  SL_ASSERT ( caller );
  SL_ASSERT ( state );
  
  // Get the interface we need from the caller.
  SlQueryPtr<IShapeQueryFloatUchar> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

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

bool DbStlDatabase::_addDataSets ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbStlDatabase::_addDataSets(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
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

bool DbStlDatabase::_addVertices ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbStlDatabase::_addVertices(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeVerticesVec3f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the primitive type.
  VertexSetType type;
  if ( false == query->getVertexSetType ( shape, type ) )
    return ERROR ( "Failed to obtain primitive type.", FAILED );

  // Should be true.
  SL_ASSERT ( CadKit::UNKNOWN != type );

  // Get the vertices.
  DbStlVertexSetter setter ( type );
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

bool DbStlDatabase::_addNormals ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbStlDatabase::_addNormals(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
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
  DbStlNormalSetter setter ( binding );
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

bool DbStlDatabase::_addColors ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbStlDatabase::_addColors(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
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
  DbStlColorSetter setter ( binding );
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

bool DbStlDatabase::_addTexCoords ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbStlDatabase::_addTexCoords(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeTexCoordsVec2f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the texture coordinates.
  DbStlTextureCoordSetter texCoords;
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

bool DbStlDatabase::_hasColorAttribute ( IUnknown *caller, ShapeHandle shape ) const
{
  SL_PRINT4 ( "In DbStlDatabase::_hasColorAttribute(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
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

bool DbStlDatabase::endEntity ( ShapeHandle shape, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::endEntity(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the set.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::startEntity ( SetHandle set, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::startEntity(), this = %X, set = %d, caller = %X\n", this, set, caller );
  SL_ASSERT ( caller );

  // We ignore this because we build the geometry in the shape function above.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the set.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlDatabase::endEntity ( SetHandle set, IUnknown *caller )
{
  SL_PRINT4 ( "In DbStlDatabase::endEntity(), this = %X, set = %d, caller = %X\n", this, set, caller );
  SL_ASSERT ( caller );

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the group.
//
///////////////////////////////////////////////////////////////////////////////

void DbStlDatabase::_pushGroup ( osg::Group *group )
{
  SL_PRINT3 ( "In DbStlDatabase::_pushGroup(), this = %X, group = %X\n", this, group );
  SL_ASSERT ( group );

  // Push it onto our stack.
  _groupStack->push ( group );

  // Reference it.
  group->ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the root.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *DbStlDatabase::_getRoot() const
{
  SL_PRINT2 ( "In DbStlDatabase::_getRoot(), this = %X\n", this );

  // If the stack is empty then the root is null.
  return ( _groupStack->size() ) ? _groupStack->top() : NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the group.
//
///////////////////////////////////////////////////////////////////////////////

void DbStlDatabase::_popGroup()
{
  SL_PRINT2 ( "In DbStlDatabase::_popGroup(), this = %X\n", this );
  SL_ASSERT ( false == _groupStack->empty() );

  // Unreference the top one.
  _groupStack->top()->unref();

  // Pop it off our stack.
  _groupStack->pop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the group associated with the instance's part or assembly.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *DbStlDatabase::_findGroup ( InstanceHandle instance, IInstanceQueryFloat *query ) const
{
  SL_PRINT4 ( "In DbStlDatabase::_findGroup(), this = %X, instance = %X, query = %X\n", this, instance, query );
  SL_ASSERT ( query );

  // See if this instance is a part.
  PartHandle part = query->getCorrespondingPart ( instance );
  if ( NULL != part )
    return dynamic_cast<osg::Group *> ( (osg::Object *) ( this->getClientData ( part ) ) );

  // See if this instance is an assembly.
  AssemblyHandle assembly = query->getCorrespondingAssembly ( instance );
  if ( NULL != assembly )
    return dynamic_cast<osg::Group *> ( (osg::Object *) ( this->getClientData ( assembly ) ) );

  // It didn't work.
  return NULL;
}
