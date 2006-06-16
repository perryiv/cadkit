
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "PhaseFieldDocument.h"

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"

#include "OsgTools/ReadModel.h"

#include "osgDB/WriteFile"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PhaseFieldDocument, PhaseFieldDocument::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

PhaseFieldDocument::PhaseFieldDocument() : BaseClass ( "Phase Field Document" ), 
  _scene ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

PhaseFieldDocument::~PhaseFieldDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *PhaseFieldDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IGetBoundingBox::IID:
    return static_cast < Usul::Interfaces::IGetBoundingBox* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool PhaseFieldDocument::canExport ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "ive" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool PhaseFieldDocument::canInsert ( const std::string &file ) const
{
  //return this->canOpen ( file );
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool PhaseFieldDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "ive" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool PhaseFieldDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "ive" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void PhaseFieldDocument::write ( const std::string &name, Unknown *caller  ) const
{
  //Write the file using osg
  osgDB::writeNodeFile( *_scene, name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void PhaseFieldDocument::read ( const std::string &name, Unknown *caller )
{
  osg::ref_ptr< osg::Node > node ( OsgTools::readModel ( name ) );

  _scene = node;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void PhaseFieldDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
  _scene = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

PhaseFieldDocument::Filters PhaseFieldDocument::filtersSave() const
{
  Filters filters;
  filters.push_back ( Filter ( "OpenSceneGraph Binary (*.ive)", "*.ive" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

PhaseFieldDocument::Filters PhaseFieldDocument::filtersExport() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

PhaseFieldDocument::Filters PhaseFieldDocument::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "OpenSceneGraph Binary (*.ive)", "*.ive"       ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

PhaseFieldDocument::Filters PhaseFieldDocument::filtersInsert() const
{
  //return this->filtersOpen();
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *PhaseFieldDocument::buildScene ( const BaseClass::Options &opt, Unknown *caller )
{
  // Return the scene
  return _scene.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

osg::BoundingBox PhaseFieldDocument::getBoundingBox() const
{
  // Return hard coded values for now.
  osg::BoundingBox bb;
  bb.expandBy( 0.0, 0.0, 0.0 );
  bb.expandBy ( 255.0, 255.0, 255.0 );
  return bb;
}
