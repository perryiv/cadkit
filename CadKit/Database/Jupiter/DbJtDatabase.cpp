
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbJtDatabase: Jupiter database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbJtPrecompiled.h"
#include "DbJtDatabase.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlStringFunctions.h"

#include "Interfaces/IErrorNotify.h"
#include "Interfaces/IWarningNotify.h"
#include "Interfaces/IProgressNotify.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "DbJtVisApiHeaders.h"
#endif

// This is the only way to get a pointer to the traverser from inside the 
// callback function.
namespace CadKit { DbJtDatabase *_traverser = NULL; }

// To help shorted up the lines.
#undef  ERROR
#define ERROR    this->_notifyError
#define PROGRESS this->_notifyProgress
#define WARNING  this->_notifyWarning
#define FORMAT   CadKit::getString

using namespace CadKit;

// These live in DbJtTraverser for now...
namespace CadKit
{
void _incrementPointerReferenceCount ( eaiEntity *p );
void _decrementPointerReferenceCount ( eaiEntity *p );
};


SL_IMPLEMENT_DYNAMIC_CLASS ( DbJtDatabase, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtDatabase::DbJtDatabase ( const unsigned int &customerId ) : SlRefBase ( 0 ),
  _flags ( 0 ),
  _clients ( new Clients ),
  _target ( NULL ),
  _controller ( NULL ),
  _customerId ( customerId ),
  _initialized ( false ),
  _assemblyLoadOption ( INSTANCE_ASSEMBLY ),
  _brepLoadOption ( TESS_ONLY ),
  _shapeLoadOption ( ALL_LODS )
{
  SL_PRINT2 ( "In DbJtDatabase::DbJtDatabase(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DbJtDatabase::~DbJtDatabase()
{
  SL_PRINT2 ( "In DbJtDatabase::~DbJtDatabase(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query interface.
//
///////////////////////////////////////////////////////////////////////////////

IUnknown *DbJtDatabase::queryInterface ( const unsigned long &iid )
{
  SL_PRINT2 ( "In DbJtDatabase::queryInterface(), this = %X\n", this );

  switch ( iid )
  {
  case IDataSource::IID:
    return static_cast<IDataSource *>(this);
  case IController::IID:
    return static_cast<IController *>(this);
  default:
    return NULL;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the reference count.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long DbJtDatabase::getRefCount() const
{
  SL_PRINT2 ( "In DbJtDatabase::getRefCount(), this = %X\n", this );
  return SlRefBase::getRefCount();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::ref()
{
  SL_PRINT2 ( "In DbJtDatabase::ref(), this = %X\n", this );
  this->_incrementReferenceCount();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::unref()
{
  SL_PRINT2 ( "In DbJtDatabase::unref(), this = %X\n", this );
  this->_decrementReferenceCount();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_init()
{
  SL_PRINT2 ( "In DbJtDatabase::_init(), this = %X\n", this );

  // Just return if we are already initialized.
  if ( true == _initialized )
    return true;

#if ( _DMDTK_VERSION > 4 )

  if ( false == PROGRESS ( "Initializing DirectModel Data Toolkit." ) )
    return false;

  // Initialize DMDTk.
  if ( eai_ERROR == eaiEntityFactory::init() )
  {
    ERROR ( FORMAT ( "Failed to initialize the DirectModel Data Toolkit." ) );
    return false;
  }

  if ( false == PROGRESS ( "Done initializing DirectModel Data Toolkit." ) )
    return false;

#endif // _DMDTK_VERSION

  // Get the customer number.
  const unsigned int &customer = this->_getCustomerId();

  if ( false == PROGRESS ( FORMAT ( "Attempting to register customer number %d... ", customer ) ) )
    return false;

  // Register the customer.
  if ( eai_ERROR == eaiEntityFactory::registerCustomer ( customer ) )
  {
    ERROR ( FORMAT ( "Failed to register customer number: %d.", customer ), 0 );
    return false;
  }

  if ( false == PROGRESS ( "Done registering customer." ) )
    return false;

  // We are now initialized.
  _initialized = true;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::loadData ( const char *filename )
{
  SL_PRINT3 ( "In DbJtDatabase::setDataTarget(), this = %X, name = %s\n", this, ( filename ) ? filename : "" );
  SL_ASSERT ( filename );

  // Try to traverse.
  try
  {
    // See if we need to initialize.
    if ( false == this->_init() )
      return false;

    // Traverse the database.
    return this->_traverse ( filename );
  }

  // Catch any exceptions.
  catch ( ... )
  {
    ERROR ( FORMAT ( "Exception generated when traversing '%s'", filename ), 0 );
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse the database.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_traverse ( const char *filename )
{
  SL_PRINT3 ( "In DbJtDatabase::_traverse(), this = %X, filename = %s\n", this, filename );
  SL_ASSERT ( filename );

  // Initialize.
//  _currentNode = NULL;
//  _currentLevel = 0;

  if ( false == PROGRESS ( "Creating CAD importer." ) )
    return false;

  // Declare a CAD importer. The returned instance has a zero reference count.
  // Assigning it to the SlRefPtr will increment it to one. When the SlRefPtr 
  // goes out of scope the internal pointer will be deferenced (back to zero),
  // and the importer will get deleted.
  SlRefPtr<eaiCADImporter> importer = eaiEntityFactory::createCADImporter();
  if ( importer.isNull() )
  {
    ERROR ( FORMAT ( "Failed to create CAD importer. Is your license set up correctly?" ), 0 );
    return false;
  }

  if ( false == PROGRESS ( "Done creating CAD importer." ) )
    return false;

  // Set the options.
  importer->setShapeLoadOption ( (eaiCADImporter::ShapeLoadOption) _shapeLoadOption );
  importer->setBrepLoadOption ( (eaiCADImporter::BrepLoadOption) _brepLoadOption );
  importer->setAssemblyOption ( (eaiCADImporter::AssemblyOption) _assemblyLoadOption );

  if ( false == PROGRESS ( "Creating traverser." ) )
    return false;

  // Declare the traverser. Note: returned pointer has ref-count of one.
  SlRefPtr<eaiTraverser> traverser = eaiEntityFactory::createTraverser();
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

  if ( false == PROGRESS ( FORMAT ( "Done registering traversal callbacks.\nImporting database: %s", filename ) ) )
    return false;

  // Import the database. Note: returned pointer has ref-count of one.
  SlRefPtr<eaiHierarchy> root = importer->import ( filename );
  if ( root.isNull() )
  {
    ERROR ( FORMAT ( "Failed to import database: %s", filename ), 0 );
    return false;
  }

  if ( false == PROGRESS ( FORMAT ( "Done importing database: %s\nStarting the traversal.", filename ) ) )
    return false;

  // There doesn't appear to be a way to get a pointer to this instance inside 
  // the callback function. So I have to use this work-around.
  _traverser = this;

  // Traverse the database.
  if ( eai_OK != traverser->traverseGraph ( root ) )
  {
    ERROR ( "Failed to traverse database.", 0 );
    return false;
  }

  // Reset this.
  _traverser = NULL;

  if ( false == PROGRESS ( FORMAT ( "Done traversing: %s", filename ) ) )
    return false;

  // TODO: It throws an exception when I allow this to be unref'd (and deleted).
  importer.makeNull();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-action traversal callback.
//
///////////////////////////////////////////////////////////////////////////////

int DbJtDatabase::_preActionTraversalCallback ( eaiHierarchy *node, int level )
{
  SL_PRINT3 ( "In DbJtDatabase::_preActionTraversalCallback(), node = %X, level = %d\n", node, level );
  SL_ASSERT ( _traverser );

  // Call the other one.
  return eai_OK == _traverser->_preActionTraversalNotify ( node, level );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post-action traversal callback.
//
///////////////////////////////////////////////////////////////////////////////

int DbJtDatabase::_postActionTraversalCallback ( eaiHierarchy *node, int level )
{
  SL_PRINT3 ( "In DbJtDatabase::_postActionTraversalCallback(), node = %X, level = %d\n", node, level );
  SL_ASSERT ( _traverser );

  // Call the other one.
  return eai_OK == _traverser->_postActionTraversalNotify ( node, level );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-action traversal notify.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_preActionTraversalNotify ( eaiHierarchy *node, int level )
{
  SL_PRINT3 ( "In DbJtDatabase::_preActionTraversalNotify(), node = %X, level = %d\n", node, level );
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post-action traversal notify.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_postActionTraversalNotify ( eaiHierarchy *node, int level )
{
  SL_PRINT3 ( "In DbJtDatabase::_postActionTraversalNotify(), node = %X, level = %d\n", node, level );
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data target.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::setDataTarget ( IUnknown *target )
{
  SL_PRINT3 ( "In DbJtDatabase::setDataTarget(), this = %X, target = %X\n", this, target );

  // Set the target, it may be null.
  _target = target;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the controller.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::setController ( IUnknown *controller )
{
  SL_PRINT3 ( "In DbJtDatabase::setController(), this = %X, controller = %X\n", this, controller );

  // Set the target, it may be null.
  _controller = controller;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the customer number.
//
///////////////////////////////////////////////////////////////////////////////

const unsigned int &DbJtDatabase::_getCustomerId()
{
  SL_PRINT2 ( "In DbJtDatabase::_getCustomerId(), this = %X\n", this );

  // Was it set by the client?
  if ( 0 != _customerId )
    return _customerId;

  // Try to get it from the environment.
  const char *temp = ::getenv ( "DMDTK_CUSTOMER_ID" );
  if ( temp ) 
    _customerId = ::atoi ( temp );

  // Return what we have.
  return _customerId;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Error notification.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_notifyError ( const std::string &message, const unsigned long &id )
{
  SL_PRINT4 ( "In DbJtDatabase::_notifyError(), this = %X, id = %d, message = \n", this, id, message.c_str() );

  // See if the target supports the proper interface.
  SlRefPtr<IErrorNotify> target = static_cast<IErrorNotify *> ( _target->queryInterface ( IErrorNotify::IID ) );

  // If the interface is not implemented then return true to proceed.
  if ( target.isNull() )
    return true;

  // Let the target know.
  return target->errorNotify ( message, id );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Progress notification.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_notifyProgress ( const std::string &message )
{
  SL_PRINT3 ( "In DbJtDatabase::_notifyProgress(), this = %X, message = \n", this, message.c_str() );

  // See if the target supports the proper interface.
  SlRefPtr<IProgressNotify> target = static_cast<IProgressNotify *> ( _target->queryInterface ( IProgressNotify::IID ) );

  // If the interface is not implemented then return true to proceed.
  if ( target.isNull() )
    return true;

  // Let the target know.
  return target->progressNotify ( message );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Warning notification.
//
///////////////////////////////////////////////////////////////////////////////

bool DbJtDatabase::_notifyWarning ( const std::string &message, const unsigned long &id )
{
  SL_PRINT4 ( "In DbJtDatabase::_notifyWarning(), this = %X, id = %d, message = \n", this, id, message.c_str() );

  // See if the target supports the proper interface.
  SlRefPtr<IWarningNotify> target = static_cast<IWarningNotify *> ( _target->queryInterface ( IWarningNotify::IID ) );

  // If the interface is not implemented then return true to proceed.
  if ( target.isNull() )
    return true;

  // Let the target know.
  return target->warningNotify ( message, id );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the assembly load option.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::setAssemblyLoadOption ( const AssemblyLoadOption &option )
{
  SL_PRINT3 ( "In DbJtDatabase::setAssemblyLoadOption(), this = %X, option = %d\n", this, option );
  SL_ASSERT ( (unsigned int) eaiCADImporter::eaiINSTANCE_ASSEMBLY == (unsigned int) INSTANCE_ASSEMBLY );
  SL_ASSERT ( (unsigned int) eaiCADImporter::eaiEXPLODE_ASSEMBLY == (unsigned int) EXPLODE_ASSEMBLY );

  // Set the option.
  _assemblyLoadOption = option;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the brep load option.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::setBrepLoadOption ( const BrepLoadOption &option )
{
  SL_PRINT3 ( "In DbJtDatabase::setBrepLoadOption(), this = %X, option = %d\n", this, option );
  SL_ASSERT ( (unsigned int) eaiCADImporter::eaiTESS_ONLY == (unsigned int) TESS_ONLY );
  SL_ASSERT ( (unsigned int) eaiCADImporter::eaiBREP_ONLY == (unsigned int) BREP_ONLY );
  SL_ASSERT ( (unsigned int) eaiCADImporter::eaiTESS_AND_BREP == (unsigned int) ALL );

  // Set the option.
  _brepLoadOption = option;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the shape load option.
//
///////////////////////////////////////////////////////////////////////////////

void DbJtDatabase::setShapeLoadOption ( const ShapeLoadOption &option )
{
  SL_PRINT3 ( "In DbJtDatabase::setShapeLoadOption(), this = %X, option = %d\n", this, option );
  SL_ASSERT ( (unsigned int) eaiCADImporter::eaiALL_LODS == (unsigned int) ALL_LODS );
  SL_ASSERT ( (unsigned int) eaiCADImporter::eaiHIGH_LOD == (unsigned int) HIGH_LOD );

  // Set the option.
  _shapeLoadOption = option;
}
