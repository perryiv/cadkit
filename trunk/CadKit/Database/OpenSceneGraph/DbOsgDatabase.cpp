
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

#define LAST_LOD_RANGE            1e7
#define MAX_LOD_DISTANCE_FACTOR   20

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( DbOsgDatabase, DbBaseTarget );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( DbOsgDatabase, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbOsgDatabase::DbOsgDatabase() : DbBaseTarget(),
  _groupStack ( new GroupStack ),
  _groupMap ( new GroupMap )
{
  SL_PRINT2 ( "In DbOsgDatabase::DbOsgDatabase(), this = %X\n", this );
  SL_ASSERT ( NULL != _groupStack.get() );
  SL_ASSERT ( NULL != _groupMap.get() );

  // Push a new group onto stack.
  this->_pushGroup ( new osg::Group );
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
  SL_ASSERT ( 1 == _groupStack->size() );

  // If the client may have referenced the root.
  SL_ASSERT ( this->_getRoot()->referenceCount() >= 1 );

  // Pop the root off the stack.
  this->_popGroup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell the target it is about to receive data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::dataTransferStart ( IUnknown *caller )
{
  SL_PRINT3 ( "In DbOsgDatabase::dataTransferStart(), this = %X, caller = %X\n", this, caller );

  // Should be true.
  SL_ASSERT ( 1 == _groupStack->size() );

  // Clear the group map.
  _groupMap->clear();

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

bool DbOsgDatabase::dataTransferEnd ( IUnknown *caller )
{
  SL_PRINT3 ( "In DbOsgDatabase::dataTransferEnd(), this = %X, caller = %X\n", this, caller );

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

void DbOsgDatabase::_clearGroupStack()
{
  SL_PRINT2 ( "In DbOsgDatabase::_clearGroupStack(), this = %X\n", this );

  // Pop all the groups off the stack.
  while ( _groupStack->size() )
    this->_popGroup();
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
  SL_ASSERT ( NULL != this->_getRoot() );

  // Write the root to file.
  return osgDB::writeNodeFile ( *(this->_getRoot()), filename );
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

  // Get the interface we need from the caller.
  SlQueryPtr<IAssemblyQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Create a group (really a MatrixTransform).
  SlRefPtr<osg::Group> mt = CadKit::createGroup ( assembly, query.getValue() );
  if ( mt.isNull() )
    return ERROR ( "Failed to create osg::MatrixTransform for assembly.", NO_INTERFACE );

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

  // Get the interface we need from the caller.
  SlQueryPtr<IPartQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the interface we need from the caller.
  SlQueryPtr<IPartClientData> clientData ( caller );
  if ( clientData.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Create a group (really a MatrixTransform).
  SlRefPtr<osg::Group> mt = CadKit::createGroup ( part, query.getValue() );
  if ( mt.isNull() )
    return ERROR ( "Failed to create osg::MatrixTransform for assembly.", NO_INTERFACE );

  // Create a LOD group.
  SlRefPtr<osg::LOD> lod = new osg::LOD;
  if ( lod.isNull() )
    return ERROR ( "Failed to create osg::LOD for given lod handle.", FAILED );

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

bool DbOsgDatabase::endEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::endEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Get the interface we need from the caller.
  SlQueryPtr<IPartClientData> partClientData ( caller );
  if ( partClientData.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the osg::LOD associated with the lod's parent part.
  SlRefPtr<osg::Object> object = (osg::Object *) ( partClientData->getClientData ( part ) );
  SlRefPtr<osg::LOD> lod = dynamic_cast<osg::LOD *> ( object.getValue() );
  if ( lod.isNull() )
    return ERROR ( "Failed to find lod group to set ranges for.", FAILED );

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

bool DbOsgDatabase::startEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::startEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Get the interface we need from the caller.
  SlQueryPtr<IInstanceQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Create a group (really a MatrixTransform).
  SlRefPtr<osg::Group> mt = CadKit::createGroup ( instance, query.getValue() );
  if ( mt.isNull() )
    return ERROR ( "Failed to create osg::MatrixTransform for instance.", NO_INTERFACE );

  // Get the corresponding part or assembly.
  HierarchyHandle hierarchy = query->getCorresponding ( instance );
  if ( NULL == hierarchy )
    return ERROR ( "Failed to get corresponding part or assembly from the instance.", FAILED );

  // Call this function to handle the LODs.
//  if ( false == CadKit::createInstanceLods() )
//    return ERROR ( "Failed to process (and create) LOD(s) for instance.", NO_INTERFACE );

/*
  HERE

  If the hierarchy is a part, then the corresponding group should hold only 
  one child (an osg::LOD). You need to make another osg::LOD and attach all
  the children from the part's osg::LOD, and add the same ranges. However,
  you also have to calculate the bounding sphere's and set the new osg::LOD's
  center's (they will be different than the corresponding part because of the
  different transformation.

  If the hierarchy is an assembly then you have to traverse that assembly's
  osg::MatrixTransform and find all of the osg::LODs. For each one of these
  osg::LODs you have to do something similar to what is described above for
  a part. Probably, you'll want to make a copy of the assembly's scene down
  to the osg::LODs. Copy the osg::LODs but set different centers. Then just
  add the same children of the original osg::LOD to the new osg::LOD. You
  will probably need a recursive function for this.
*/
  // Find the group that corresponds with this part or assembly.
  SlRefPtr<osg::Group> original = dynamic_cast<osg::Group *> ( this->_findGroup ( hierarchy ) );
  if ( original.isNull() )
    return ERROR ( "Failed to find group associated with instance's corresponding part or assembly.", FAILED );

  // Should be true (as of when I type this).
  SL_ASSERT ( 1 == original->getNumChildren() );
  SL_ASSERT ( NULL != dynamic_cast<osg::LOD *> ( original->getChild ( 0 ) ) );

  // Make a new lod.

  // Add all of the original group's children to the instance's group.
  unsigned int numChildren = original->getNumChildren();
  for ( unsigned int i = 0; i < numChildren; ++i )
    mt->addChild ( original->getChild ( i ) );

  // Add this group to the scene.
  _groupStack->top()->addChild ( mt );

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

  // Create a Geode.
  SlRefPtr<osg::Geode> geode = new osg::Geode;
  if ( geode.isNull() )
    return ERROR ( "Failed to create osg::Geode for given lod handle.", FAILED );

  // Get the osg::LOD associated with the lod's parent part.
  SlRefPtr<osg::Object> object = (osg::Object *) ( partClientData->getClientData ( query->getParent ( lod ) ) );
  SlRefPtr<osg::LOD> osgLod = dynamic_cast<osg::LOD *> ( object.getValue() );
  if ( osgLod.isNull() )
    return ERROR ( "Failed to find lod group to add geode to.", FAILED );

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

  // Create a StateSet.
  SlRefPtr<osg::StateSet> state = new osg::StateSet;
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

bool DbOsgDatabase::_addAttributes ( IUnknown *caller, ShapeHandle shape, osg::StateSet *state )
{
  SL_PRINT5 ( "In DbOsgDatabase::_addAttributes(), this = %X, caller = %X, shape = %d, state = %X\n", this, caller, shape, state );
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
//  See if the given shape has any color attributes.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::_hasColorAttribute ( IUnknown *caller, ShapeHandle shape ) const
{
  SL_PRINT4 ( "In DbOsgDatabase::_hasColorAttribute(), this = %X, shape = %d, caller = %X\n", this, shape, caller );
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
//  Get the root.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *DbOsgDatabase::_getRoot() const
{
  SL_PRINT2 ( "In DbOsgDatabase::_getRoot(), this = %X\n", this );

  // If the stack is empty then the root is null.
  return ( _groupStack->size() ) ? _groupStack->top() : NULL;
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

  // Pop it off our stack.
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
