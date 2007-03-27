
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "PhaseFieldDocument.h"
#include "SceneElement.h"

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"

#include "OsgTools/ReadModel.h"

#include "osg/Group"
#include "osg/MatrixTransform"

#include "osgDB/WriteFile"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PhaseFieldDocument, PhaseFieldDocument::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

PhaseFieldDocument::PhaseFieldDocument() : BaseClass ( "Phase Field Document" ), 
  _scene ( 0x0 ),
  _current ( 0 ),
  _filenames (),
  _assemblies()
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
  case Usul::Interfaces::ITimeVaryingData::IID:
    return static_cast < Usul::Interfaces::ITimeVaryingData * > ( this );
  case Usul::Interfaces::IAssemblyManager::IID:
    return static_cast < Usul::Interfaces::IAssemblyManager * > ( this );
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
  return this->canOpen ( file );
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
  _filenames.push_back ( name );
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
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *PhaseFieldDocument::buildScene ( const BaseClass::Options &opt, Unknown *caller )
{
  if ( _filenames.empty() )
    return 0x0;

  osg::ref_ptr< osg::Node > node ( OsgTools::readModel ( _filenames.at( _current ) ) );

  _scene = node;

  _assemblies.clear();

  // Should be a group if it's truely a phase field document.
  if ( osg::Group *root = dynamic_cast < osg::Group * > ( _scene.get() ) )
  {
    for ( unsigned int i = 0; i < root->getNumChildren(); ++i )
    {
      Assembly::RefPtr assembly ( new Assembly );

      osg::Node *n ( root->getChild ( i ) );

      if ( osg::Group * group = dynamic_cast < osg::Group * > ( n ) )
      {
        for ( unsigned int j = 0; j < group->getNumChildren(); ++j )
        {
          osg::ref_ptr < osg::Node > child ( group->getChild ( j ) );
          
          if ( osg::MatrixTransform *mt = dynamic_cast < osg::MatrixTransform * > ( child.get() ) )
          {
            SceneElement::RefPtr element ( new SceneElement );
            element->transform ( mt );
            assembly->appendSceneElement ( element.get() );
          }
          else
          {
            group->removeChild ( child.get() );

            osg::ref_ptr < osg::MatrixTransform > transform ( new osg::MatrixTransform );
            transform->addChild ( child.get() );

            group->addChild ( transform.get() );

            SceneElement::RefPtr element ( new SceneElement );
            element->transform ( transform.get() );
            assembly->appendSceneElement ( element.get() );
          }
        }
      }

      _assemblies.insert ( Assemblies::value_type ( n->getName(), assembly.get() ) );
    }
  }

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


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current time step.
//
///////////////////////////////////////////////////////////////////////////////

void PhaseFieldDocument::setCurrentTimeStep ( unsigned int current )
{
  _current = current;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current time step.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int PhaseFieldDocument::getCurrentTimeStep () const
{
  return _current;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of timesteps.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int PhaseFieldDocument::getNumberOfTimeSteps () const
{
  return _filenames.size();
}


void PhaseFieldDocument::getAssemblyNames ( std::vector< std::string >& names )
{
  for ( Assemblies::const_iterator iter = _assemblies.begin(); iter != _assemblies.end(); ++iter )
  {
    names.push_back ( iter->first );
  }
}

Usul::Interfaces::IAssembly* PhaseFieldDocument::getAssemblyByName ( const std::string& name )
{
  return _assemblies[ name ].get();
}

void PhaseFieldDocument::createAssembly ( const std::string& name )
{
  _assemblies.insert ( Assemblies::value_type ( name, new Assembly ) );
}
