
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

#include "../../../Usul/Units/Length.h"

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
  int numPasses = 1;

  char *passes = getenv("OSG_OPTIMIZER_PASSES");
  
  if(passes)
    numPasses = atoi(passes);

  osgUtil::Optimizer optimizer;

  for(int i = 0; i < numPasses; i++)
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
  
  float scaleFactor = 1.0f;

  // If output units are specified, get the units interface from the caller
  // and attempt to get a scale factor.
  if( !_scaled && !_outputUnits.empty() )
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
  
  // Determine the number of sets from the number of origins.

  // Get the origin interface from the caller.
  SlQueryPtr<IQueryPrimOriginsVec3f> originQuery ( caller );
  if ( originQuery.isNull() )
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

  int numSets = originSetter.getPrimOrigins()->getNumElements();

  for(int i = 0; i < numSets; i++)
  {
    // Create a StateSet.
    SlRefPtr<osg::StateSet> state ( new osg::StateSet );
    if ( state.isNull() )
      return ERROR ( "Failed to create osg::StateSet for given primitive handle.", CadKit::FAILED );

    // Add the material, texture, etc.
    if ( false == this->_addAttributes ( caller, prim, state ) )
      return ERROR ( "Failed to add attributes for given primitive.", CadKit::FAILED );
    
    // Add the osg::Shape node to represent this primitive. We have to call this before we add the 
    // attributes (like material) because in OSG, materials take precedence.
    if ( false == this->_addPrimitiveSet ( caller, prim, geode, state, type, i ) )
      return ERROR ( "Failed to add osg shape for given primitive.", CadKit::FAILED );
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the primitive to the scene.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::_addPrimitiveSet  ( IUnknown *caller, PrimHandle prim, osg::Geode *geode, osg::StateSet *state, PrimitiveType type, int set )
{
  SL_PRINT7 ( "In DbOsgDatabase::_addPrimitive(), this = %X, caller = %X, primitive = %d, geode = %X, type = %d, set = %d\n", this, caller, prim, geode, type, set );
  SL_ASSERT ( caller );
  SL_ASSERT ( geode );

  // Get the interfaces we need from the caller.
  SlQueryPtr<IQueryPrimOriginsVec3f> originQuery ( caller );
  if ( originQuery.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  SlQueryPtr<IQueryPrimParamsFloat> paramQuery ( caller );
  if ( paramQuery.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the origins.
  DbOsgPrimOriginSetter originSetter( type );
  if ( !originQuery->getPrimOrigins ( prim, originSetter ) )
    return ERROR ( "Failed to obtain primitive origins.", CadKit::FAILED );
  
  // Get the parameters
  DbOsgPrimParamSetter paramSetter( type );
  if ( !paramQuery->getPrimParams ( prim, paramSetter ) )
    return ERROR ( "Failed to obtain primitive parameters.", CadKit::FAILED );
  
  int tmp;

  if ( type == CadKit::BOX )
  {
    // Create a shape drawable.
    SlRefPtr<osg::ShapeDrawable> boxDrawable ( new osg::ShapeDrawable );
    if ( boxDrawable.isNull() )
      return ERROR ( "Failed to create osg::ShapeDrawable for box.", CadKit::FAILED );
    
    // 9 params define x, y, and z vectors
    tmp = set * 9;
    float xBox = osg::Vec3f(paramSetter.getPrimParams()->at(tmp), 
                             paramSetter.getPrimParams()->at(tmp+1), 
                             paramSetter.getPrimParams()->at(tmp+2)).length();
    float yBox = osg::Vec3f(paramSetter.getPrimParams()->at(tmp+3), 
                             paramSetter.getPrimParams()->at(tmp+4), 
                             paramSetter.getPrimParams()->at(tmp+5)).length();
    float zBox = osg::Vec3f(paramSetter.getPrimParams()->at(tmp+6), 
                             paramSetter.getPrimParams()->at(tmp+7), 
                             paramSetter.getPrimParams()->at(tmp+8)).length();
    
    // Origin is box lower-left corner, so offset to get the center for OSG
    osg::Vec3f boxCenter(originSetter.getPrimOrigins()->at(set));
    boxCenter[0] += xBox * 0.5;
    boxCenter[1] += yBox * 0.5;
    boxCenter[2] += zBox * 0.5;

    SlRefPtr<osg::Box> box = new osg::Box(boxCenter, xBox, yBox, zBox);
    boxDrawable->setShape(box);

    // add color to the primitive
    _addColor( caller, prim, boxDrawable, set );

    // Set the state of the geometry.
    boxDrawable->setStateSet ( state );

    // Add the Geometry to the Geode.
    geode->addDrawable ( boxDrawable );

    return true;
  }
  else if ( type == CadKit::CYLINDER )
  {    
    // 5 params define a cylinder
    tmp = set * 5;
    osg::Vec3f orientationVec(paramSetter.getPrimParams()->at(tmp), 
                              paramSetter.getPrimParams()->at(tmp+1), 
                              paramSetter.getPrimParams()->at(tmp+2));
    osg::Vec3f startVec(0.0f, 0.0f, 1.0f);
    osg::Quat cylRot;
    cylRot.makeRotate(startVec, orientationVec);
    float cylLength = orientationVec.length();
    osg::Vec3f cylCenter(originSetter.getPrimOrigins()->at(set));
    float bottomRadius = paramSetter.getPrimParams()->at(tmp+3);
    float topRadius = paramSetter.getPrimParams()->at(tmp+4);

    if(topRadius == 0.0f) // cone
    {      
      cylCenter[1] += cylLength * 0.5f;
      
      // Create a shape drawable.
      SlRefPtr<osg::ShapeDrawable> coneDrawable ( new osg::ShapeDrawable );
      if ( coneDrawable.isNull() )
        return ERROR ( "Failed to create osg::ShapeDrawable for cone.", CadKit::FAILED );

      SlRefPtr<osg::Cone> cone = new osg::Cone;
      cone->setCenter(cylCenter);
      cone->setHeight(cylLength);
      cone->setRotation(cylRot);
      cone->setRadius(bottomRadius);

      coneDrawable->setShape(cone);

      // add color to the primitive
      _addColor( caller, prim, coneDrawable, set );

      // Set the state of the geometry.
      coneDrawable->setStateSet ( state );

      // Add the Geometry to the Geode.
      geode->addDrawable ( coneDrawable );
    }
    else if ( topRadius == bottomRadius ) // regular cylinder
    {
      cylCenter[1] += cylLength * 0.5f;
      
      // Create a shape drawable.
      SlRefPtr<osg::ShapeDrawable> cylDrawable ( new osg::ShapeDrawable );
      if ( cylDrawable.isNull() )
        return ERROR ( "Failed to create osg::ShapeDrawable for cone.", CadKit::FAILED );

      SlRefPtr<osg::Cylinder> cyl = new osg::Cylinder;
      cyl->setCenter(cylCenter);
      cyl->setHeight(cylLength);
      cyl->setRotation(cylRot);
      cyl->setRadius(bottomRadius);

      cylDrawable->setShape(cyl);

      // add color to the primitive
      _addColor( caller, prim, cylDrawable, set );

      // Set the state of the geometry.
      cylDrawable->setStateSet ( state );

      // Add the Geometry to the Geode.
      geode->addDrawable ( cylDrawable );

    }
    else // truncated cone: not in OSG, so we make our own
    {
      // Create a geometry drawable.
      SlRefPtr<osg::Geometry> truncDrawable ( new osg::Geometry );
      if ( truncDrawable.isNull() )
        return ERROR ( "Failed to create osg::Geometry for truncated cone.", CadKit::FAILED );
      
      int numFacets = 40;  // TODO, make this configurable
      int setSize = numFacets + 2;

      // create vertices & indices for top & bottom circles
      SlRefPtr<osg::Vec3Array> truncVertices = new osg::Vec3Array;
      truncVertices->resize( 4 * setSize );

      SlRefPtr<osg::UByteArray> truncIndices = new osg::UByteArray(4 * setSize, GL_INT);

      SlRefPtr<osg::Vec3Array> truncNormals = new osg::Vec3Array;
      truncNormals->resize(4 * setSize);
      
      float angleDelta = 2.0f * osg::PI / (float)numFacets;
      float angle = 0.0f;
      float zNorm = (bottomRadius - topRadius) / cylLength;

      (*truncVertices)[0].set( 0.0f, 0.0f, 0.0f ); // bottom center
      (*truncVertices)[setSize].set( 0.0f, 0.0f, cylLength ); // top center

      (*truncIndices)[0] = 0;
      (*truncIndices)[setSize] = setSize;

      (*truncNormals)[0].set(0.0, 0.0, -1.0);
      (*truncNormals)[setSize].set(0.0, 0.0, 1.0);
      
      int i, j;
      for( i = 1, j = 0; i <= numFacets; i++,j+=2,angle+=angleDelta )
      {
        // Create vertices, indices & normals for 2 circles
        float c = cosf(angle);
        float s = sinf(angle);

        (*truncVertices)[i].set(c * bottomRadius, 
                                s * bottomRadius,
                                0.0f);
        (*truncIndices)[i] = i;
        (*truncNormals)[i].set(0.0, 0.0, -1.0);

        (*truncVertices)[i+setSize].set(c * topRadius, 
                                        s * topRadius, 
                                        cylLength);
        (*truncIndices)[i+setSize] = i+setSize;
        (*truncNormals)[i+setSize].set(0.0, 0.0, 1.0);

        // also create indices & normals for the cylinder sides
        (*truncIndices)[(2*setSize) + j] = (*truncIndices)[i];
        (*truncNormals)[(2*setSize) + j].set(c, s, zNorm);
        (*truncIndices)[(2*setSize) + j + 1] = (*truncIndices)[i+setSize];
        (*truncNormals)[(2*setSize) + j + 1].set(c, s, zNorm);
      }
      
      // last piece of fan
      (*truncIndices)[numFacets+1] = (*truncIndices)[1];
      (*truncNormals)[numFacets+1].set(0.0, 0.0, -1.0);
      (*truncIndices)[2*setSize - 1] = (*truncIndices)[setSize + 1];
      (*truncNormals)[2*setSize - 1].set(0.0, 0.0, 1.0);
      
      // last part of strip
      (*truncIndices)[4*setSize - 4] = (*truncIndices)[(2*setSize)];
      (*truncNormals)[4*setSize - 4] = (*truncNormals)[(2*setSize)];
      (*truncIndices)[4*setSize - 3] = (*truncIndices)[(2*setSize)+1];
      (*truncNormals)[4*setSize - 3] = (*truncNormals)[(2*setSize)+1];
      
      // apply transform to vertices
      for(i = 0; i < 2 * setSize; i++)
      {
        (*truncVertices)[i] = cylRot * (*truncVertices)[i];
        (*truncVertices)[i] += cylCenter;
      }
      
      // apply transform to normals
      for(i = 0; i < 4 * setSize; i++)
        (*truncNormals)[i] = cylRot * (*truncNormals)[i];

      truncDrawable->setVertexArray(truncVertices);
      truncDrawable->setVertexIndices (truncIndices);
      truncDrawable->setNormalArray( truncNormals );
      truncDrawable->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

      osg::Geometry::PrimitiveSetList primitives;
      primitives.resize(3);
      primitives[0] = new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN, 0, setSize);
      primitives[1] = new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN, setSize, setSize);
      primitives[2] = new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 2 * setSize, 2 * setSize - 2);
      truncDrawable->setPrimitiveSetList( primitives );

      // assign a color
      _addColor( caller, prim, truncDrawable, set );

      // Set the state of the geometry.
      truncDrawable->setStateSet ( state );

      // Add the Geometry to the Geode.
      geode->addDrawable ( truncDrawable );

      return true;
    }

    return true;
  }
  else if ( type == CadKit::PYRAMID )
  {
    // Create a geometry drawable.
    SlRefPtr<osg::Geometry> pyrDrawable ( new osg::Geometry );
    if ( pyrDrawable.isNull() )
      return ERROR ( "Failed to create osg::Geometry for pyramid.", CadKit::FAILED );
    
    tmp = set * 9;
    osg::Vec3f pyrAxisX(paramSetter.getPrimParams()->at(tmp), 
                        paramSetter.getPrimParams()->at(tmp+1), 
                        paramSetter.getPrimParams()->at(tmp+2));
    osg::Vec3f pyrAxisY(paramSetter.getPrimParams()->at(tmp+3), 
                        paramSetter.getPrimParams()->at(tmp+4), 
                        paramSetter.getPrimParams()->at(tmp+5));
    osg::Vec3f pyrAxisZ(paramSetter.getPrimParams()->at(tmp+6), 
                        paramSetter.getPrimParams()->at(tmp+7), 
                        paramSetter.getPrimParams()->at(tmp+8));
    
    osg::Vec3f pyrCenter(originSetter.getPrimOrigins()->at(set));

    osg::Vec3f pyrTop = pyrCenter;
    pyrTop[2] += pyrAxisZ.length();

    osg::Vec3f pyrOrigin(pyrCenter[0] - pyrAxisX.length() * 0.5, pyrCenter[1] - pyrAxisY.length() * 0.5, pyrCenter[2]);

    // Create a vertices for this shape
    SlRefPtr<osg::Vec3Array> pyrVertices = new osg::Vec3Array;
    pyrVertices->resize(5);
    
    (*pyrVertices)[0].set( pyrTop ); // top
    (*pyrVertices)[1].set( pyrOrigin );  // lower left corner
    (*pyrVertices)[2].set( pyrOrigin + pyrAxisX ); // lower right corner
    (*pyrVertices)[3].set( pyrOrigin + pyrAxisX + pyrAxisY ); // far right corner
    (*pyrVertices)[4].set( pyrOrigin + pyrAxisY ); // far left corner
    
    pyrDrawable->setVertexArray(pyrVertices);

    SlRefPtr<osg::UByteArray> pyrIndices = new osg::UByteArray(16, GL_INT);
    (*pyrIndices)[0] = 0; (*pyrIndices)[1] = 1; (*pyrIndices)[2] = 4;  // side 1
    (*pyrIndices)[3] = 0; (*pyrIndices)[4] = 4; (*pyrIndices)[5] = 3;  // side 2
    (*pyrIndices)[6] = 0; (*pyrIndices)[7] = 3; (*pyrIndices)[8] = 2;  // side 3
    (*pyrIndices)[9] = 0; (*pyrIndices)[10] = 2; (*pyrIndices)[11] = 1; // side 4
    (*pyrIndices)[12] = 4; (*pyrIndices)[13] = 3; (*pyrIndices)[14] = 2; (*pyrIndices)[15] = 1; // bottom quad
    
    pyrDrawable->setVertexIndices (pyrIndices);

    osg::Geometry::PrimitiveSetList primitives;
    primitives.resize(2);
    primitives[0] = new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, 12);
    primitives[1] = new osg::DrawArrays(osg::PrimitiveSet::QUADS, 12, 4);
    pyrDrawable->setPrimitiveSetList( primitives );

    // calculate normals
    SlRefPtr<osg::Vec3Array> pyrNormals = new osg::Vec3Array;
    pyrNormals->resize(16);

    osg::Vec3f tempVec = ( (*pyrVertices)[4] - (*pyrVertices)[0] ) ^ ( (*pyrVertices)[1] - (*pyrVertices)[0] );
    tempVec.normalize();
    (*pyrNormals)[0].set( tempVec );
    (*pyrNormals)[1].set( (*pyrNormals)[0] );
    (*pyrNormals)[2].set( (*pyrNormals)[0] );
    
    tempVec = ( (*pyrVertices)[3] - (*pyrVertices)[0] ) ^ ( (*pyrVertices)[4] - (*pyrVertices)[0] );
    tempVec.normalize();
    (*pyrNormals)[3].set( tempVec );
    (*pyrNormals)[4].set( (*pyrNormals)[3] );
    (*pyrNormals)[5].set( (*pyrNormals)[3] );
    
    tempVec = ( (*pyrVertices)[2] - (*pyrVertices)[0] ) ^ ( (*pyrVertices)[3] - (*pyrVertices)[0] );
    tempVec.normalize();
    (*pyrNormals)[6].set( tempVec );
    (*pyrNormals)[7].set( (*pyrNormals)[6] );
    (*pyrNormals)[8].set( (*pyrNormals)[6] );
    
    tempVec = ( (*pyrVertices)[1] - (*pyrVertices)[0] ) ^ ( (*pyrVertices)[2] - (*pyrVertices)[0] );
    tempVec.normalize();
    (*pyrNormals)[9].set( tempVec );
    (*pyrNormals)[10].set( (*pyrNormals)[9] );
    (*pyrNormals)[11].set( (*pyrNormals)[9] );

    (*pyrNormals)[12].set( 0.0f, 0.0f, -1.0f );
    (*pyrNormals)[13].set( (*pyrNormals)[12] );
    (*pyrNormals)[14].set( (*pyrNormals)[12] );
    (*pyrNormals)[15].set( (*pyrNormals)[12] );
      
    pyrDrawable->setNormalArray( pyrNormals );
    pyrDrawable->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

    // assign a color
    _addColor( caller, prim, pyrDrawable, set );

    // Set the state of the geometry.
    pyrDrawable->setStateSet ( state );

    // Add the Geometry to the Geode.
    geode->addDrawable ( pyrDrawable );

    return true;
  }
  else if ( type == CadKit::SPHERE )
  {
    SlRefPtr<osg::ShapeDrawable> sphereDrawable ( new osg::ShapeDrawable );
    if ( sphereDrawable.isNull() )
      return ERROR ( "Failed to create osg::ShapeDrawable for sphere.", CadKit::FAILED );
    
    // 1 param (radius) defines a sphere
    SlRefPtr<osg::Sphere> sphere = new osg::Sphere;
    sphere->setCenter(osg::Vec3f(originSetter.getPrimOrigins()->at(set)));
    sphere->setRadius(paramSetter.getPrimParams()->at(set));
    sphereDrawable->setShape(sphere);

    // add color to the primitive
    _addColor( caller, prim, sphereDrawable, set );

    // Set the state of the geometry.
    sphereDrawable->setStateSet ( state );

    // Add the Geometry to the Geode.
    geode->addDrawable ( sphereDrawable );

    return true;
  }
  else if ( type == CadKit::TRIPRISM )
  {
    // Create a geometry drawable.
    SlRefPtr<osg::Geometry> prismDrawable ( new osg::Geometry );
    if ( prismDrawable.isNull() )
      return ERROR ( "Failed to create osg::Geometry for triprism.", CadKit::FAILED );
    
    tmp = set * 9;
    osg::Vec3f prismAxisX(paramSetter.getPrimParams()->at(tmp), 
                          paramSetter.getPrimParams()->at(tmp+1), 
                          paramSetter.getPrimParams()->at(tmp+2));
    osg::Vec3f prismAxisY(paramSetter.getPrimParams()->at(tmp+3), 
                          paramSetter.getPrimParams()->at(tmp+4), 
                          paramSetter.getPrimParams()->at(tmp+5));
    osg::Vec3f prismAxisZ(paramSetter.getPrimParams()->at(tmp+6), 
                          paramSetter.getPrimParams()->at(tmp+7), 
                          paramSetter.getPrimParams()->at(tmp+8));
    
    osg::Vec3f prismOrigin(originSetter.getPrimOrigins()->at(set));

    // Create a vertices for this shape
    SlRefPtr<osg::Vec3Array> prismVertices = new osg::Vec3Array;
    prismVertices->resize(6);
    
    (*prismVertices)[0].set( prismOrigin );
    (*prismVertices)[1].set( prismOrigin + prismAxisX );
    (*prismVertices)[2].set( prismOrigin + prismAxisY );
    (*prismVertices)[3].set( prismOrigin + prismAxisZ );
    (*prismVertices)[4].set( (*prismVertices)[1] + prismAxisZ );
    (*prismVertices)[5].set( (*prismVertices)[2] + prismAxisZ );
    
    prismDrawable->setVertexArray(prismVertices);

    SlRefPtr<osg::UByteArray> prismIndices = new osg::UByteArray(18, GL_INT);
    (*prismIndices)[0] = 0; (*prismIndices)[1] = 1; (*prismIndices)[2] = 2;  // back triangle
    (*prismIndices)[3] = 3; (*prismIndices)[4] = 5; (*prismIndices)[5] = 4;  // front triangle
    (*prismIndices)[6] = 0; (*prismIndices)[7] = 3; (*prismIndices)[8] = 4; (*prismIndices)[9] = 1; // bottom quad
    (*prismIndices)[10] = 0; (*prismIndices)[11] = 3; (*prismIndices)[12] = 5; (*prismIndices)[13] = 2;  // left quad
    (*prismIndices)[14] = 2; (*prismIndices)[15] = 5; (*prismIndices)[16] = 4; (*prismIndices)[17] = 1;  // top quad
    
    prismDrawable->setVertexIndices (prismIndices);

    osg::Geometry::PrimitiveSetList primitives;
    primitives.resize(2);
    primitives[0] = new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, 6);
    primitives[1] = new osg::DrawArrays(osg::PrimitiveSet::QUADS, 6, 12);
    prismDrawable->setPrimitiveSetList( primitives );

    // calculate normals
    SlRefPtr<osg::Vec3Array> prismNormals = new osg::Vec3Array;
    prismNormals->resize(18);

    (*prismNormals)[0].set( 0.0f, 0.0f, -1.0f );
    (*prismNormals)[1].set( (*prismNormals)[0] );
    (*prismNormals)[2].set( (*prismNormals)[0] );

    (*prismNormals)[3].set( 0.0f, 0.0f, 1.0f );
    (*prismNormals)[4].set( (*prismNormals)[3] );
    (*prismNormals)[5].set( (*prismNormals)[3] );

    (*prismNormals)[6].set( 0.0, -1.0, 0.0 );
    (*prismNormals)[7].set( (*prismNormals)[6] );
    (*prismNormals)[8].set( (*prismNormals)[6] );
    (*prismNormals)[9].set( (*prismNormals)[6] );

    (*prismNormals)[10].set( -1.0, 0.0, 0.0 );
    (*prismNormals)[11].set( (*prismNormals)[10] );
    (*prismNormals)[12].set( (*prismNormals)[10] );
    (*prismNormals)[13].set( (*prismNormals)[10] );
    
     osg::Vec3f tempVec = ( prismAxisY - prismAxisX ) ^ prismAxisZ;
     tempVec.normalize();
    (*prismNormals)[14].set( tempVec );
    (*prismNormals)[15].set( (*prismNormals)[14] );
    (*prismNormals)[16].set( (*prismNormals)[14] );
    (*prismNormals)[17].set( (*prismNormals)[14] );
    
    prismDrawable->setNormalArray( prismNormals );
    prismDrawable->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

    // assign a color
    _addColor( caller, prim, prismDrawable, set );

    // Set the state of the geometry.
    prismDrawable->setStateSet ( state );

    // Add the Geometry to the Geode.
    geode->addDrawable ( prismDrawable );

    return true;
  }

  // It didn't work.
  return WARNING ( FORMAT ( "Failed to add primitive %X.", prim ), CadKit::FAILED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the primitive's color(s).
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::_addColor ( IUnknown *caller, PrimHandle prim, osg::ShapeDrawable *drawable, int set )
{
  SL_PRINT6 ( "In DbOsgDatabase::_addColor(), this = %X, caller = %X, primitive = %d, drawable = %X, set = %d\n", this, caller, prim, drawable, set );
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
    drawable->setColor( setter.getColors()->at(set) );

    // It worked.
    return true;
  }

  // It didn't work. Don't report an error, there may not be any.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the primitive's colors.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgDatabase::_addColor ( IUnknown *caller, PrimHandle prim, osg::Geometry *geometry, int set )
{
  SL_PRINT5 ( "In DbOsgDatabase::_addColor(), this = %X, caller = %X, primitive = %d, geometry = %X\n", this, caller, prim, geometry );
  SL_ASSERT ( caller );
  SL_ASSERT ( geometry );

  // Get the interface we need from the caller.
  SlQueryPtr<IQueryPrimColorsVec4f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", CadKit::NO_INTERFACE );

  // Get the colors.
  DbOsgPrimColorSetter setter;
  if ( query->getColors ( prim, setter ) )
  {
    // Set the color.
    SlRefPtr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->resize(1);
    (*colors)[0].set( setter.getColors()->at(set)[0], setter.getColors()->at(set)[1], 
                      setter.getColors()->at(set)[2], setter.getColors()->at(set)[3] );

    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

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
