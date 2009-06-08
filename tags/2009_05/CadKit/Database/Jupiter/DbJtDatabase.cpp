
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbJtDatabase: Jupiter database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbJtPrecompiled.h"
#include "DbJtDatabase.h"
#include "DbJtInline.h"
#include "DbJtFunctions.h"
#include "DbJtTraversalState.h"
#include "DbJtVisApi.h"
#include "DbJtVisApiHeaders.h"

#include "Interfaces/ITriangleAppend.h" // Keep this first or else VC++ internal compiler error.
#include "Interfaces/IMessageNotify.h"  // VC++ internal compiler error if
#include "Interfaces/IEntityNotify.h"   // if these are after SlInline.h
#include "Interfaces/IDataTarget.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlMessageIds.h"
#include "Standard/SlQueryPtr.h"
#include "Standard/SlVec2.h"
#include "Standard/SlVec3.h"
#include "Standard/SlInline.h"

// This is the only way to get a pointer to the traverser from inside the 
// callback function.
namespace CadKit { DbJtDatabase *_traverser = NULL; }

// To help shorten up the lines.
#undef  ERROR
#define ERROR         this->_notifyError
#define PROGRESS      this->_notifyProgress
#define WARNING       this->_notifyWarning
#define FORMAT        CadKit::getString
#define UNKNOWN(ptr)  SlRefPtr<CadKit::IUnknown> ( ptr ? ptr->queryInterface ( CadKit::IUnknown::IID ) : NULL )
#define THIS_UNKNOWN  UNKNOWN ( this )

// The arguments have changed from DMDTk 5.0 to 5.2.
#if ( DMDTK_MAJOR_VERSION == 5 && DMDTK_MINOR_VERSION >= 2 )
#define TRAVERSER_ARGUMENTS root, 0x0
#else
#define TRAVERSER_ARGUMENTS root
#endif

// Don't bother formatting the string if the controller won't print it.
#define PROGRESS_LEVEL(priority)\
  if ( priority <= _progressPriorityLevel )\
    this->_notifyProgress

// We need one of these to exist for the duration of this library's execution.
// This will initialize and uninitialize the DMDTk.
namespace CadKit { DbJtVisApi _globalVisApi; };

using namespace CadKit;

// These live in DbJtTraverser for now.
namespace CadKit
{
void _incrementPointerReferenceCount ( eaiEntity *p );
void _decrementPointerReferenceCount ( eaiEntity *p );
};

