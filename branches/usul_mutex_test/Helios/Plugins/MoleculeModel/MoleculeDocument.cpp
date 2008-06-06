
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for Molecule models.
//
///////////////////////////////////////////////////////////////////////////////

#include "MoleculeDocument.h"
#include "MoleculeComponent.h"

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"
#include "Usul/Policies/Update.h"
#include "Usul/Interfaces/GUI/IProgressBar.h" 
#include "Usul/File/Stats.h"
#include "Usul/File/Temp.h"

#include "osgPlugins/pdb/ReaderWriterPDB.h"

#include <fstream>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( MoleculeDocument, MoleculeDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

MoleculeDocument::MoleculeDocument() : BaseClass ( "Molecule Document" ),
_molecules()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

MoleculeDocument::~MoleculeDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *MoleculeDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  default:
    return BaseClass::queryInterface( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool MoleculeDocument::canExport ( const std::string &file ) const
{
  return this->canSave ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool MoleculeDocument::canInsert ( const std::string &file ) const
{
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool MoleculeDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "pdb" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool MoleculeDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "pdb" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing data.
//
///////////////////////////////////////////////////////////////////////////////

void MoleculeDocument::read ( const std::string &name, Unknown *caller )
{
  // The reader
  osg::ref_ptr<ReaderWriterPDB> reader ( new ReaderWriterPDB );

  reader->init();

  // Open the file.
  std::ifstream in ( name.c_str() );

  // Make sure it opened.
  if ( !in.is_open() )
    throw std::runtime_error ( "Error in reading file." );

  // Update functor
  Usul::Interfaces::IProgressBar::UpdateProgressBar update( 0.0, 100.0, caller );

  // Parse the pdb file
  reader->parse ( in, Usul::File::size( name ), update );

  //Set the molecule list
  Molecules m ( reader->getMolecules() );
  
  for( Molecules::const_iterator i = m.begin(); i != m.end(); ++i )
    _molecules.push_back( *i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void MoleculeDocument::write ( const std::string &name, Unknown *caller  ) const
{
  //Open the file for writing
  Usul::File::Temp temp;

  //loop through the molecules
  for (Molecules::const_iterator i = _molecules.begin(); i != _molecules.end(); ++i)
  {
    const MoleculePtr &molecule = *i;
    const osgPlugins::pdb::Molecule::Atoms &atoms ( molecule->atoms() );

    //Write out all the atoms
    for( unsigned int i = 0; i < atoms.size(); ++i )
      temp.stream() << *atoms.at(i) << std::endl;
  }

  temp.rename( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void MoleculeDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
  _molecules.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

MoleculeDocument::Filters MoleculeDocument::filtersExport() const
{
  Filters filters;
  filters.push_back ( Filter ( "Protien Data Bank (*.pdb)", "*.pdb" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

MoleculeDocument::Filters MoleculeDocument::filtersInsert() const
{
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

MoleculeDocument::Filters MoleculeDocument::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "Protien Data Bank (*.pdb)", "*.pdb" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

MoleculeDocument::Filters MoleculeDocument::filtersSave() const
{
  Filters filters;
  filters.push_back ( Filter ( "Protien Data Bank (*.pdb)", "*.pdb" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *MoleculeDocument::buildScene ( const BaseClass::Options &opt, Unknown *caller )
{
  osg::ref_ptr<osg::Group> root ( new osg::Group );

  // Loop through the molecules
  for (Molecules::const_iterator i = _molecules.begin(); i != _molecules.end(); ++i)
  {
    const MoleculePtr &molecule = *i;
    osg::ref_ptr<osg::MatrixTransform> mt ((osg::MatrixTransform *) (molecule.get())->build() );
    root->addChild ( mt.get() );
  }

  // Return the root.
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify this document of the message.
//
///////////////////////////////////////////////////////////////////////////////

void MoleculeDocument::notify ( unsigned short message )
{
  BaseClass::notify ( message );
}
