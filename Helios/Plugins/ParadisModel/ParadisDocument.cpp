
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for Paradis models.
//
///////////////////////////////////////////////////////////////////////////////

#include "ParadisDocument.h"
#include "ParadisReader.h"

#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Interfaces/GUI/ICancelButton.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/ISmoothTriangles.h"
#include "Usul/Interfaces/IDecimateTriangles.h"
#include "Usul/Interfaces/ISubdivideTriangles.h"

#include "Usul/Components/Manager.h"

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"
#include "Usul/Policies/Update.h"

#include "Usul/Loops/FindLoops.h"
#include "Usul/Types/Types.h"


#include "Usul/Adaptors/Random.h"

#include "Usul/Resources/ProgressBar.h"
#include "Usul/Resources/StatusBar.h"
#include "Usul/Resources/CancelButton.h"
#include "Usul/Errors/Assert.h"

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ParadisDocument, ParadisDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

ParadisDocument::ParadisDocument() : BaseClass ( "Paradis Document" ),
  _triangles ( new TriangleSet )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ParadisDocument::~ParadisDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ParadisDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IGetBoundingBox::IID:
    return static_cast < Usul::Interfaces::IGetBoundingBox* > ( this );
  case Usul::Interfaces::IGroupPrimitives::IID:
    return static_cast < Usul::Interfaces::IGroupPrimitives* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool ParadisDocument::canExport ( const std::string &file ) const
{
  return this->canSave ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool ParadisDocument::canInsert ( const std::string &file ) const
{
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool ParadisDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "prds" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool ParadisDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  //return ( ext == "prds" );
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing data.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );

  if ( "prds" == ext )
  {
    ParadisReader reader ( name, caller, this );
    reader();
  }
  else
  {
    throw std::runtime_error("Error: 983249834290 Invalid file extension for Paradis Document:" + name);
  }

  //this->purge();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to the given file name.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
  // Clear the triangle set.
  //_triangles->clear ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

ParadisDocument::Filters ParadisDocument::filtersInsert() const
{
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

ParadisDocument::Filters ParadisDocument::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "Paradis (*.prds)", "*.prds" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

ParadisDocument::Filters ParadisDocument::filtersSave() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

ParadisDocument::Filters ParadisDocument::filtersExport() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ParadisDocument::buildScene ( const BaseClass::Options &opt, Unknown *caller )
{
  // Redirect to triangle set
  return _triangles->buildScene ( opt, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the default GUI
//
///////////////////////////////////////////////////////////////////////////////

void ParadisDocument::createDefaultGUI ( Unknown *caller )
{
  if ( this->delegate() )
  {
    BaseClass::Options &options ( this->options() );
    options["normals"] = "per-vertex";
    options["colors"]  = "per-vertex";
    this->delegate()->createDefaultGUI ( this, caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get The BoundingBox for this document
//
///////////////////////////////////////////////////////////////////////////////

osg::BoundingBox ParadisDocument::getBoundingBox() const 
{
  return _triangles->boundingBox();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the triangle set. If the current one is empty, it just replaces it.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisDocument::addTriangleSet ( TriangleSet *t ) 
{
  if ( _triangles->empty() )
  {
    _triangles = t;
    return;
  }

  USUL_ASSERT ( 0 ); // TODO, insert...
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of groups.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int ParadisDocument::groupsNumber () const
{
  return _triangles->numberOfGroups();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the group at the given index.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ParadisDocument::getPrimitiveGroup ( unsigned int i )
{
  return _triangles->getPrimitiveGroup ( i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Smooth the model.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisDocument::smoothModel ( )
{
  Usul::Interfaces::IUnknown *unknown ( Usul::Components::Manager::instance().getInterface ( Usul::Interfaces::ISmoothTriangles::IID ) );
  Usul::Interfaces::ISmoothTriangles::QueryPtr plugin ( unknown );

  if ( plugin.valid() )
  {
    _triangles->smooth ( plugin.get(), 20 );
    this->modified( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decimate the model.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisDocument::decimateModel ( )
{
  Usul::Interfaces::IUnknown *unknown ( Usul::Components::Manager::instance().getInterface ( Usul::Interfaces::IDecimateTriangles::IID ) );
  Usul::Interfaces::IDecimateTriangles::QueryPtr plugin ( unknown );

  if ( plugin.valid() )
  {
    _triangles->decimate( plugin.get(), 0.50 );
    this->modified( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subdivide the model.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisDocument::subdivideModel ( unsigned int numSubdivisions )
{
  Usul::Interfaces::IUnknown *unknown ( Usul::Components::Manager::instance().getInterface ( Usul::Interfaces::ISubdivideTriangles::IID ) );
  Usul::Interfaces::ISubdivideTriangles::QueryPtr plugin ( unknown );

  if ( plugin.valid() )
  {
    _triangles->subdivide ( plugin.get(), numSubdivisions );
    this->modified( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Document already grouped.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisDocument::groupPrimitives ( Usul::Interfaces::IUnknown *caller )
{
}