SL_IMPLEMENT_DYNAMIC_CLASS ( DbJtDatabase, DbBaseSource );
CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS ( DbJtDatabase, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtDatabase::DbJtDatabase ( const unsigned int &customerId ) : DbBaseSource(),
  _initialized ( false ),
  _assemblyLoadOption ( INSTANCE_ASSEMBLY ),
  _partLoadOption ( INSTANCE_PART ),
  _brepLoadOption ( TESS_ONLY ),
  _shapeLoadOption ( ALL_LODS ), // TODO, want all lods by default.
  _assemblies ( new Assemblies ),
  _current ( new DbJtTraversalState ),
  _shapeData ( new ShapeData ( NULL ) ),
  _primData  ( new PrimData ( NULL ) ) ,
  _progressPriorityLevel ( 0 ), // Send everything.
  _result ( true ),
  _lodOption ( CadKit::PROCESS_ALL_LODS ),
  _matrixStack()
{
  SL_PRINT2 ( "In DbJtDatabase::DbJtDatabase(), this = %X\n", this );
  SL_ASSERT ( NULL != _assemblies.get() );
  SL_ASSERT ( NULL != _current.get() );
  SL_ASSERT ( NULL != _shapeData.get() );
  SL_ASSERT ( NULL != _primData.get() );

  // Set the VisApi's customer id.
  _globalVisApi.setCustomerId ( customerId );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtDatabase::~DbJtDatabase()
{
  SL_PRINT2 ( "In DbJtDatabase::~DbJtDatabase(), this = %X\n", this );
  SL_ASSERT ( UNSET_INDEX == _current->getLevel() );
  SL_ASSERT ( UNSET_INDEX == _current->getLod() );
  SL_ASSERT ( UNSET_INDEX == _current->getShape() );
  SL_ASSERT ( UNSET_INDEX == _current->getPrim() );
  SL_ASSERT ( UNSET_INDEX == _current->getSet() );
  SL_ASSERT ( NULL == _current->getPart() );
  SL_ASSERT ( NULL == _current->getInstance() );
  SL_ASSERT ( true == _assemblies->empty() );
  SL_ASSERT ( true == _matrixStack.empty() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbJtDatabase::queryInterface ( unsigned long iid )
{
  SL_PRINT2 ( "In DbJtDatabase::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case ILoadOptions::IID:
    return static_cast<ILoadOptions *>(this);
  case IAssemblyQueryFloat::IID:
    return static_cast<IAssemblyQueryFloat *>(this);
  case IPartQueryFloat::IID:
    return static_cast<IPartQueryFloat *>(this);
  case IInstanceQueryFloat::IID:
    return static_cast<IInstanceQueryFloat *>(this);
  case ILodQuery::IID:
    return static_cast<ILodQuery *>(this);
  case IShapeQueryFloatUchar::IID:
    return static_cast<IShapeQueryFloatUchar *>(this);
  case IPrimQueryFloat::IID:
    return static_cast<IPrimQueryFloat *>(this);
  case ISetQuery::IID:
    return static_cast<ISetQuery *>(this);
  case IQueryShapeVerticesVec3f::IID:
    return static_cast<IQueryShapeVerticesVec3f *>(this);
  case IQueryShapeNormalsVec3f::IID:
    return static_cast<IQueryShapeNormalsVec3f *>(this);
  case IQueryShapeColorsVec3f::IID:
    return static_cast<IQueryShapeColorsVec3f *>(this);
  case IQueryShapeColorsVec4f::IID:
    return static_cast<IQueryShapeColorsVec4f *>(this);
  case IQueryShapeTexCoordsVec2f::IID:
    return static_cast<IQueryShapeTexCoordsVec2f *>(this);
  case IMessagePriority::IID:
    return static_cast<IMessagePriority *>(this);
  case ILodOption::IID:
    return static_cast<ILodOption *>(this);
  case IFileExtension::IID:
    return static_cast<IFileExtension *>(this);
  case IDataRead::IID:
    return static_cast<IDataRead *>(this);
  case IUnits::IID:
    return static_cast<IUnits *>(this);
  case IQueryPrimOriginsVec3f::IID:
    return static_cast<IQueryPrimOriginsVec3f *>(this);
  case IQueryPrimParamsFloat::IID:
    return static_cast<IQueryPrimParamsFloat *>(this);
  case IQueryPrimColorsVec3f::IID:
    return static_cast<IQueryPrimColorsVec3f *>(this);
  case IQueryPrimColorsVec4f::IID:
    return static_cast<IQueryPrimColorsVec4f *>(this);
  default:
    return DbBaseSource::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file extension.
//
///////////////////////////////////////////////////////////////////////////////

std::string DbJtDatabase::getFileExtension() const
{
  SL_PRINT2 ( "In DbJtDatabase::getFileExtension(), this = %X\n", this );
  return "jt";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the format have the attribute?
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::isAttributeSupported ( const FormatAttribute &attribute ) const
{
  SL_PRINT3 ( "In DbJtDatabase::isAttributeSupported(), this = %X, attribute = %d\n", this, attribute );

  switch ( attribute )
  {
  case FORMAT_ATTRIBUTE_BINARY:
    return true;
  case FORMAT_ATTRIBUTE_ASCII:
    return false;
  default:
    SL_ASSERT ( 0 ); // What format is this?
    return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the units.
//
///////////////////////////////////////////////////////////////////////////////

std::string DbJtDatabase::getUnits ( ) const
{
  return _units;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::readData ( const std::string &filename )
{
  SL_PRINT3 ( "In DbJtDatabase::readData(), this = %X, name = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );

  // Try to traverse.
  try
  {
#ifdef _CADKIT_USE_JTOPEN
    if ( false == PROGRESS ( FORMAT ( "Attempting to register JT Open Toolkit customer number %d.", _globalVisApi.getCustomerId() ) ) )
#else // _CADKIT_USE_JTOPEN
    if ( false == PROGRESS ( FORMAT ( "Attempting to register DirectModel Data Toolkit customer number %d.", _globalVisApi.getCustomerId() ) ) )
#endif // _CADKIT_USE_JTOPEN
      return false;

    // Initialize (register customer).
    if ( false == _globalVisApi.init() )
    {
#ifdef _CADKIT_USE_JTOPEN
      ERROR ( FORMAT ( "Failed to register JT Open Toolkit customer number: %d.", _globalVisApi.getCustomerId() ), 0 );
#else // _CADKIT_USE_JTOPEN
      ERROR ( FORMAT ( "Failed to register DirectModel Data Toolkit customer number: %d.", _globalVisApi.getCustomerId() ), 0 );
#endif
      return false;
    }

#ifdef _CADKIT_USE_JTOPEN
    if ( false == PROGRESS ( "Done registering JT Open Toolkit customer." ) )
#else // _CADKIT_USE_JTOPEN
    if ( false == PROGRESS ( "Done registering DirectModel Data Toolkit customer." ) )
#endif // _CADKIT_USE_JTOPEN
      return false;

    // Traverse the database.
    return this->_traverse ( filename );
  }

  // Catch any exceptions.
  catch ( const std::exception &e )
  {
    ERROR ( FORMAT ( "Exception generated when traversing '%s', reason: %s", filename.c_str(), e.what() ), 0 );
    return false;
  }
  catch ( const char *e )
  {
    ERROR ( FORMAT ( "Exception generated when traversing '%s', reason: %s", filename.c_str(), e ), 0 );
    return false;
  }
#ifdef _CADKIT_CATCH_ALL
  catch ( ... )
  {
    ERROR ( FORMAT ( "Exception generated when traversing '%s'", filename.c_str() ), 0 );
    return false;
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse the database.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_traverse ( const std::string &filename )
{
  SL_PRINT3 ( "In DbJtDatabase::_traverse(), this = %X, filename = %s\n", this, filename.c_str() );
  SL_ASSERT ( filename.size() );


  // Since this is the beginning of a new file, we need to scale.

  this->_setNeedToScale ( true );


  // Make sure the client data is cleared.
  this->_clearClientDataMaps();

  if ( false == PROGRESS ( "Creating CAD importer." ) )
    return false;

  // Declare a CAD importer. The returned instance has a zero reference count.
  // Assigning it to the SlRefPtr will increment it to one. When the SlRefPtr 
  // goes out of scope the internal pointer will be deferenced (back to zero),
  // and the importer will get deleted.
  SlRefPtr<eaiCADImporter> importer ( eaiEntityFactory::createCADImporter() );
  if ( importer.isNull() )
  {
    ERROR ( FORMAT ( "Failed to create CAD importer. Is your license set up correctly?" ), 0 );
    return false;
  }

  if ( false == PROGRESS ( "Done creating CAD importer." ) )
    return false;

  // Set the options.
  importer->setShapeLoadOption ( CadKit::convert ( _shapeLoadOption,    _messageNotify ) );
  importer->setBrepLoadOption  ( CadKit::convert ( _brepLoadOption,     _messageNotify ) );
  importer->setAssemblyOption  ( CadKit::convert ( _assemblyLoadOption, _messageNotify ) );

  if ( false == PROGRESS ( "Creating traverser." ) )
    return false;

  // Declare the traverser. Note: returned pointer has ref-count of one.
  SlRefPtr<eaiTraverser> traverser ( eaiEntityFactory::createTraverser() );
  if ( traverser.isNull() )
  {
    ERROR ( "Failed to create database traverser.", 0 );
    return false;
  }

  if ( false == PROGRESS ( "Done creating traverser.\nRegistering traversal callbacks." ) )
    return false;

  // Hook up the callback function.
  if ( eai_OK != traverser->setupPreActionCallback ( &DbJtDatabase::_preActionTraversalCallback ) )
  {
    ERROR ( "Failed to register pre-action traversal callback.", 0 );
    return false;
  }

  // And the other callback function too.
  if ( eai_OK != traverser->setupPostActionCallback ( &DbJtDatabase::_postActionTraversalCallback ) )
  {
    ERROR ( "Failed to register post-action traversal callback.", 0 );
    return false;
  }

  if ( false == PROGRESS ( FORMAT ( "Done registering traversal callbacks.\nImporting database: %s", filename.c_str() ) ) )
    return false;

  // Import the database. Note: returned pointer has ref-count of one.
  SlRefPtr<eaiHierarchy> root ( importer->import ( filename.c_str() ) );
  if ( root.isNull() )
  {
    ERROR ( FORMAT ( "Failed to import database: %s", filename.c_str() ), 0 );
    return false;
  }

  if ( false == PROGRESS ( FORMAT ( "Done importing database: %s\nStarting the traversal.", filename.c_str() ) ) )
    return false;

  // Inform the target that it is about to receive data.
  SlQueryPtr<IDataTarget> target ( _target );
  if ( target.isValid() )
    if ( false == target->dataTransferStart ( THIS_UNKNOWN ) )
      return false;

  // There doesn't appear to be a way to get a pointer to this instance inside 
  // the callback function. So I have to use this work-around.
  _traverser = this;

  // Traverse the database.
  if ( eai_OK != traverser->traverseGraph ( TRAVERSER_ARGUMENTS ) )
  {
    ERROR ( "Failed to traverse database.", 0 );
    return false;
  }

  // Reset this.
  _traverser = NULL;

  // Inform the target that it is done receiving data.
  if ( target.isValid() )
    if ( false == target->dataTransferEnd ( THIS_UNKNOWN ) )
      return false;

  // Once again, clear the client data.
  this->_clearClientDataMaps();

  if ( false == PROGRESS ( FORMAT ( "Done traversing: %s", filename.c_str() ) ) )
    return false;

  // TODO: It throws an exception when I allow this to be unref'd (and deleted).
  importer.makeNull();

  // Did it work?
  return _result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-action traversal callback. For DMDTk < 5.2.
//
///////////////////////////////////////////////////////////////////////////////

int DbJtDatabase::_preActionTraversalCallback ( eaiHierarchy *hierarchy, int level )
{
  SL_PRINT3 ( "In DbJtDatabase::_preActionTraversalCallback(), hierarchy = %X, level = %d\n", hierarchy, level );
  SL_ASSERT ( _traverser );

  // Call the other one.
  return ( true == _traverser->_preActionTraversalNotify ( hierarchy, level ) ) ? eai_OK : eai_ERROR;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post-action traversal callback. For DMDTk < 5.2.
//
///////////////////////////////////////////////////////////////////////////////

int DbJtDatabase::_postActionTraversalCallback ( eaiHierarchy *hierarchy, int level )
{
  SL_PRINT3 ( "In DbJtDatabase::_postActionTraversalCallback(), hierarchy = %X, level = %d\n", hierarchy, level );
  SL_ASSERT ( _traverser );

  // Call the other one.
  return ( true == _traverser->_postActionTraversalNotify ( hierarchy, level ) ) ? eai_OK : eai_ERROR;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-action traversal callback. For DMDTk >= 5.2.
//
///////////////////////////////////////////////////////////////////////////////

int DbJtDatabase::_preActionTraversalCallback ( eaiHierarchy *hierarchy, int level, eaiClientData *data )
{
  SL_PRINT3 ( "In DbJtDatabase::_preActionTraversalCallback(), hierarchy = %X, level = %d\n", hierarchy, level );
  SL_ASSERT ( _traverser );
  SL_ASSERT ( NULL == data );

  // Call the other one.
  return ( true == _traverser->_preActionTraversalNotify ( hierarchy, level ) ) ? eai_OK : eai_ERROR;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post-action traversal callback. For DMDTk >= 5.2.
//
///////////////////////////////////////////////////////////////////////////////

int DbJtDatabase::_postActionTraversalCallback ( eaiHierarchy *hierarchy, int level, eaiClientData *data )
{
  SL_PRINT3 ( "In DbJtDatabase::_postActionTraversalCallback(), hierarchy = %X, level = %d\n", hierarchy, level );
  SL_ASSERT ( _traverser );
  SL_ASSERT ( NULL == data );

  // Call the other one.
  return ( true == _traverser->_postActionTraversalNotify ( hierarchy, level ) ) ? eai_OK : eai_ERROR;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-action traversal notify.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_preActionTraversalNotify ( eaiHierarchy *hierarchy, int level )
{
  SL_PRINT3 ( "In DbJtDatabase::_preActionTraversalNotify(), hierarchy = %X, level = %d\n", hierarchy, level );

  // If we failed then just return. Return false makes the DMDTk 
  // throw an exception.
  if ( false == _result )
    return true;

  // Check the level.
  if ( level < 0 )
  {
    SL_ASSERT ( 0 ); // Corrupt database?
    return ERROR ( FORMAT ( "Traverser reported eaiHierarchy at level: %2d, name: %s", level, hierarchy->name() ), 0 );
  }

  // Grab the level.
  _current->setLevel ( level );

  // Initialize.
  _result = false;
  this->_resetStateIndices();
  _shapeData->init ( NULL );
  _primData->init ( NULL );

  // Determine the type.
  switch ( hierarchy->typeID() )
  {
  case eaiHierarchy::eaiASSEMBLY:

    // Save this assembly.
    this->_pushAssembly ( (eaiAssembly *) hierarchy );

    // Push its matrix.
    this->_pushMatrix ( hierarchy );

    // Start the assembly.
    _result = this->_startAssembly ( (eaiAssembly *) hierarchy );
    break;

  case eaiHierarchy::eaiPART:

    // Save this part.
    this->_setCurrentPart ( (eaiPart *) hierarchy );

    // Push its matrix.
    this->_pushMatrix ( hierarchy );

    // Start the part.
    _result = this->_startPart ( (eaiPart *) hierarchy );
    break;

  case eaiHierarchy::eaiINSTANCE:

    // Save this instance.
    this->_setCurrentInstance ( (eaiInstance *) hierarchy );

    // Push its matrix. Note: Nesting this inside _setCurrentInstance() and 
    // _setCurrentPart() won't work because of the way you call 
    // _setCurrentPart() below (which pushed one too may matrices on the stack).
    this->_pushMatrix ( hierarchy );

    // If we are supposed to convert the part-instances into parts...
    if ( EXPLODE_PART == _partLoadOption )
    {
      // For convenience.
      eaiInstance *instance = (eaiInstance *) hierarchy;

      // See if this instance is really a part.
      if ( CadKit::isPart ( instance->original() ) )
      {
        // For convenience.
        eaiPart *part = (eaiPart *) instance->original();

        // Save this part.
        this->_setCurrentPart ( part );

        // Replace the part's properties with the instance's properties.
        this->_replaceProperties ( instance, part );

        // Start the part.
        _result = this->_startPart ( part );
      }
    }

    // Otherwise, just process the instance as-is.
    else
    {
      // Start the instance.
      _result = this->_startInstance ( (eaiInstance *) hierarchy );
    }

    break;

  default:

    SL_ASSERT ( 0 ); // What entity type is this?
    _result = ERROR ( FORMAT ( "Unknown entity type %d, level %2d, name: %s", hierarchy->typeID(), level, hierarchy->name() ), 0 );
    break;
  }

  // Reset the level.
  _current->setLevel ( UNSET_INDEX );

  // Return the result.
  return _result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post-action traversal notify.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_postActionTraversalNotify ( eaiHierarchy *hierarchy, int level )
{
  SL_PRINT3 ( "In DbJtDatabase::_postActionTraversalNotify(), hierarchy = %X, level = %d\n", hierarchy, level );

  // If we failed then just return. Return false makes the DMDTk 
  // throw an exception.
  if ( false == _result )
    return true;

  // Check the level.
  if ( level < 0 )
  {
    SL_ASSERT ( 0 ); // Corrupt database?
    return ERROR ( FORMAT ( "Traverser reported eaiHierarchy at level: %2d, name: %s", level, hierarchy->name() ), 0 );
  }

  // Grab the level.
  _current->setLevel ( level );

  // Initialize.
  _result = false;
  this->_resetStateIndices();
  _shapeData->init ( NULL );
  _primData->init ( NULL );

  // Determine the type.
  switch ( hierarchy->typeID() )
  {
  case eaiHierarchy::eaiASSEMBLY:

    // Should be true.
    SL_ASSERT ( ((eaiAssembly *) hierarchy) == this->_getCurrentAssembly() );

    // End the assembly.
    _result = this->_endAssembly ( (eaiAssembly *) hierarchy );

    // Done with this assembly.
    this->_popAssembly();

    // Pop its matrix.
    this->_popMatrix();

    break;

  case eaiHierarchy::eaiPART:

    // End the part.
    _result = this->_endPart ( (eaiPart *) hierarchy );

    // No more part.
    this->_setCurrentPart ( NULL );

    // Pop its matrix.
    this->_popMatrix();

    break;

  case eaiHierarchy::eaiINSTANCE:

    // If we are supposed to convert the part-instances into parts...
    if ( EXPLODE_PART == _partLoadOption )
    {
      // For convenience.
      eaiInstance *instance = (eaiInstance *) hierarchy;

      // See if this instance is really a part.
      if ( CadKit::isPart ( instance->original() ) )
      {
        // For convenience.
        eaiPart *part = (eaiPart *) instance->original();

        // Should be true.
        SL_ASSERT ( instance == this->_getCurrentInstance() );
        SL_ASSERT ( part == this->_getCurrentPart() );

        // End the part.
        _result = this->_endPart ( part );

        // No more part.
        this->_setCurrentPart ( NULL );
      }
    }

    // Otherwise, just process the instance as-is.
    else
    {
      // End the instance.
      _result = this->_endInstance ( (eaiInstance *) hierarchy );
    }

    // No more instance.
    this->_setCurrentInstance ( NULL );
    
    // Pop its matrix.
    this->_popMatrix();

    break;

  default:

    SL_ASSERT ( 0 ); // What entity type is this?
    _result = WARNING ( FORMAT ( "Unknown entity type %d, level %2d, name: %s", hierarchy->typeID(), level, hierarchy->name() ), CadKit::UNKNOWN_ENTITY );
    break;
  }

  // Reset the level.
  _current->setLevel ( UNSET_INDEX );

  // Return the result.
  return _result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start an assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_startAssembly ( eaiAssembly *assembly )
{
  SL_PRINT3 ( "In DbJtDatabase::_startAssembly(), assembly = %X, level = %d\n", assembly, _current->getLevel() );
  SL_ASSERT ( assembly );
  
  // set the model's units.
  if(_units.empty())
  {
    JtkUnits units;
    if(Jtk_OK == assembly->getUnits(units))
    {
      switch(units)
      {
      case JtkMICROMETERS:
        _units = "Micrometers";
        break;
      case JtkMILLIMETERS:
        _units = "Millimeters";
        break;
      case JtkCENTIMETERS:
        _units = "Centimeters";
        break;
      case JtkDECIMETERS:
        _units = "Decimeters";
        break;
      case JtkMETERS:
        _units = "Meters";
        break;
      case JtkKILOMETERS:
        _units = "Kilometers";
        break;
      case JtkINCHES:
        _units = "Inches";
        break;
      case JtkFEET:
        _units = "Feet";
        break;
      case JtkYARDS:
        _units = "Yards";
        break;
      case JtkMILES:
        _units = "Miles";
        break;
      case JtkMILS:
        _units = "Mils";
        break;
      default:
        _units = "Unknown";
        break;
      }
    }
  }

  if ( false == PROGRESS ( FORMAT ( "Assembly, level %2d, name: %s", _current->getLevel(), assembly->name() ) ) )
    return false;

  // Try this interface.
  SlQueryPtr<IAssemblyNotify> assemblyNotify ( _target );
  if ( assemblyNotify.isValid() )
    return CadKit::handleEntityStart ( assemblyNotify.getValue(), (AssemblyHandle) assembly, THIS_UNKNOWN, _messageNotify );

  // Try this interface.
  SlQueryPtr<IGroupNotify> groupNotify ( _target );
  if ( groupNotify.isValid() )
    return CadKit::handleEntityStart ( groupNotify.getValue(), (GroupHandle) assembly, THIS_UNKNOWN, _messageNotify );

  // If we get here then we couldn't find an appropriate interface.
  // We let the controller decide whether or not to continue.
  return ERROR ( FORMAT ( "Failed to start assembly '%s' at level %d.\n\tNo known interface available from target.", assembly->name(), _current->getLevel() ), CadKit::NO_INTERFACE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  End an assembly.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_endAssembly ( eaiAssembly *assembly )
{
  SL_PRINT3 ( "In DbJtDatabase::_endAssembly(), assembly = %X, level = %d\n", assembly, _current->getLevel() );
  SL_ASSERT ( assembly );

  // Try this interface.
  SlQueryPtr<IAssemblyNotify> assemblyNotify ( _target );
  if ( assemblyNotify.isValid() )
    return CadKit::handleEntityEnd ( assemblyNotify.getValue(), (AssemblyHandle) assembly, THIS_UNKNOWN, _messageNotify );

  // Try this interface.
  SlQueryPtr<IGroupNotify> groupNotify ( _target );
  if ( groupNotify.isValid() )
    return CadKit::handleEntityEnd ( groupNotify.getValue(), (GroupHandle) assembly, THIS_UNKNOWN, _messageNotify );

  // If we get here then we couldn't find an appropriate interface.
  // We let the controller decide whether or not to continue.
  return ERROR ( FORMAT ( "Failed to end assembly '%s' at level %d.\n\tNo known interface available from target.", assembly->name(), _current->getLevel() ), CadKit::NO_INTERFACE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start a part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_startPart ( eaiPart *part )
{
  SL_PRINT3 ( "In DbJtDatabase::_startPart(), part = %X, level = %d\n", part, _current->getLevel() );
  SL_ASSERT ( part );

  if ( false == PROGRESS ( FORMAT ( "    Part, level %2d, name: %s", _current->getLevel(), part->name() ) ) )
    return false;

  // Try this interface.
  SlQueryPtr<IPartNotify> partNotify ( _target );
  if ( partNotify.isValid() )
  {
    if ( false == CadKit::handleEntityStart ( partNotify.getValue(), (PartHandle) part, THIS_UNKNOWN, _messageNotify ) )
      return false;
  }

  // Try this interface.
  SlQueryPtr<IGroupNotify> groupNotify ( _target );
  if ( groupNotify.isValid() )
  {
    if ( false == CadKit::handleEntityStart ( groupNotify.getValue(), (GroupHandle) part, THIS_UNKNOWN, _messageNotify ) )
      return false;
  }

  // If we get here then we couldn't find an appropriate interface.
  // We let the controller decide whether or not to continue.
  if ( false == ERROR ( FORMAT ( "Failed to start part '%s' at level %d.\n\tNo known interface available from target.", part->name(), _current->getLevel() ), CadKit::NO_INTERFACE ) )
    return false;

  // Process the LODs. 
  return this->_processLods ( part );
}


///////////////////////////////////////////////////////////////////////////////
//
//  End a part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_endPart ( eaiPart *part )
{
  SL_PRINT3 ( "In DbJtDatabase::_endPart(), part = %X, level = %d\n", part, _current->getLevel() );
  SL_ASSERT ( part );

  // Try this interface.
  SlQueryPtr<IPartNotify> partNotify ( _target );
  if ( partNotify.isValid() )
    return CadKit::handleEntityEnd ( partNotify.getValue(), (PartHandle) part, THIS_UNKNOWN, _messageNotify );

  // Try this interface.
  SlQueryPtr<IGroupNotify> groupNotify ( _target );
  if ( groupNotify.isValid() )
    return CadKit::handleEntityEnd ( groupNotify.getValue(), (GroupHandle) part, THIS_UNKNOWN, _messageNotify );

  // If we get here then we couldn't find an appropriate interface.
  // We let the controller decide whether or not to continue.
  return ERROR ( FORMAT ( "Failed to end part '%s' at level %d.\n\tNo known interface available from target.", part->name(), _current->getLevel() ), CadKit::NO_INTERFACE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start an instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_startInstance ( eaiInstance *instance )
{
  SL_PRINT3 ( "In DbJtDatabase::_startInstance(), instance = %X, level = %d\n", instance, _current->getLevel() );
  SL_ASSERT ( instance );

  if ( false == PROGRESS ( FORMAT ( "Instance, level %2d, name: %s", _current->getLevel(), instance->name() ) ) )
    return false;

  // Try this interface.
  SlQueryPtr<IInstanceNotify> instanceNotify ( _target );
  if ( instanceNotify.isValid() )
    return CadKit::handleEntityStart ( instanceNotify.getValue(), (InstanceHandle) instance, THIS_UNKNOWN, _messageNotify );

  // Try this interface.
  SlQueryPtr<IGroupNotify> groupNotify ( _target );
  if ( groupNotify.isValid() )
    return CadKit::handleEntityStart ( groupNotify.getValue(), (GroupHandle) instance, THIS_UNKNOWN, _messageNotify );

  // If we get here then we couldn't find an appropriate interface.
  // We let the controller decide whether or not to continue.
  return ERROR ( FORMAT ( "Failed to start instance '%s' at level %d.\n\tNo known interface available from target.", instance->name(), _current->getLevel() ), CadKit::NO_INTERFACE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  End an instance.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_endInstance ( eaiInstance *instance )
{
  SL_PRINT3 ( "In DbJtDatabase::_endInstance(), instance = %X, level = %d\n", instance, _current->getLevel() );
  SL_ASSERT ( instance );

  // Try this interface.
  SlQueryPtr<IInstanceNotify> instanceNotify ( _target );
  if ( instanceNotify.isValid() )
    return CadKit::handleEntityEnd ( instanceNotify.getValue(), (InstanceHandle) instance, THIS_UNKNOWN, _messageNotify );

  // Try this interface.
  SlQueryPtr<IGroupNotify> groupNotify ( _target );
  if ( groupNotify.isValid() )
    return CadKit::handleEntityEnd ( groupNotify.getValue(), (GroupHandle) instance, THIS_UNKNOWN, _messageNotify );

  // If we get here then we couldn't find an appropriate interface.
  // We let the controller decide whether or not to continue.
  return ERROR ( FORMAT ( "Failed to end instance '%s' at level %d.\n\tNo known interface available from target.", instance->name(), _current->getLevel() ), CadKit::NO_INTERFACE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process all the LODs of the given part.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_processLods ( eaiPart *part )
{
  SL_PRINT3 ( "In DbJtDatabase::_processLods(), this = %X, part = %X\n", this, part );
  SL_ASSERT ( part );
  SL_ASSERT ( UNSET_INDEX == _current->getLod() );
  SL_ASSERT ( UNSET_INDEX == _current->getShape() );
  SL_ASSERT ( UNSET_INDEX == _current->getPrim() );
  SL_ASSERT ( UNSET_INDEX == _current->getSet() );

  // Initialize.
  this->_resetStateIndices();

  // Get the number of LODs.
  bool ( DbJtDatabase::*processLod ) ( eaiPart *, int ) = &DbJtDatabase::_processShapeLod;
  int numLods = part->numPolyLODs();

  // If there are no Poly LODs, then check for Primitive LODs.
  // If neither exists, then return, otherwise the logic below gets more 
  // complicated. It is not an error to have zero LODs.
  if ( numLods <= 0 )
  {
    // Get number of primitive LODs.
    numLods = part->numPrimLODs();
    if ( numLods <= 0 )
      return true;
    else
      processLod = &DbJtDatabase::_processPrimLod;
  }

  // We start the loop at zero unless we only want the lowest LOD, 
  // in which case we start at the last LOD.
  int start = ( PROCESS_LOW_LOD == _lodOption ) ? numLods - 1 : 0;

  // We end the loop at the last LOD unless we only want the highest LOD,
  // in which case we end at the first LOD.
  int end = ( PROCESS_HIGH_LOD == _lodOption ) ? 1 : numLods;

  PROGRESS_LEVEL ( 2 ) ( FORMAT ( "\tLODs: %d", numLods ) );

  // Loop through the LODs.
  for ( int i = start; i < end; ++i )
  {
    // Set the current indices.
    _current->setLod ( i );
    _current->setShape ( UNSET_INDEX );
    _current->setPrim ( UNSET_INDEX );
    _current->setSet ( UNSET_INDEX );

    // Process the shapes.
    if ( false == (*this.*processLod) ( part, i ) )
      if ( false == ERROR ( FORMAT ( "Failed to process LOD %d in part '%s'", i, part->name() ), CadKit::FAILED ) )
        return false;
  }

  // If we get to here then it worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process all the shapes of the given LOD.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_processShapeLod ( eaiPart *part, int whichLod )
{
  SL_PRINT4 ( "In DbJtDatabase::_processShapeLod(), this = %X, part = %X, whichLod = %d\n", this, part, whichLod );
  SL_ASSERT ( part );
  SL_ASSERT ( whichLod >= 0 && whichLod < part->numPolyLODs() );
  SL_ASSERT ( whichLod == _current->getLod() );
  SL_ASSERT ( UNSET_INDEX == _current->getShape() );
  SL_ASSERT ( UNSET_INDEX == _current->getSet() );

  // We have to initialize the shape data each time we process a new LOD.
  _shapeData->init ( NULL );

  // Try this interface.
  SlQueryPtr<ILodNotify> notify ( _target );
  if ( notify.isValid() )
    if ( false == notify->startEntity ( CadKit::makeLodHandle ( whichLod ), THIS_UNKNOWN ) )
      if ( false == ERROR ( FORMAT ( "Failed to start LOD %d in part '%s'", whichLod, part->name() ), CadKit::FAILED ) )
        return false;

  // Get the number of shapes for this LOD.
  int numShapes = part->numPolyShapes ( whichLod );

  PROGRESS_LEVEL ( 3 ) ( FORMAT ( "\t\tShapes: %d", numShapes ) );

  // Loop through the shapes.
  for ( int i = 0; i < numShapes; ++i )
  {
    // Set the current indices.
    _current->setShape ( i );
    _current->setSet ( UNSET_INDEX );

    // Process the shape.
    if ( false == this->_processShape ( part, whichLod, i ) )
      if ( false == ERROR ( FORMAT ( "Failed to process shape %d, LOD %d, part '%s'", i, whichLod, part->name() ), CadKit::FAILED ) )
        return false;
  }

  // Try this interface.
  if ( notify.isValid() )
    if ( false == notify->endEntity ( CadKit::makeLodHandle ( whichLod ), THIS_UNKNOWN ) )
      if ( false == ERROR ( FORMAT ( "Failed to end LOD %d in part '%s'", whichLod, part->name() ), CadKit::FAILED ) )
        return false;

  // If we get to here then it worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process all the primitives of the given LOD.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_processPrimLod ( eaiPart *part, int whichLod )
{
  SL_PRINT4 ( "In DbJtDatabase::_processPrimLod(), this = %X, part = %X, whichLod = %d\n", this, part, whichLod );
  SL_ASSERT ( part );
  SL_ASSERT ( whichLod >= 0 && whichLod < part->numPrimLODs() );
  SL_ASSERT ( whichLod == _current->getLod() );
  SL_ASSERT ( UNSET_INDEX == _current->getPrim() );
  SL_ASSERT ( UNSET_INDEX == _current->getSet() );

  // We have to initialize the primitive data each time we process a new LOD.
  _primData->init ( NULL );

  // Try this interface.
  SlQueryPtr<ILodNotify> notify ( _target );
  if ( notify.isValid() )
    if ( false == notify->startEntity ( CadKit::makeLodHandle ( whichLod ), THIS_UNKNOWN ) )
      if ( false == ERROR ( FORMAT ( "Failed to start LOD %d in part '%s'", whichLod, part->name() ), CadKit::FAILED ) )
        return false;

  // Get the number of shapes for this LOD.
  int numPrims = part->numPrimShapes ( whichLod );

  PROGRESS_LEVEL ( 3 ) ( FORMAT ( "\t\tPrimitives: %d", numPrims ) );

  // Loop through the shapes.
  for ( int i = 0; i < numPrims; ++i )
  {
    // Set the current indices.
    _current->setPrim ( i );
    _current->setSet ( UNSET_INDEX );

    // Process the primitive.
    if ( false == this->_processPrim ( part, whichLod, i ) )
      if ( false == ERROR ( FORMAT ( "Failed to process shape %d, LOD %d, part '%s'", i, whichLod, part->name() ), CadKit::FAILED ) )
        return false;
  }

  // Try this interface.
  if ( notify.isValid() )
    if ( false == notify->endEntity ( CadKit::makeLodHandle ( whichLod ), THIS_UNKNOWN ) )
      if ( false == ERROR ( FORMAT ( "Failed to end LOD %d in part '%s'", whichLod, part->name() ), CadKit::FAILED ) )
        return false;

  // If we get to here then it worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process all the sets of the given shape.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_processShape ( eaiPart *part, int whichLod, int whichShape )
{
  SL_PRINT4 ( "In DbJtDatabase::_processShape(), this = %X, part = %X, whichLod = %d\n", this, part, whichLod );
  SL_ASSERT ( part );
  SL_ASSERT ( whichLod >= 0 && whichLod < part->numPolyLODs() );
  SL_ASSERT ( whichShape >= 0 && whichShape < part->numPolyShapes ( whichLod ) );
  SL_ASSERT ( whichLod == _current->getLod() );
  SL_ASSERT ( whichShape == _current->getShape() );
  SL_ASSERT ( UNSET_INDEX == _current->getSet() );

  // Try this interface.
  SlQueryPtr<IShapeNotify> notify ( _target );
  if ( notify.isValid() )
    if ( false == notify->startEntity ( CadKit::makeShapeHandle ( whichShape ), THIS_UNKNOWN ) )
      if ( false == ERROR ( FORMAT ( "Failed to start shape %d, LOD %d, part '%s'", whichShape, whichLod, part->name() ), CadKit::FAILED ) )
        return false;
  
  // Get the shape.
  SlRefPtr<eaiShape> shape = this->_getShape ( part, whichLod, whichShape );
  if ( shape.isNull() )
    return ERROR ( FORMAT ( "Failed to get shape %d, LOD %d, part '%s'", whichShape, whichLod, part->name() ), CadKit::FAILED );

  // Get the number of sets for this shape.
  int numSets = shape->numOfSets();

  // Loop through the sets.
  for ( int i = 0; i < numSets; ++i )
  {
    // Set the current index.
    _current->setSet ( i );

    // Process the set.
    if ( false == this->_processSet ( shape, whichShape, i ) )
      if ( false == ERROR ( FORMAT ( "Failed to process vertices for set %d, shape %d, LOD %d, part '%s'", i, whichShape, whichLod, part->name() ), CadKit::FAILED ) )
        return false;
  }

  // Try this interface.
  if ( notify.isValid() )
    if ( false == notify->endEntity ( CadKit::makeShapeHandle ( whichShape ), THIS_UNKNOWN ) )
      if ( false == ERROR ( FORMAT ( "Failed to end shape %d, LOD %d, part '%s'", whichShape, whichLod, part->name() ), CadKit::FAILED ) )
        return false;

  // If we get to here then it worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process all the sets of the given primitive.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_processPrim ( eaiPart *part, int whichLod, int whichPrim )
{
  SL_PRINT4 ( "In DbJtDatabase::_processPrim(), this = %X, part = %X, whichLod = %d\n", this, part, whichLod );
  SL_ASSERT ( part );
  SL_ASSERT ( whichLod >= 0 && whichLod < part->numPrimLODs() );
  SL_ASSERT ( whichPrim >= 0 && whichPrim < part->numPrimShapes ( whichLod ) );
  SL_ASSERT ( whichLod == _current->getLod() );
  SL_ASSERT ( whichPrim == _current->getPrim() );
  SL_ASSERT ( UNSET_INDEX == _current->getSet() );

  // Try this interface.
  SlQueryPtr<IPrimNotify> notify ( _target );
  if ( notify.isValid() )
    if ( false == notify->startEntity ( CadKit::makePrimHandle ( whichPrim ), THIS_UNKNOWN ) )
      if ( false == ERROR ( FORMAT ( "Failed to start primitive %d, LOD %d, part '%s'", whichPrim, whichLod, part->name() ), CadKit::FAILED ) )
        return false;
  
  // Get the shape.
  SlRefPtr<eaiPrim> prim = this->_getPrim ( part, whichLod, whichPrim );
  if ( prim.isNull() )
    return ERROR ( FORMAT ( "Failed to get primitive %d, LOD %d, part '%s'", whichPrim, whichLod, part->name() ), CadKit::FAILED );

  // Get the number of sets for this shape.
  int numSets = prim->numOfSets();

  // Loop through the sets.
  for ( int i = 0; i < numSets; ++i )
  {
    // Set the current index.
    _current->setSet ( i );

    // Process the set.
    if ( false == this->_processSet ( prim, whichPrim, i ) )
      if ( false == ERROR ( FORMAT ( "Failed to process vertices for set %d, primitive %d, LOD %d, part '%s'", i, whichPrim, whichLod, part->name() ), CadKit::FAILED ) )
        return false;
  }

  // Try this interface.
  if ( notify.isValid() )
    if ( false == notify->endEntity ( CadKit::makePrimHandle ( whichPrim ), THIS_UNKNOWN ) )
      if ( false == ERROR ( FORMAT ( "Failed to end primitive %d, LOD %d, part '%s'", whichPrim, whichLod, part->name() ), CadKit::FAILED ) )
        return false;

  // If we get to here then it worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process all the vertices of the given set.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_processSet ( eaiShape *shape, int whichShape, int whichSet )
{
  SL_PRINT4 ( "In DbJtDatabase::_processSet(), this = %X, shape = %X, whichSet = %d\n", this, shape, whichSet );
  SL_ASSERT ( shape );
  SL_ASSERT ( whichSet >= 0 && whichSet < shape->numOfSets() );
  SL_ASSERT ( UNSET_INDEX != _current->getLod() );
  SL_ASSERT ( UNSET_INDEX != _current->getShape() );
  SL_ASSERT ( whichSet == _current->getSet() );

  // Try this interface.
  SlQueryPtr<ISetNotify> set ( _target );
  if ( set.isValid() )
  {
    if ( false == set->startEntity ( CadKit::makeSetHandle ( whichSet ), THIS_UNKNOWN ) )
    {
      if ( false == ERROR ( FORMAT ( "Failed to start set %d, shape %d, LOD %d, part '%s'", whichSet, _current->getShape(), _current->getLod(), _current->getPart()->name() ), CadKit::FAILED ) )
        return false;
    }
  }

  // If that interface isn't supported...
  else
  {
    // If this set is a tri-strip...
    if ( eaiEntity::eaiTRISTRIPSET == shape->typeID() )
    {
      // Try this interface.
      SlQueryPtr<ITriangleAppendFloat> triangle ( _target );
      if ( triangle.isValid() )
      {
        // Append the triangles to the source.
        if ( false == this->_appendTriangles ( CadKit::makeShapeHandle ( whichShape ), whichSet ) )
        {
          if ( false == ERROR ( FORMAT ( "Failed to append triangles from tri-strip for set %d, shape %d, LOD %d, part '%s'", whichSet, _current->getShape(), _current->getLod(), _current->getPart()->name() ), CadKit::FAILED ) )
            return false;
        }
      }
    }
  }

  // Try this interface.
  if ( set.isValid() )
  {
    if ( false == set->endEntity ( CadKit::makeSetHandle ( whichSet ), THIS_UNKNOWN ) )
    {
      if ( false == ERROR ( FORMAT ( "Failed to end set %d, shape %d, LOD %d, part '%s'", whichSet, _current->getShape(), _current->getLod(), _current->getPart()->name() ), CadKit::FAILED ) )
        return false;
    }
  }

  // If we get to here then it worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process all the primitives of the given set.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_processSet ( eaiPrim *prim, int whichPrim, int whichSet )
{
  SL_PRINT4 ( "In DbJtDatabase::_processSet(), this = %X, shape = %X, whichSet = %d\n", this, prim, whichSet );
  SL_ASSERT ( prim );
  SL_ASSERT ( whichSet >= 0 && whichSet < prim->numOfSets() );
  SL_ASSERT ( UNSET_INDEX != _current->getLod() );
  SL_ASSERT ( UNSET_INDEX != _current->getPrim() );
  SL_ASSERT ( whichSet == _current->getSet() );

  // Try this interface.
  SlQueryPtr<ISetNotify> set ( _target );
  if ( set.isValid() )
  {
    if ( false == set->startEntity ( CadKit::makeSetHandle ( whichSet ), THIS_UNKNOWN ) )
    {
      if ( false == ERROR ( FORMAT ( "Failed to start set %d, primitive %d, LOD %d, part '%s'", whichSet, _current->getPrim(), _current->getLod(), _current->getPart()->name() ), CadKit::FAILED ) )
        return false;
    }
  }

  // Try this interface.
  if ( set.isValid() )
  {
    if ( false == set->endEntity ( CadKit::makeSetHandle ( whichSet ), THIS_UNKNOWN ) )
    {
      if ( false == ERROR ( FORMAT ( "Failed to end set %d, primitive %d, LOD %d, part '%s'", whichSet, _current->getPrim(), _current->getLod(), _current->getPart()->name() ), CadKit::FAILED ) )
        return false;
    }
  }

  // If we get to here then it worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process all the vertices of the given set.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_appendTriangles ( ShapeHandle shape, int whichSet )
{
  SL_PRINT4 ( "In DbJtDatabase::_appendTriangles(), this = %X, shape = %X, whichSet = %d\n", this, shape, whichSet );
  SL_ASSERT ( shape );
  SL_ASSERT ( UNSET_INDEX != _current->getLod() );
  SL_ASSERT ( UNSET_INDEX != _current->getShape() );
  SL_ASSERT ( whichSet == _current->getSet() );

  // Check before calling this function. This keeps definition of 
  // ITriangleAppendFloat out of this class's header file.
  SlQueryPtr<ITriangleAppendFloat> triangle ( _target );
  SL_ASSERT ( triangle.isValid() );

  // Used frequently below.
  SlRefPtr<CadKit::IUnknown> unknown ( this->queryInterface ( CadKit::IUnknown::IID ) );

  // Set the shape data. Even though we already have a pointer to the 
  // eaiShape, we make a handle so that the correct _setShapeData() 
  // is called.
  if ( false == this->_setShapeData ( shape ) )
    return false;

  // Get the number of vertices.
  unsigned int numVertices ( _shapeData->getVertices().size ( whichSet ) );

  // Should be true.
  SL_ASSERT ( numVertices >= 3 );

  // Grab the first two vertices.
  SlVec3f v0 ( _shapeData->getVertices() ( whichSet, 0 ) );
  SlVec3f v1 ( _shapeData->getVertices() ( whichSet, 1 ) ), v2;

  // Get the matrix that transforms the vertices from local to global.
  SlMatrix44f localToGlobal ( true );
  _matrixStack.multiply ( localToGlobal );

  // Convert from local to global coordinates.
  v0 = localToGlobal * v0;
  v1 = localToGlobal * v1;

  // Loop through the vertices.
  for ( unsigned int i = 2; i < numVertices; ++i )
  {
    // Get the next triangle.
    v2 = _shapeData->getVertices() ( whichSet, i );

    // Convert from local to global coordinates.
    v2 = localToGlobal * v2;

    // Append the triangle.
    triangle->appendTriangle ( 
      v0[0], v0[1], v0[2],
      v1[0], v1[1], v1[2],
      v2[0], v2[1], v2[2],
      unknown );

    // Swap vertices so that we get the 3rd vertex of a new triangle next 
    // time. Note: we have to swap differently every other time in order
    // to maintain consistant ordering.
    if ( 0 == i % 2 )
      v0 = v2;
    else
      v1 = v2;
  }

  // If we get to here then it worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the assembly load option.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::setAssemblyLoadOption ( const AssemblyLoadOption &option )
{
  SL_PRINT3 ( "In DbJtDatabase::setAssemblyLoadOption(), this = %X, option = %d\n", this, option );

  // Set the option.
  _assemblyLoadOption = option;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the brep load option.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::setBrepLoadOption ( const BrepLoadOption &option )
{
  SL_PRINT3 ( "In DbJtDatabase::setBrepLoadOption(), this = %X, option = %d\n", this, option );

  // Set the option.
  _brepLoadOption = option;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the part load option.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::setPartLoadOption ( const PartLoadOption &option )
{
  SL_PRINT3 ( "In DbJtDatabase::setPartLoadOption(), this = %X, option = %d\n", this, option );

  // Set the option.
  _partLoadOption = option;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the shape load option.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::setShapeLoadOption ( const ShapeLoadOption &option )
{
  SL_PRINT3 ( "In DbJtDatabase::setShapeLoadOption(), this = %X, option = %d\n", this, option );

  // Set the option.
  _shapeLoadOption = option;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::_pushMatrix ( eaiHierarchy *hierarchy )
{
  SL_PRINT3 ( "In DbJtDatabase::_pushMatrix(), this = %X, hierarchy = %X\n", this, hierarchy );
  SL_ASSERT ( hierarchy );

  // Get the transformation.
  SlMatrix44f matrix ( true );
  CadKit::getTransform ( _truncate.getLow(), _truncate.getHigh(), hierarchy, matrix );

  // Push it onto our stack of matrices.
  _matrixStack.push ( matrix );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::_popMatrix()
{
  SL_PRINT2 ( "In DbJtDatabase::_popMatrix(), this = %X\n", this );

  // Pop the top matrix from the stack.
  _matrixStack.pop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the assembly.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::_pushAssembly ( eaiAssembly *assembly )
{
  SL_PRINT3 ( "In DbJtDatabase::_pushAssembly(), this = %X, assembly = %X\n", this, assembly );
  SL_ASSERT ( assembly );

  // Push it onto our stack.
  _assemblies->push_front ( assembly );

  // This is not a stack of ref-pointers. We have to manually reference it.
  assembly->ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the assembly.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::_popAssembly()
{
  SL_PRINT2 ( "In DbJtDatabase::_popAssembly(), this = %X\n", this );
  SL_ASSERT ( false == _assemblies->empty() );

  // This is not a stack of ref-pointers. We have to manually unreference it.
  this->_getCurrentAssembly()->unref();

  // Pop it off of our stack.
  _assemblies->pop_front();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current assembly.
//
///////////////////////////////////////////////////////////////////////////////

eaiAssembly *DbJtDatabase::_getCurrentAssembly() const
{
  SL_ASSERT ( NULL != _assemblies.get() );

  // We are pushing the assemblies onto the front of the list. 
  // Therefore, the current assembly will be on the front.
  return ( _assemblies->empty() ) ? NULL : _assemblies->front();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the top-level assembly.
//
///////////////////////////////////////////////////////////////////////////////

eaiAssembly *DbJtDatabase::_getTopAssembly() const
{
  SL_ASSERT ( NULL != _assemblies.get() );

  // We are pushing the assemblies onto the front of the list.
  // Therefore, the top assembly will be on the back.
  return ( _assemblies->empty() ) ? NULL : _assemblies->back();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current part.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::_setCurrentPart ( eaiPart *part )
{
  SL_PRINT3 ( "In DbJtDatabase::_setCurrentPart(), this = %X, part = %X\n", this, part );

  // Set the current part.
  _current->setPart ( part );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current instance.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::_setCurrentInstance ( eaiInstance *instance )
{
  SL_PRINT3 ( "In DbJtDatabase::_setCurrentInstance(), this = %X, instance = %X\n", this, instance );

  // Set the current instance.
  _current->setInstance ( instance );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current part.
//
///////////////////////////////////////////////////////////////////////////////

eaiPart *DbJtDatabase::_getCurrentPart() const
{
  SL_PRINT2 ( "In DbJtDatabase::_getCurrentPart(), this = %X\n", this );
  return _current->getPart();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current instance.
//
///////////////////////////////////////////////////////////////////////////////

eaiInstance *DbJtDatabase::_getCurrentInstance() const
{
  SL_PRINT2 ( "In DbJtDatabase::_getCurrentInstance(), this = %X\n", this );
  return _current->getInstance();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string DbJtDatabase::getName ( AssemblyHandle assembly ) const { return CadKit::getName ( (eaiEntity *) assembly ); }
std::string DbJtDatabase::getName ( PartHandle part )         const { return CadKit::getName ( (eaiEntity *) part ); }
std::string DbJtDatabase::getName ( InstanceHandle instance ) const { return CadKit::getName ( (eaiEntity *) instance ); }


///////////////////////////////////////////////////////////////////////////////
//
//  Get the assembly's material.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getMaterial ( AssemblyHandle assembly, SlMaterialf &material, bool tryParents ) const
{
  // Handle trivial case. Do not assert.
  if ( NULL == assembly )
    return false;

  // Try the given entity.
  if ( true == CadKit::getMaterial ( _truncate.getLow(), _truncate.getHigh(), (eaiEntity *) assembly, material ) )
    return true;

  // Bail now if we aren't supposed to try the parents.
  if ( false == tryParents )
    return false;

  // If we get to here then recursively try the parent (if it isn't null).
  AssemblyHandle parent = this->getParent ( assembly );
  return ( parent ) ? this->getMaterial ( parent, material, tryParents ) : false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the part's material.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getMaterial ( PartHandle part, SlMaterialf &material, bool tryParents ) const
{
  // Try the given entity.
  if ( true == CadKit::getMaterial ( _truncate.getLow(), _truncate.getHigh(), (eaiEntity *) part, material ) )
    return true;

  // Bail now if we aren't supposed to try the parents.
  if ( false == tryParents )
    return false;

  // If we get to here then try the parent assembly (which may be null).
  AssemblyHandle parent = this->getParent ( part );
  return ( parent ) ? this->getMaterial ( parent, material, tryParents ) : false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the instance's material.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getMaterial ( InstanceHandle instance, SlMaterialf &material, bool tryParents ) const
{
  // Try the given entity.
  if ( true == CadKit::getMaterial ( _truncate.getLow(), _truncate.getHigh(), (eaiEntity *) instance, material ) )
    return true;

  // Bail now if we aren't supposed to try the parents.
  if ( false == tryParents )
    return false;

  // If we get to here then try the parent assembly.
  AssemblyHandle parent = this->getParent ( instance );
  SL_ASSERT ( parent );
  return this->getMaterial ( parent, material, tryParents );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the shape's material.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getMaterial ( ShapeHandle shape, SlMaterialf &material, bool tryParents ) const
{
  // Get the shape.
  SlRefPtr<eaiShape> temp = this->_getShape ( shape );
  if ( temp.isNull() )
  {
    SL_ASSERT ( 0 ); // Why did this happen?
    return false;
  }

  // Try the given entity.
  if ( true == CadKit::getMaterial ( _truncate.getLow(), _truncate.getHigh(), (eaiEntity *) temp.getValue(), material ) )
    return true;

  // Bail now if we aren't supposed to try the parents.
  if ( false == tryParents )
    return false;

  // If we get to here then try the parent part.
  LodHandle lod = this->getParent ( shape );
  SL_ASSERT ( lod );
  PartHandle part = this->getParent ( lod );
  return this->getMaterial ( part, material, tryParents );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive's material.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getMaterial ( PrimHandle prim, SlMaterialf &material, bool tryParents ) const
{
  // Get the shape.
  SlRefPtr<eaiPrim> temp = this->_getPrim ( prim );
  if ( temp.isNull() )
  {
    SL_ASSERT ( 0 ); // Why did this happen?
    return false;
  }

  // Try the given entity.
  if ( true == CadKit::getMaterial ( _truncate.getLow(), _truncate.getHigh(), (eaiEntity *) temp.getValue(), material ) )
    return true;

  // Bail now if we aren't supposed to try the parents.
  if ( false == tryParents )
    return false;

  // If we get to here then try the parent part.
  LodHandle lod = this->getParent ( prim );
  SL_ASSERT ( lod );
  PartHandle part = this->getParent ( lod );
  return this->getMaterial ( part, material, tryParents );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the assembly's transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getTransform ( AssemblyHandle assembly, SlMatrix44f &matrix, bool tryParents ) const
{
  // TODO. Handle "tryParents".
  bool result = CadKit::getTransform ( _truncate.getLow(), _truncate.getHigh(), (eaiEntity *) assembly, matrix );

  // The first time we scale & rotate, even if the above didn't work.
  if ( this->_doWeNeedToScale() )
  {
    this->_applyScaleOnce ( matrix );
    if( this->_doWeNeedToRotate() )
      this->_applyRotateOnce( matrix );
    return true;
  }

  // Return the result.
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the part's transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getTransform ( PartHandle part, SlMatrix44f &matrix, bool tryParents ) const
{
  // TODO. Handle "tryParents".
  bool result = CadKit::getTransform ( _truncate.getLow(), _truncate.getHigh(), (eaiEntity *) part, matrix );

  // The first time we scale, even if the above didn't work.
  if ( this->_doWeNeedToScale() )
  {
    this->_applyScaleOnce ( matrix );
    if( this->_doWeNeedToRotate() )
      this->_applyRotateOnce ( matrix );
    return true;
  }

  // Return the result.
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the instance's transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getTransform ( InstanceHandle instance, SlMatrix44f &matrix, bool tryParents ) const
{
  // TODO. Handle "tryParents".
  return CadKit::getTransform ( _truncate.getLow(), _truncate.getHigh(), (eaiEntity *) instance, matrix );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getTexture ( ShapeHandle shape, std::vector<unsigned char> &texture, bool tryParents ) const
{
  return false; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the parent.
//
///////////////////////////////////////////////////////////////////////////////

AssemblyHandle DbJtDatabase::getParent ( AssemblyHandle assembly ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getParent(), this = %X, assembly = %X\n", this, assembly );

  // Handle trivial cases.
  if ( NULL == assembly || true == _assemblies->empty() )
  {
    // For the usage that I have in mind, this should not happen.
    // If that proves not to be the case, then take this assertion out.
    SL_ASSERT ( 0 );
    return NULL;
  }

  // Make sure it is an assembly.
  if ( false == CadKit::isValidHandle ( assembly ) )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return NULL;
  }

  // Is this the top-level assembly?
  if ( ((eaiAssembly *) assembly) == this->_getTopAssembly() )
    return NULL;

  // Since we push/pop assemblies from the front of the list, the parent 
  // assembly will be the one directly after the given assembly.
  Assemblies::iterator i = std::find ( _assemblies->begin(), _assemblies->end(), (eaiAssembly *) assembly );

  // Should be true.
  SL_ASSERT ( i != _assemblies->end() );

  // This iterator is the one before the one we want. So increment one.
  ++i;

  // Should still be true.
  SL_ASSERT ( i != _assemblies->end() );

  // Grab the parent.
  eaiAssembly *parent = *i;

  // When we get to here this should be true.
  SL_ASSERT ( parent );

  // Return the parent.
  return (AssemblyHandle) parent;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the parent.
//
///////////////////////////////////////////////////////////////////////////////

AssemblyHandle DbJtDatabase::getParent ( PartHandle part ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getParent(), this = %X, part = %X\n", this, part );

  // Handle trivial cases.
  if ( NULL == part )
  {
    // For the usage that I have in mind, this should not happen.
    // If that proves not to be the case, then take this assertion out.
    SL_ASSERT ( 0 );
    return NULL;
  }

  // Make sure it is a part.
  if ( false == CadKit::isValidHandle ( part ) )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return NULL;
  }

  // We assume that the parent is our current assembly.
  eaiAssembly *parent = this->_getCurrentAssembly();
  return (AssemblyHandle) parent;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the parent.
//
///////////////////////////////////////////////////////////////////////////////

AssemblyHandle DbJtDatabase::getParent ( InstanceHandle instance ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getParent(), this = %X, instance = %X\n", this, instance );

  // Handle trivial cases.
  if ( NULL == instance || true == _assemblies->empty() )
  {
    // For the usage that I have in mind, this should not happen.
    // If that proves not to be the case, then take this assertion out.
    SL_ASSERT ( 0 );
    return NULL;
  }

  // Make sure it is an instance.
  if ( false == CadKit::isValidHandle ( instance ) )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return NULL;
  }

  // We assume that the parent is our current assembly.
  eaiAssembly *parent = this->_getCurrentAssembly();
  return (AssemblyHandle) parent;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the parent.
//
///////////////////////////////////////////////////////////////////////////////

PartHandle DbJtDatabase::getParent ( LodHandle lod ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getParent(), this = %X, lod = %d\n", this, lod );
  SL_ASSERT ( _current->getPart() );
  SL_ASSERT ( lod );

  // Get the index from the handle.
  int whichLod = CadKit::makeLodIndex ( lod );

  // Handle bad cases.
  if ( UNSET_INDEX == whichLod || 
       whichLod < 0 || 
       ( whichLod >= _current->getPart()->numPolyLODs() &&
         whichLod >= _current->getPart()->numPrimLODs() ) )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return NULL;
  }

  // We assume that the parent is our current part.
  return (PartHandle) _current->getPart();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the parent.
//
///////////////////////////////////////////////////////////////////////////////

LodHandle DbJtDatabase::getParent ( ShapeHandle shape ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getParent(), this = %X, shape = %d\n", this, shape );
  SL_ASSERT ( _current->getPart() );
  SL_ASSERT ( shape );

  // Get the index from the handle.
  int whichShape = CadKit::makeShapeIndex ( shape );

  // Handle bad cases.
  if ( UNSET_INDEX == _current->getLod() ||
       _current->getLod() < 0 || 
       _current->getLod() >= _current->getPart()->numPolyLODs() ||
       UNSET_INDEX == whichShape || 
       whichShape < 0 || 
       whichShape >= _current->getPart()->numPolyShapes ( _current->getLod() ) )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return NULL;
  }

  // We assume that the parent is our current lod.
  return CadKit::makeLodHandle ( _current->getLod() );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the parent.
//
///////////////////////////////////////////////////////////////////////////////

LodHandle DbJtDatabase::getParent ( PrimHandle prim ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getParent(), this = %X, prim = %d\n", this, prim );
  SL_ASSERT ( _current->getPart() );
  SL_ASSERT ( prim );

  // Get the index from the handle.
  int whichPrim = CadKit::makePrimIndex ( prim );

  // Handle bad cases.
  if ( UNSET_INDEX == _current->getLod() ||
       _current->getLod() < 0 || 
       _current->getLod() >= _current->getPart()->numPrimLODs() ||
       UNSET_INDEX == whichPrim || 
       whichPrim < 0 || 
       whichPrim >= _current->getPart()->numPrimShapes ( _current->getLod() ) )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return NULL;
  }

  // We assume that the parent is our current lod.
  return CadKit::makeLodHandle ( _current->getLod() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the parent.
//
///////////////////////////////////////////////////////////////////////////////

ShapeHandle DbJtDatabase::getParent ( SetHandle set ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getParent(), this = %X, set = %d\n", this, set );
  SL_ASSERT ( _current->getPart() );
  SL_ASSERT ( set );

  // Get the index from the handle.
  int whichSet = CadKit::makeSetIndex ( set );

  // Handle bad cases.
  if ( UNSET_INDEX == whichSet ||
       whichSet < 0 ||
       UNSET_INDEX == _current->getLod() ||
       _current->getLod() < 0 || 
       _current->getLod() >= _current->getPart()->numPolyLODs() ||
       UNSET_INDEX == _current->getShape() || 
       _current->getShape() < 0 || 
       _current->getShape() >= _current->getPart()->numPolyShapes ( _current->getLod() ) )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return NULL;
  }

  // Get the shape.
  SlRefPtr<eaiShape> shape = this->_getShape ( _current->getPart(), _current->getLod(), _current->getShape() );
  if ( shape.isNull() )
    return NULL;

  // See if the given set index is too big (we tested "too small" above).
  if ( whichSet >= shape->numOfSets() )
    return NULL;

  // We assume that the parent is our current shape.
  return CadKit::makeShapeHandle ( _current->getShape() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the corresponding part.
//
///////////////////////////////////////////////////////////////////////////////

PartHandle DbJtDatabase::getCorrespondingPart ( InstanceHandle instance ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getCorrespondingPart(), this = %X, instance = %X\n", this, instance );
  SL_ASSERT ( instance );

  // It has to be an instance.
  if ( false == CadKit::isValidHandle ( instance ) )
    return NULL;

  // Get a pointer to the original hierarchy (which may be null).
  eaiHierarchy *hierarchy = ((eaiInstance *) instance)->original();

  // It has to be a part.
  if ( false == CadKit::isPart ( hierarchy ) )
    return NULL;

  // Return the part.
  return (PartHandle) ((eaiPart *) hierarchy);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the corresponding assembly.
//
///////////////////////////////////////////////////////////////////////////////

AssemblyHandle DbJtDatabase::getCorrespondingAssembly ( InstanceHandle instance ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getCorrespondingAssembly(), this = %X, instance = %X\n", this, instance );
  SL_ASSERT ( instance );

  // It has to be an instance.
  if ( false == CadKit::isValidHandle ( instance ) )
    return NULL;

  // Get a pointer to the original hierarchy (which may be null).
  eaiHierarchy *hierarchy = ((eaiInstance *) instance)->original();

  // It has to be an assembly.
  if ( false == CadKit::isAssembly ( hierarchy ) )
    return NULL;

  // Return the assembly.
  return (AssemblyHandle) ((eaiAssembly *) hierarchy);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the state indices.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::_resetStateIndices()
{
  SL_PRINT2 ( "In DbJtDatabase::_resetStateIndices(), this = %X\n", this );

  // Reset.
  _current->setLod   ( UNSET_INDEX );
  _current->setShape ( UNSET_INDEX );
  _current->setPrim  ( UNSET_INDEX );
  _current->setSet   ( UNSET_INDEX );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the shape. The returned pointer will have a reference count of zero. 
//  The caller has to reference it (and ultimately dereference it).
//
///////////////////////////////////////////////////////////////////////////////

eaiShape *DbJtDatabase::_getShape ( eaiPart *part, int whichLod, int whichShape ) const
{
  SL_PRINT5 ( "In DbJtDatabase::_getShape(), this = %X, part = %X, whichLod = %d, whichShape = %d\n", this, part, whichLod, whichShape );
  SL_ASSERT ( part );
  SL_ASSERT ( whichLod >= 0 && whichLod < part->numPolyLODs() );
  SL_ASSERT ( whichShape >= 0 && whichShape < part->numPolyShapes ( whichLod ) );

  // Ask for the shape (there may not be one).
  eaiShape *shape = NULL;
  part->getPolyShape ( shape, whichLod, whichShape );

  // Return the shape (which may still be null).
  return shape;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the shape.
//
///////////////////////////////////////////////////////////////////////////////

eaiShape *DbJtDatabase::_getShape ( ShapeHandle shape ) const
{
  // Handle trivial case.
  if ( NULL == shape )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return false;
  }

  // Get the parent LOD.
  LodHandle lod = this->getParent ( shape );
  if ( NULL == lod )
  {
    SL_ASSERT ( 0 ); // Why did this happen?
    return false;
  }

  // Get the LOD's parent part.
  PartHandle part = this->getParent ( lod );
  if ( NULL == part )
  {
    SL_ASSERT ( 0 ); // Why did this happen?
    return false;
  }

  // Return the shape.
  return this->_getShape ( (eaiPart *) part, CadKit::makeLodIndex ( lod ), CadKit::makeShapeIndex ( shape ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive. The returned pointer will have a reference count of zero. 
//  The caller has to reference it (and ultimately dereference it).
//
///////////////////////////////////////////////////////////////////////////////

eaiPrim *DbJtDatabase::_getPrim ( eaiPart *part, int whichLod, int whichPrim ) const
{
  SL_PRINT5 ( "In DbJtDatabase::_getPrim(), this = %X, part = %X, whichLod = %d, whichPrim = %d\n", this, part, whichLod, whichPrim );
  SL_ASSERT ( part );
  SL_ASSERT ( whichLod >= 0 && whichLod < part->numPrimLODs() );
  SL_ASSERT ( whichPrim >= 0 && whichPrim < part->numPrimShapes ( whichLod ) );

  // Ask for the primitive (there may not be one).
  eaiPrim *prim = NULL;
  part->getPrimShape ( prim, whichLod, whichPrim );

  // Return the shape (which may still be null).
  return prim;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive.
//
///////////////////////////////////////////////////////////////////////////////

eaiPrim *DbJtDatabase::_getPrim ( PrimHandle prim ) const
{
  // Handle trivial case.
  if ( NULL == prim )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return false;
  }

  // Get the parent LOD.
  LodHandle lod = this->getParent ( prim );
  if ( NULL == lod )
  {
    SL_ASSERT ( 0 ); // Why did this happen?
    return false;
  }

  // Get the LOD's parent part.
  PartHandle part = this->getParent ( lod );
  if ( NULL == part )
  {
    SL_ASSERT ( 0 ); // Why did this happen?
    return false;
  }

  // Return the primitive.
  return this->_getPrim ( (eaiPart *) part, CadKit::makeLodIndex ( lod ), CadKit::makePrimIndex ( prim ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the shape data if needed. The shape data is mutable, which is why 
//  this functions can be const. It is called from other "get" functions 
//  which are also const.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_setShapeData ( ShapeHandle sh )
{
  SL_PRINT3 ( "In DbJtDatabase::_setShapeData(), this = %X, shape = %X\n", this, sh );

  // If the given shape is the one saved with the data then we're already set.
  if ( sh == _shapeData->getToken() )
    return true;

  // Initialize the shape data.
  _shapeData->init ( NULL );

  // Get the shape.
  SlRefPtr<eaiShape> shape = this->_getShape ( sh );
  if ( shape.isNull() )
  {
    SL_ASSERT ( 0 ); // Why did this happen?
    return false;
  }

  // Call the other one.
  if ( false == this->_setShapeData ( shape ) )
    return false;

  // Set the new token.
  _shapeData->setToken ( sh );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the shape data if needed. The shape data is mutable, which is why 
//  this functions can be const. It is called from other "get" functions 
//  which are also const.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_setShapeData ( eaiShape *shape )
{
  SL_PRINT3 ( "In DbJtDatabase::_setShapeData(), this = %X, shape = %X\n", this, shape );
  SL_ASSERT ( NULL != _current.get() );
  SL_ASSERT ( NULL != _current->getPart() );

  // Handle trivial case.
  if ( NULL == shape )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return false;
  }

  // Initialize the shape data.
  _shapeData->init ( NULL );

  // For convenience.
  std::vector<SlVec3f> &vertices  = _shapeData->getVertices().getData();
  std::vector<SlVec3f> &normals   = _shapeData->getNormals().getData();
  std::vector<SlVec3f> &colors    = _shapeData->getColors().getData();
  std::vector<SlVec2f> &texCoords = _shapeData->getTexCoords().getData();

  std::vector<unsigned int> &numVertices  = _shapeData->getVertices().getSizes();
  std::vector<unsigned int> &numNormals   = _shapeData->getNormals().getSizes();
  std::vector<unsigned int> &numColors    = _shapeData->getColors().getSizes();
  std::vector<unsigned int> &numTexCoords = _shapeData->getTexCoords().getSizes();

  std::vector<unsigned int> &startOfVertices  = _shapeData->getVertices().getStarts();
  std::vector<unsigned int> &startOfNormals   = _shapeData->getNormals().getStarts();
  std::vector<unsigned int> &startOfColors    = _shapeData->getColors().getStarts();
  std::vector<unsigned int> &startOfTexCoords = _shapeData->getTexCoords().getStarts();

  // Used in the loop.
  int vertexCount ( -1 ), normalCount ( -1 ), colorCount ( -1 ), textureCount ( -1 );
  bool gotVertices ( false );

  // Get the number of sets.
  int numSets ( shape->numOfSets() );

  PROGRESS_LEVEL ( 4 ) ( FORMAT ( "\t\t\tSets: %d", numSets ) );

  // Loop through all the sets.
  for ( int i = 0; i < numSets; ++i )
  {
    // Initialize.
    vertexCount = normalCount = colorCount = textureCount = 0;
    gotVertices = false;

    // Keep this in the loop. The destructor will delete the internal array.
    DbJtVisApiArray<float> v, n, c, t;

    // Get the arrays for this set of the shape. This function is the reason 
    // we jump through all these hoops (storing shape data and associating it 
    // with the handle). If we could ask for just the vertices, or just the 
    // normals (etc.) then this methodology could be eliminated and the code 
    // greatly simplified. Perhaps version DMDTk 5.1...
    if ( eai_OK == shape->getInternal ( v.getReference(), vertexCount, 
                                        n.getReference(), normalCount, 
                                        c.getReference(), colorCount, 
                                        t.getReference(), textureCount, 
                                        i ) )
    {
      // Should be true.
      SL_ASSERT ( vertexCount > 0 );
      SL_ASSERT ( NULL != v.getReference() );

      // If we have vertices...
      if ( vertexCount > 0 && NULL != v.getReference() )
      {
        // Save the starting places. Have to do this first.
        startOfVertices.push_back  ( vertices.size() );
        startOfNormals.push_back   ( normals.size() );
        startOfColors.push_back    ( colors.size() );
        startOfTexCoords.push_back ( texCoords.size() );

        // Append the arrays to the std::vectors. If they are empty then the 
        // append function will just return.
        CadKit::append3D ( vertexCount,  v.getReference(), vertices );
        CadKit::append3D ( normalCount,  n.getReference(), normals );
        CadKit::append3D ( colorCount,   c.getReference(), colors );
        CadKit::append2D ( textureCount, t.getReference(), texCoords );

        // Save the number of elements found (which may be zero).
        numVertices.push_back  ( vertexCount );
        numNormals.push_back   ( normalCount );
        numColors.push_back    ( colorCount );
        numTexCoords.push_back ( textureCount );

        // This iteration worked.
        gotVertices = true;
      }

      PROGRESS_LEVEL ( 5 ) ( FORMAT ( "\t\t\t\t%4d:   Vertices: %4d, Normals: %4d, Colors: %4d, Texture Coordinates: %4d", i + 1, vertexCount, normalCount, colorCount, textureCount ) );
    }

    // If we didn't get any vertices...
    if ( false == gotVertices )
    {
      SL_ASSERT ( 0 ); // Why did this happen?
      if ( false == ERROR ( FORMAT ( "Failed to get internal data for shape %X, set %s.", shape, i ), CadKit::FAILED ) )
        return false;
    }
  }

  // Should be true, but isn't detrimental.
  SL_ASSERT ( (unsigned int) numSets == numVertices.size() );

  // If don't have any vertices then we failed.
  if ( vertices.empty() )
    return false;

  // Truncate the numbers close to zero.
  std::for_each ( vertices.begin(),  vertices.end(),  _truncate );
  std::for_each ( normals.begin(),   normals.end(),   _truncate );
  std::for_each ( colors.begin(),    colors.end(),    _truncate );
  std::for_each ( texCoords.begin(), texCoords.end(), _truncate );

  // Calculate the bindings.
  if ( false == _shapeData->calculateBindings() )
    return false;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primtive data if needed. The primitive data is mutable, which is why 
//  this functions can be const. It is called from other "get" functions 
//  which are also const.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_setPrimData ( PrimHandle ph )
{
  SL_PRINT3 ( "In DbJtDatabase::_setPrimData(), this = %X, prim = %X\n", this, ph );

  // If the given shape is the one saved with the data then we're already set.
  if ( ph == _primData->getToken() )
    return true;

  // Initialize the primitive data.
  _primData->init ( NULL );

  // Get the primitive.
  SlRefPtr<eaiPrim> prim = this->_getPrim ( ph );
  if ( prim.isNull() )
  {
    SL_ASSERT ( 0 ); // Why did this happen?
    return false;
  }

  // Call the other one.
  if ( false == this->_setPrimData ( prim ) )
    return false;

  // Set the new token.
  _primData->setToken ( ph );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primtive data if needed. The primtive data is mutable, which is why 
//  this functions can be const. It is called from other "get" functions 
//  which are also const.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_setPrimData ( eaiPrim *prim )
{
  SL_PRINT3 ( "In DbJtDatabase::_setPrimData(), this = %X, prim = %X\n", this, prim );
  SL_ASSERT ( NULL != _current.get() );
  SL_ASSERT ( NULL != _current->getPart() );

  // Handle trivial case.
  if ( NULL == prim )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return false;
  }

  // Initialize the shape data.
  _primData->init ( NULL );

  // For convenience.
  std::vector<SlVec3f> &origins   = _primData->getOrigins().getData();
  std::vector<SlVec3f> &colors    = _primData->getColors().getData();
  std::vector<float>   &params    = _primData->getParameters();

  std::vector<unsigned int> &numOrigins  = _primData->getOrigins().getSizes();
  std::vector<unsigned int> &numColors   = _primData->getColors().getSizes();

  std::vector<unsigned int> &startOfOrigins  = _primData->getOrigins().getStarts();
  std::vector<unsigned int> &startOfColors   = _primData->getColors().getStarts();

  // Used in the loop.
  int originCount ( -1 ), paramCount ( -1 ), colorCount ( -1 );
  bool gotOrigins ( false );

  // Get the number of sets.
  int numSets ( prim->numOfSets() );

  PROGRESS_LEVEL ( 4 ) ( FORMAT ( "\t\t\tSets: %d", numSets ) );

  // Loop through all the sets.
  for ( int i = 0; i < numSets; ++i )
  {
    // Initialize.
    originCount = paramCount = colorCount = 0;
    gotOrigins = false;

    // Keep this in the loop. The destructor will delete the internal array.
    DbJtVisApiArray<float> o, p, c;

    // Get the arrays for this set of the shape. This function is the reason 
    // we jump through all these hoops (storing shape data and associating it 
    // with the handle). If we could ask for just the vertices, or just the 
    // normals (etc.) then this methodology could be eliminated and the code 
    // greatly simplified. Perhaps version DMDTk 5.1...
    if ( eai_OK == prim->getInternal ( o.getReference(), originCount, 
                                       p.getReference(), paramCount, 
                                       c.getReference(), colorCount, 
                                       i ) )
    {
      // Should be true.
      SL_ASSERT ( originCount > 0 );
      SL_ASSERT ( NULL != o.getReference() );

      // If we have origins...
      if ( originCount > 0 && NULL != o.getReference() )
      {
        originCount /= 3;
        colorCount /= 3;
        
        // Save the starting places. Have to do this first.
        startOfOrigins.push_back  ( origins.size() );
        startOfColors.push_back   ( colors.size() );

        // Append the arrays to the std::vectors. If they are empty then the 
        // append function will just return.  Adjust counts for differences in prim
        // internal data versus shape.
        CadKit::append3D ( originCount,  o.getReference(), origins );
        CadKit::append3D ( colorCount,   c.getReference(), colors );
        CadKit::append   ( paramCount,   p.getReference(), params );

        // Save the number of elements found (which may be zero).
        numOrigins.push_back  ( originCount );
        numColors.push_back   ( colorCount );

        // This iteration worked.
        gotOrigins = true;
      }

      PROGRESS_LEVEL ( 5 ) ( FORMAT ( "\t\t\t\t%4d:   Origins: %4d, Parameters: %4d, Colors: %4d", i + 1, originCount, paramCount, colorCount ) );
    }

    // If we didn't get any vertices...
    if ( false == gotOrigins )
    {
      SL_ASSERT ( 0 ); // Why did this happen?
      if ( false == ERROR ( FORMAT ( "Failed to get internal data for prim %X, set %s.", prim, i ), CadKit::FAILED ) )
        return false;
    }
  }

  // Should be true, but isn't detrimental.
  SL_ASSERT ( (unsigned int) numSets == numOrigins.size() );

  // If don't have any vertices then we failed.
  if ( origins.empty() )
    return false;

  // Truncate the numbers close to zero.
  std::for_each ( origins.begin(),  origins.end(),  _truncate );
  std::for_each ( params.begin(),   params.end(),   _truncate );
  std::for_each ( colors.begin(),    colors.end(),  _truncate );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertices.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getVertices ( ShapeHandle shape, IQueryShapeVerticesVec3f::VertexSetter &setter ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getVertices(), this = %X, shape = %X\n", this, shape );

  // Make sure the data is set.
  if ( false == ((DbJtDatabase *) this)->_setShapeData ( shape ) )
    return false;

  // For convenience.
  std::vector<SlVec3f> &vertices         = _shapeData->getVertices().getData();
  std::vector<unsigned int> &numVertices = _shapeData->getVertices().getSizes();

  // Get the number of vertices.
  unsigned int totalNumVertices = vertices.size();
  if ( 0 == totalNumVertices )
    return false;

  // Tell the setter the size.
  if ( false == setter.setSize ( totalNumVertices ) )
    return false;

  // Loop through and call the setter's function.
  unsigned int i;
  for ( i = 0; i < totalNumVertices; ++i )
    if ( false == setter.setData ( i, vertices[i] ) )
      return false;

  // Get the number of primitives.
  unsigned int numPrimitives = numVertices.size();
  SL_ASSERT ( numPrimitives > 0 );

  // Tell the setter the number of primitives.
  if ( false == setter.setNumPrimitives ( numPrimitives ) )
    return false;

  // Loop through and call the setter's function.
  unsigned int start ( 0 );
  for ( i = 0; i < numPrimitives; ++i )
  {
    if ( false == setter.setPrimitiveRange ( i, start, numVertices[i] ) )
      return false;
    start += numVertices[i];
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the normals.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getNormals ( ShapeHandle shape, IQueryShapeNormalsVec3f::NormalSetter &setter ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getNormals(), this = %X, shape = %X\n", this, shape );

  // Make sure the data is set.
  if ( false == ((DbJtDatabase *) this)->_setShapeData ( shape ) )
    return false;

  // For convenience.
  std::vector<SlVec3f> &normals = _shapeData->getNormals().getData();

  // Get the number of normals.
  unsigned int totalNumNormals = normals.size();
  if ( 0 == totalNumNormals )
    return false;

  // Tell the setter the size.
  if ( false == setter.setSize ( totalNumNormals ) )
    return false;

  // Loop through and call the setter's function.
  unsigned int i;
  for ( i = 0; i < totalNumNormals; ++i )
    if ( false == setter.setData ( i, normals[i] ) )
      return false;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the colors.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getColors ( ShapeHandle shape, IQueryShapeColorsVec3f::ColorSetter &setter ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getColors(), this = %X, shape = %X\n", this, shape );

  // Make sure the data is set.
  if ( false == ((DbJtDatabase *) this)->_setShapeData ( shape ) )
    return false;

  // For convenience.
  std::vector<SlVec3f> &colors = _shapeData->getColors().getData();

  // Get the number of colors.
  unsigned int totalNumColors = colors.size();
  if ( 0 == totalNumColors )
    return false;

  // Tell the setter the size.
  if ( false == setter.setSize ( totalNumColors ) )
    return false;

  // Loop through and call the setter's function.
  unsigned int i;
  for ( i = 0; i < totalNumColors; ++i )
    if ( false == setter.setData ( i, colors[i] ) )
      return false;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the colors.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getColors ( ShapeHandle shape, IQueryShapeColorsVec4f::ColorSetter &setter ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getColors(), this = %X, shape = %X\n", this, shape );

  // Make sure the data is set.
  if ( false == ((DbJtDatabase *) this)->_setShapeData ( shape ) )
    return false;

  // For convenience.
  std::vector<SlVec3f> &colors = _shapeData->getColors().getData();

  // Get the number of colors.
  unsigned int totalNumColors = colors.size();
  if ( 0 == totalNumColors )
    return false;

  // Tell the setter the size.
  if ( false == setter.setSize ( totalNumColors ) )
    return false;

  // Loop through and call the setter's function.
  unsigned int i;
  for ( i = 0; i < totalNumColors; ++i )
    if ( false == setter.setData ( i, SlVec4f ( colors[i][0], colors[i][1], colors[i][2], 1.0f ) ) )
      return false;

  // It worked.
  return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the colors for a primitive.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getColors ( PrimHandle prim, IQueryPrimColorsVec3f::ColorSetter &setter ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getColors(), this = %X, prim = %X\n", this, prim );

  // Make sure the data is set.
  if ( false == ((DbJtDatabase *) this)->_setPrimData ( prim ) )
    return false;

  // For convenience.
  std::vector<SlVec3f> &colors = _primData->getColors().getData();

  // Get the number of colors.
  unsigned int totalNumColors = colors.size();
  if ( 0 == totalNumColors )
    return false;

  // Tell the setter the size.
  if ( false == setter.setSize ( totalNumColors ) )
    return false;

  // Loop through and call the setter's function.
  unsigned int i;
  for ( i = 0; i < totalNumColors; ++i )
    if ( false == setter.setData ( i, colors[i] ) )
      return false;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the colors for a primitive.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getColors ( PrimHandle prim, IQueryPrimColorsVec4f::ColorSetter &setter ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getColors(), this = %X, prim = %X\n", this, prim );

  // Make sure the data is set.
  if ( false == ((DbJtDatabase *) this)->_setPrimData ( prim ) )
    return false;

  // For convenience.
  std::vector<SlVec3f> &colors = _primData->getColors().getData();

  // Get the number of colors.
  unsigned int totalNumColors = colors.size();
  if ( 0 == totalNumColors )
    return false;

  // Tell the setter the size.
  if ( false == setter.setSize ( totalNumColors ) )
    return false;

  // Loop through and call the setter's function.
  unsigned int i;
  for ( i = 0; i < totalNumColors; ++i )
    if ( false == setter.setData ( i, SlVec4f ( colors[i][0], colors[i][1], colors[i][2], 1.0f ) ) )
      return false;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture coordinates.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getTextureCoords ( ShapeHandle shape, IQueryShapeTexCoordsVec2f::TexCoordSetter &setter ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getTextureCoords(), this = %X, shape = %X\n", this, shape );

  // Make sure the data is set.
  if ( false == ((DbJtDatabase *) this)->_setShapeData ( shape ) )
    return false;

  // For convenience.
  std::vector<SlVec2f> &texCoords = _shapeData->getTexCoords().getData();

  // Get the number of texture coordinates.
  unsigned int totalNumTexCoords = texCoords.size();
  if ( 0 == totalNumTexCoords )
    return false;

  // Should be per-vertex binding.
  SL_ASSERT ( totalNumTexCoords == _shapeData->getVertices().getData().size() );

  // Tell the setter the size.
  if ( false == setter.setSize ( totalNumTexCoords ) )
    return false;

  // Loop through and call the setter's function.
  unsigned int i;
  for ( i = 0; i < totalNumTexCoords; ++i )
    if ( false == setter.setData ( i, texCoords[i] ) )
      return false;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the set type.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getVertexSetType ( ShapeHandle sh, VertexSetType &type ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getVertexSetType(), this = %X, shape = %X\n", this, sh );

  // Get the shape.
  SlRefPtr<eaiShape> shape = this->_getShape ( sh );
  if ( shape.isNull() )
  {
    SL_ASSERT ( 0 ); // Why did this happen?
    return false;
  }

  // Determine the type of shape.
  switch ( shape->typeID() )
  {
  case eaiEntity::eaiLINESTRIPSET:

    type = CadKit::LINE_STRIP_SET;
    break;

  case eaiEntity::eaiPOINTSET:

    type = CadKit::POINT_SET;
    break;

  case eaiEntity::eaiPOLYGONSET:

    type = CadKit::POLYGON_SET;
    break;

  case eaiEntity::eaiTRISTRIPSET:

    type = CadKit::TRI_STRIP_SET;
    break;

  default:

    type = CadKit::UNKNOWN;
    SL_ASSERT ( 0 ); // Heads up.
    return false;
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the normal binding.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getNormalBinding ( ShapeHandle shape, VertexBinding &binding ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getNormalBinding(), this = %X, shape = %X\n", this, shape );

  // Make sure the data is set.
  if ( false == ((DbJtDatabase *) this)->_setShapeData ( shape ) )
    return false;

  // Set the binding.
  binding = _shapeData->getNormalBinding();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color binding.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getColorBinding ( ShapeHandle shape, VertexBinding &binding ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getColorBinding(), this = %X, shape = %X\n", this, shape );

  // Make sure the data is set.
  if ( false == ((DbJtDatabase *) this)->_setShapeData ( shape ) )
    return false;

  // Set the binding.
  binding = _shapeData->getColorBinding();

  // It worked.
  return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the color binding for a primitive.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getColorBinding ( PrimHandle prim, VertexBinding &binding ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getColorBinding(), this = %X, prim = %X\n", this, prim );

  // Make sure the data is set.
  if ( false == ((DbJtDatabase *) this)->_setPrimData ( prim ) )
    return false;

  // Set the binding, one color per primitive.
  binding = BINDING_OVERALL;

  // It worked.
  return true;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the message priority level.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::setMessagePriorityLevel ( const MessageType &type, unsigned int priority )
{
  SL_PRINT4 ( "In DbJtDatabase::setMessagePriorityLevel(), this = %X, type = %d, priority = %d\n", this, type, priority );

  // We only handle progress priority levels.
  if ( MESSAGE_PROGRESS != type )
    return false;

  // Set the level.
  _progressPriorityLevel = priority;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set option for processing the LODs.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::setLodProcessOption ( const LodProcessOption &option )
{
  SL_PRINT3 ( "In DbJtDatabase::setLodProcessOption(), this = %X, option = %d\n", this, option );

  // Set the option.
  _lodOption = option;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Replace the part's properties with the instance's properties.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::_replaceProperties ( eaiInstance *instance, eaiPart *part )
{
  SL_PRINT4 ( "In DbJtDatabase::_replaceProperties(), this = %X, instance = %X, part = %X\n", this, instance, part );

  // Replace the name. Note: a missing name will get replaced with a blank 
  // name, but that should have the same behavior as the way we handle 
  // missing names now (i.e., we return a blank string).
  std::string name = CadKit::getName ( instance );
  SL_VERIFY ( eai_OK == part->setName ( name.c_str() ) );

  // Replace the material.
  SlRefPtr<eaiMaterial> material = CadKit::getMaterial ( instance );
  if ( material.isValid() )
    SL_VERIFY ( eai_OK == part->setMaterial ( material ) );

  // Replace the transform.
  SlRefPtr<eaiTransform> transform = CadKit::getTransform ( instance );
  if ( transform.isValid() )
    SL_VERIFY ( eai_OK == part->setTransform ( transform ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive type.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getPrimType ( PrimHandle ph, PrimitiveType &type ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getPrimitiveType(), this = %X, prim = %X\n", this, ph );

  // Get the shape.
  SlRefPtr<eaiPrim> prim = this->_getPrim ( ph );
  if ( prim.isNull() )
  {
    SL_ASSERT ( 0 ); // Why did this happen?
    return false;
  }

  // Determine the type of shape.
  switch ( prim->typeID() )
  {
  case eaiEntity::eaiBOXSET:
    type = CadKit::BOX;
    break;
  case eaiEntity::eaiCYLINDERSET:
    type = CadKit::CYLINDER;
    break;
  case eaiEntity::eaiPYRAMIDSET:
    type = CadKit::PYRAMID;
    break;
  case eaiEntity::eaiSPHERESET:
    type = CadKit::SPHERE;
    break;
  case eaiEntity::eaiTRIPRISMSET:
    type = CadKit::TRIPRISM;
    break;
  default:
    type = CadKit::UNKNOWN_PRIM;
    SL_ASSERT ( 0 ); // Heads up.
    return false;
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive origin(s).
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getPrimOrigins ( PrimHandle prim, IQueryPrimOriginsVec3f::OriginSetter &setter ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getPrimOrigins(), this = %X, prim = %X\n", this, prim );

  // Make sure the data is set.
  if ( false == ((DbJtDatabase *) this)->_setPrimData ( prim ) )
    return false;

  // For convenience.
  std::vector<SlVec3f> &origins          = _primData->getOrigins().getData();
  std::vector<unsigned int> &numOrigins  = _primData->getOrigins().getSizes();

  // Get the number of vertices.
  unsigned int totalNumOrigins = origins.size();
  if ( 0 == totalNumOrigins )
    return false;

  // Tell the setter the size.
  if ( false == setter.setSize ( totalNumOrigins ) )
    return false;

  // Loop through and call the setter's function.
  unsigned int i;
  for ( i = 0; i < totalNumOrigins; ++i )
    if ( false == setter.setData ( i, origins[i] ) )
      return false;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive parameters.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::getPrimParams ( PrimHandle prim, IQueryPrimParamsFloat::ParamSetter &setter ) const
{
  SL_PRINT3 ( "In DbJtDatabase::getPrimParams(), this = %X, prim = %X\n", this, prim );

  // Make sure the data is set.
  if ( false == ((DbJtDatabase *) this)->_setPrimData ( prim ) )
    return false;

  // For convenience.
  std::vector<float> &params = _primData->getParameters();

  // Get the number of vertices.
  unsigned int totalNumParams = params.size();
  if ( 0 == totalNumParams )
    return false;

  // Tell the setter the size.
  if ( false == setter.setSize ( totalNumParams ) )
    return false;

  // Loop through and call the setter's function.
  unsigned int i;
  for ( i = 0; i < totalNumParams; ++i )
    if ( false == setter.setData ( i, params[i] ) )
      return false;

  // It worked.
  return true;
}


