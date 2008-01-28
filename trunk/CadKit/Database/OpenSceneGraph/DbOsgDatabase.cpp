
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
#include "Interfaces/IUnits.h"
#include "Interfaces/IPrimQuery.h"
#include "Interfaces/IQueryPrimOrigins.h"
#include "Interfaces/IQueryPrimParams.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlQueryPtr.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlMessageIds.h"

#include "../../../Units/Length.h"

#include "osg/LOD"             // Keep these in here because they bring in 
#include "osg/Geode"           // <cmath> which fauls up <math.h> in 
#include "osg/Geometry"        // "Standard/SlTemplateSupport.h"
#include "osg/Shape"
#include "osg/ShapeDrawable"
#include "osgDB/WriteFile"
#include "osgUtil/Optimizer"

#include <limits>

// To help shorten up the lines.
#undef  ERROR
#define ERROR    this->_notifyError
#define PROGRESS this->_notifyProgress
#define WARNING  this->_notifyWarning
#define FORMAT   CadKit::getString

#define LAST_LOD_RANGE            std::numeric_limits<float>::max()
#define MAX_LOD_DISTANCE_FACTOR   30 // TODO, make this a command line argument.

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
  _outputAttribute ( FORMAT_ATTRIBUTE_ASCII ),
  _scaled ( false )
{
  SL_PRINT2 ( "In DbOsgDatabase::DbOsgDatabase(), this = %X\n", this );
  SL_ASSERT ( NULL != _groupStack.get() );

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

bool DbOsgDatabase::dataTransferStart ( IUnknown *caller )
{
  SL_PRINT3 ( "In DbOsgDatabase::dataTransferStart(), this = %X, caller = %X\n", this, caller );

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

IUnknown *DbOsgDatabase::queryInterface ( unsigned long iid )
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
  case IFileExtension::IID:
    return static_cast<IFileExtension *>(this);
  case IDataWrite::IID:
    return static_cast<IDataWrite *>(this);
  case IOutputAttribute::IID:
    return static_cast<IOutputAttribute *>(this);
  case IOutputUnits::IID:
    return static_cast<IOutputUnits *>(this);
  case IPrimNotify::IID:
    return static_cast<IPrimNotify *>(this);
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

  switch ( _outputAttribute )
  {
  case FORMAT_ATTRIBUTE_BINARY:
    return "ive";
  case FORMAT_ATTRIBUTE_ASCII:
    return "osg";
  default:
    SL_ASSERT ( 0 ); // What attribute is this?
    return "osg";
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the format have the attribute?
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::isAttributeSupported ( const FormatAttribute &attribute ) const
{
  SL_PRINT3 ( "In DbOsgDatabase::isAttributeSupported(), this = %X, attribute = %d\n", this, attribute );

  switch ( attribute )
  {
  case FORMAT_ATTRIBUTE_BINARY:
    return true;
  case FORMAT_ATTRIBUTE_ASCII:
    return true;
  default:
    SL_ASSERT ( 0 ); // What format is this?
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the output attribute.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::setOutputAttribute ( const FormatAttribute &attribute )
{
  SL_PRINT3 ( "In DbOsgDatabase::setOutputAttribute(), this = %X, attribute = %d\n", this, attribute );

  // Is it supported?
  if ( false == this->isAttributeSupported ( attribute ) )
    return false;

  // Set the attribute.
  _outputAttribute = attribute;

  // It worked.
  return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the output units.
//
///////////////////////////////////////////////////////////////////////////////

void DbOsgDatabase::setUnits ( const std::string &units )
{
  _outputUnits = units;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::writeData ( const std::string &filename )
{
  SL_PRINT3 ( "In DbOsgDatabase::writeData(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );
  SL_ASSERT ( NULL != this->_getRoot() );
  
  // Perform optimization using environment variable settings.
  osgUtil::Optimizer optimizer;
  optimizer.optimize((osg::Node *) this->_getRoot());

  // Write the root to file. Note: OpenSceneGraph will write binary or ascii 
  // depending on the extension.
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
  
  // Get the units interface from the caller
  float scaleFactor = 1.0f;
  if(!_scaled)
  {
    SlQueryPtr<IUnits> units ( caller );
    if( !units.isNull() )
    {
      try
      {
        SL_PRINT3 ( "Performing units conversion from %s to %s.", units->getUnits(), _outputUnits );
        scaleFactor = Units::Length::convert( units->getUnits(), _outputUnits, 1.0f );
      }
      catch(...)
      {
        WARNING ( "Failed to perform units conversion.", CadKit::UNSUPPORTED );
        scaleFactor = 1.0f;
      }
    }
    _scaled = true;
  }
  
  // Get the interface we need from the caller.
  SlQueryPtr<IAssemblyQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Create a group (really a MatrixTransform).
  SlRefPtr<osg::Group> mt = CadKit::createGroup ( assembly, query.getValue(), (osg::Group *) NULL, scaleFactor );
  if ( mt.isNull() )
    return ERROR ( "Failed to create osg::MatrixTransform for assembly.", CadKit::FAILED );
  
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

bool DbOsgDatabase::endEntity ( AssemblyHandle assembly, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::endEntity(), this = %X, assembly = %X, caller = %X\n", this, assembly, caller );
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

bool DbOsgDatabase::startEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::startEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );
  SL_ASSERT ( false == _groupStack->empty() );

  // Get the interface we need from the caller.
  SlQueryPtr<IPartQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Create a group (really a MatrixTransform).
  SlRefPtr<osg::Group> mt = CadKit::createGroup ( part, query.getValue(), (osg::Group *) NULL );
  if ( mt.isNull() )
    return ERROR ( "Failed to create osg::MatrixTransform for assembly.", CadKit::FAILED );

  // Create a LOD.
  SlRefPtr<osg::LOD> lod ( new osg::LOD );
  if ( lod.isNull() )
    return ERROR ( "Failed to create osg::LOD for given lod handle.", CadKit::FAILED );

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

bool DbOsgDatabase::endEntity ( PartHandle part, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::endEntity(), this = %X, part = %X, caller = %X\n", this, part, caller );
  SL_ASSERT ( caller );

  // Get the osg::Group associated with the part.
  SlRefPtr<osg::Object> object ( (osg::Object *) ( this->getClientData ( part ) ) );
  SlRefPtr<osg::Group> group ( dynamic_cast<osg::Group *> ( object.getValue() ) );
  if ( group.isNull() )
    return ERROR ( "Failed to find group for part.", CadKit::FAILED );

  // Should be true.
  SL_ASSERT ( 1 == group->getNumChildren() );

  // Get the lod from the group.
  SlRefPtr<osg::LOD> lod ( dynamic_cast<osg::LOD *> ( group->getChild ( 0 ) ) );
  if ( lod.isNull() )
    return ERROR ( "Failed to find lod to set ranges for.", CadKit::FAILED );

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

void DbOsgDatabase::_setLodParameters ( osg::LOD *lod ) const
{
  SL_PRINT3 ( "In DbOsgDatabase::_setLodParameters(), this = %X, lod = %X\n", this, lod );

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

  // The minimum of the range we set.
  float rangeMin ( 0.0f );

  // Loop through all of the children except the last one.
  // Note: Unlike previous versions of OSG, there is one LOD "range" for 
  // each LOD "child". Each "range" has a min and max value.
  unsigned int numChildren = lod->getNumChildren();
  for ( unsigned int i = 0; i < numChildren - 1; ++i )
  {
    // Set the range.
    float rangeMax = ( ( (float) i + 1 ) * maxDist ) / ( (float) ( numChildren - 1 ) );
    lod->setRange ( i, rangeMin, rangeMax );
    rangeMin = rangeMax;
  }

  // Set the range for the last child.
  lod->setRange ( numChildren - 1, rangeMin, LAST_LOD_RANGE );
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

  // Get the interface we need from the caller.
  SlQueryPtr<IInstanceQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Find the group associated with the corresponding part or assembly.
  osg::Group *group = this->_findGroup ( instance, query );
  if ( NULL == group )
    return ERROR ( "Failed to find group associated with the corresponding part or assembly.", CadKit::FAILED );

  // Create a group (really a MatrixTransform) by making a shallow copy of 
  // the given group. The new group contains all the same children as the given group.
  SlRefPtr<osg::Group> mt = CadKit::createGroup ( instance, query.getValue(), group );
  if ( mt.isNull() )
    return ERROR ( "Failed to create osg::MatrixTransform for instance.", CadKit::FAILED );

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

bool DbOsgDatabase::endEntity ( InstanceHandle instance, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::endEntity(), this = %X, instance = %X, caller = %X\n", this, instance, caller );
  SL_ASSERT ( caller );

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

  // Get the interface we need from the caller.
  SlQueryPtr<ILodQuery> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Create a Geode.
  SlRefPtr<osg::Geode> geode ( new osg::Geode );
  if ( geode.isNull() )
    return ERROR ( "Failed to create osg::Geode for given lod handle.", CadKit::FAILED );

  // Get the osg::Group associated with the lod's parent part.
  SlRefPtr<osg::Object> object ( (osg::Object *) ( this->getClientData ( query->getParent ( lod ) ) ) );
  SlRefPtr<osg::Group> group ( dynamic_cast<osg::Group *> ( object.getValue() ) );
  if ( group.isNull() )
    return ERROR ( "Failed to find lod to add geode to.", CadKit::FAILED );

  // Should be true.
  SL_ASSERT ( 1 == group->getNumChildren() );

  // The lod should be the only child of the group.
  SlRefPtr<osg::LOD> osgLod ( dynamic_cast<osg::LOD *> ( group->getChild ( 0 ) ) );
  if ( osgLod.isNull() )
    return ERROR ( "Failed to find lod to add geode to.", CadKit::FAILED );

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

bool DbOsgDatabase::endEntity ( LodHandle lod, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::endEntity(), this = %X, lod = %d, caller = %X\n", this, lod, caller );
  SL_ASSERT ( caller );

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

  // Get the interface we need from the caller.
  SlQueryPtr<IShapeQueryFloatUchar> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the osg::Geode we need.
  SlRefPtr<osg::Object> object ( (osg::Object *) ( this->getClientData ( query->getParent ( shape ) ) ) );
  SlRefPtr<osg::Geode> geode ( dynamic_cast<osg::Geode *> ( object.getValue() ) );
  if ( geode.isNull() )
    return ERROR ( "Failed to find geode to add shape geometry to.", CadKit::FAILED );

  // Create a Geometry drawable.
  SlRefPtr<osg::Geometry> geometry ( new osg::Geometry );
  if ( geometry.isNull() )
    return ERROR ( "Failed to create osg::Geometry for given shape handle.", CadKit::FAILED );

  // Create a StateSet.
  SlRefPtr<osg::StateSet> state ( new osg::StateSet );
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

bool DbOsgDatabase::_addAttributes ( IUnknown *caller, ShapeHandle shape, osg::StateSet *state )
{
  SL_PRINT5 ( "In DbOsgDatabase::_addAttributes(), this = %X, caller = %X, shape = %d, state = %X\n", this, caller, shape, state );
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

bool DbOsgDatabase::_addDataSets ( IUnknown *caller, ShapeHandle shape, osg::Geometry *geometry )
{
  SL_PRINT5 ( "In DbOsgDatabase::_addDataSets(), this = %X, caller = %X, shape = %d, geometry = %X\n", this, caller, shape, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry );
  
  // Add the vertices. If we don't add any then we failed.
  if(true == this->_addVertices ( caller, shape, geometry ))
  {
	// Add the normals, colors, and texture coordinates, if there are any.
	// It is ok if these fail.
	this->_addNormals   ( caller, shape, geometry );
	this->_addColors    ( caller, shape, geometry );
	this->_addTexCoords ( caller, shape, geometry );
  }

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
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the primitive type.
  VertexSetType type;
  if ( false == query->getVertexSetType ( shape, type ) )
    return ERROR ( "Failed to obtain primitive type.", CadKit::FAILED );

  // Should be true.
  SL_ASSERT ( CadKit::UNKNOWN != type );

  // Get the vertices.
  DbOsgVertexSetter setter ( type );
  if ( query->getVertices ( shape, setter ) )
  {
    // Set the vertices and the primitives.
    geometry->setVertexArray ( setter.getVertices() );
    geometry->setPrimitiveSetList ( setter.getPrimitives() );

    // It worked.
    return true;
  }

  // It didn't work.
  return WARNING ( FORMAT ( "Failed to get vertices for shape %X.", shape ), CadKit::FAILED );
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
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the normal binding.
  VertexBinding binding;
  if ( false == query->getNormalBinding ( shape, binding ) )
    return ERROR ( "Failed to obtain normal binding.", CadKit::FAILED );

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
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the color binding.
  VertexBinding binding;
  if ( false == query->getColorBinding ( shape, binding ) )
    return ERROR ( "Failed to obtain color binding.", CadKit::FAILED );

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
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

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

  // Nothing to do.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the primitive.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::startEntity ( PrimHandle prim, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::startEntity(), this = %X, prim = %d, caller = %X\n", this, prim, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<IPrimQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the osg::Geode we need.
  SlRefPtr<osg::Object> object ( (osg::Object *) ( this->getClientData ( query->getParent ( prim ) ) ) );
  SlRefPtr<osg::Geode> geode ( dynamic_cast<osg::Geode *> ( object.getValue() ) );
  if ( geode.isNull() )
    return ERROR ( "Failed to find geode to add primitive geometry to.", CadKit::FAILED );

  // Create a Geometry drawable.
  SlRefPtr<osg::ShapeDrawable> drawable ( new osg::ShapeDrawable );
  if ( drawable.isNull() )
    return ERROR ( "Failed to create osg::ShapeDrawable for given primitive handle.", CadKit::FAILED );

  // Create a StateSet.
  SlRefPtr<osg::StateSet> state ( new osg::StateSet );
  if ( state.isNull() )
    return ERROR ( "Failed to create osg::StateSet for given primitive handle.", CadKit::FAILED );

  // Add the osg::Shape node to represent this primitive. We have to call this before we add the 
  // attributes (like material) because in OSG, materials take precedence.
  if ( false == this->_addPrimitive ( caller, prim, drawable ) )
    return ERROR ( "Failed to add osg shape for given primitive.", CadKit::FAILED );
  
  // add color to the primitive
  _addColors( caller, prim, drawable );

  // Add the material, texture, etc.
  if ( false == this->_addAttributes ( caller, prim, state ) )
    return ERROR ( "Failed to add attributes for given primitive.", CadKit::FAILED );

  // Set the state of the geometry.
  drawable->setStateSet ( state );

  // Add the Geometry to the Geode.
  geode->addDrawable ( drawable );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the primitive to the scene.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::_addPrimitive  ( IUnknown *caller, PrimHandle prim, osg::ShapeDrawable *drawable )
{
  SL_PRINT5 ( "In DbOsgDatabase::_addPrimitive(), this = %X, caller = %X, primitive = %d, geometry = %X\n", this, caller, prim, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( drawable );

  // Get the interfaces we need from the caller.
  SlQueryPtr<IQueryPrimOriginsVec3f> originQuery ( caller );
  if ( originQuery.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  SlQueryPtr<IQueryPrimParamsFloat> paramQuery ( caller );
  if ( paramQuery.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the primitive type.
  PrimitiveType type;
  if ( false == originQuery->getPrimType ( prim, type ) )
    return ERROR ( "Failed to obtain primitive type.", CadKit::FAILED );

  // Should be true.
  SL_ASSERT ( CadKit::UNKNOWN_PRIM != type );

  // Get the origins.
  DbOsgPrimOriginSetter originSetter( type );
  if ( !originQuery->getPrimOrigins ( prim, originSetter ) )
    return ERROR ( "Failed to obtain primitive origins.", CadKit::FAILED );
  
  // Get the parameters
  DbOsgPrimParamSetter paramSetter( type );
  if ( !paramQuery->getPrimParams ( prim, paramSetter ) )
    return ERROR ( "Failed to obtain primitive parameters.", CadKit::FAILED );
  
  if ( type == CadKit::BOX )
  {
    return WARNING ( FORMAT ( "Box primitive not supported." ), CadKit::FAILED );
  }
  else if ( type == CadKit::CYLINDER )
  {
    osg::Vec3f orientationVec(paramSetter.getPrimParams()->at(0), 
                              paramSetter.getPrimParams()->at(1), 
                              paramSetter.getPrimParams()->at(2));
    osg::Vec3f startVec(0.0f, 0.0f, 1.0f);
    osg::Quat cylRot;
    cylRot.makeRotate(startVec, orientationVec);
    float cylLength = orientationVec.length();
    osg::Vec3f cylCenter(originSetter.getPrimOrigins()->at(0));
    cylCenter[1] += cylLength * 0.5f;
   
    osg::ref_ptr<osg::Cylinder> cyl = new osg::Cylinder;
    cyl->setCenter(cylCenter);
    cyl->setHeight(cylLength);
    cyl->setRotation(cylRot);
    cyl->setRadius(paramSetter.getPrimParams()->at(3)); // NOTE: source supports two radii for creating cones

    drawable->setShape(cyl.get());
    return true;
  }
  else if ( type == CadKit::PYRAMID )
  {
    return WARNING ( FORMAT ( "Pyramid primitive not supported." ), CadKit::FAILED );
  }
  else if ( type == CadKit::SPHERE )
  {
    return WARNING ( FORMAT ( "Sphere primitive not supported." ), CadKit::FAILED );
  }
  else if ( type == CadKit::TRIPRISM )
  {
    return WARNING ( FORMAT ( "TriPrism primitive not supported." ), CadKit::FAILED );
  }

  // It didn't work.
  return WARNING ( FORMAT ( "Failed to get data for primitive %X.", prim ), CadKit::FAILED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the primitive's color(s).
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::_addColors ( IUnknown *caller, PrimHandle prim, osg::ShapeDrawable *drawable )
{
  SL_PRINT5 ( "In DbOsgDatabase::_addColors(), this = %X, caller = %X, primitive = %d, drawable = %X\n", this, caller, prim, drawable );
  SL_ASSERT ( caller );
  SL_ASSERT ( drawable );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryPrimColorsVec4f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the colors.
  DbOsgPrimColorSetter setter;
  if ( query->getColors ( prim, setter ) )
  {
    // Set the color.
    drawable->setColor( setter.getColors()->at(0) );

    // It worked.
    return true;
  }

  // It didn't work. Don't report an error, there may not be any.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the primitive's attributes to the given state.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::_addAttributes ( IUnknown *caller, PrimHandle prim, osg::StateSet *state )
{
  SL_PRINT5 ( "In DbOsgDatabase::_addAttributes(), this = %X, caller = %X, shape = %d, primitive = %X\n", this, caller, prim, state );
  SL_ASSERT ( caller );
  SL_ASSERT ( state );
  
  // Get the interface we need from the caller.
  SlQueryPtr<IPrimQueryFloat> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // If there are no color attributes then look for a material. We do this 
  // because OSG will use the material first.
  if ( false == this->_hasColorAttribute ( caller, prim ) )
  {
    // Get the material from the shape, part, or assembly.
    SlMaterialf material;
    if ( true == query->getMaterial ( prim, material, true ) )
      CadKit::setMaterial ( material, state );
  }

  // TODO, texture.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given primitive has any color attributes.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::_hasColorAttribute ( IUnknown *caller, PrimHandle prim ) const
{
  SL_PRINT4 ( "In DbOsgDatabase::_hasColorAttribute(), this = %X, primitive = %d, caller = %X\n", this, prim, caller );
  SL_ASSERT ( caller );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryPrimColorsVec4f> query ( caller );
  if ( query.isNull() )
    return false;

  // Get the color binding.
  VertexBinding binding;
  if ( false == query->getColorBinding ( prim, binding ) )
    return false;

  // Does it have colors?
  return BINDING_OFF != binding;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End the primitive.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::endEntity ( PrimHandle prim, IUnknown *caller )
{
  SL_PRINT4 ( "In DbOsgDatabase::endEntity(), this = %X, prim = %d, caller = %X\n", this, prim, caller );
  SL_ASSERT ( caller );

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
//  Find the group associated with the instance's part or assembly.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *DbOsgDatabase::_findGroup ( InstanceHandle instance, IInstanceQueryFloat *query ) const
{
  SL_PRINT4 ( "In DbOsgDatabase::_findGroup(), this = %X, instance = %X, query = %X\n", this, instance, query );
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
