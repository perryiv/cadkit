
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "EnergyComponent.h"

#include "Usul/Cast/Cast.h"
#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/IOpenSceneGraph.h"
#include "Usul/Interfaces/ILoadFileDialog.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/INotify.h"
#include "Usul/Interfaces/IGetMolecules.h"
#include "Usul/Interfaces/IPsfFile.h"
#include "Usul/Interfaces/IParamFile.h"

#include "osgPlugins/pdb/Atom.h"
#include "osgPlugins/pdb/Molecule.h"
#include "osgPlugins/pdb/ReaderWriterPDB.h"

#include "Mindy/Vector.h"
#include "Mindy/CalculateEnergy.h"

#include <sstream>

using namespace OsgFox;
using namespace OsgFox::Plugins;
using namespace OsgFox::Plugins::CalculateEnergy;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Component, Component::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Component::Component() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Component::~Component()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Component::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IMenuEntry::IID:
    return static_cast < Usul::Interfaces::IMenuEntry*>(this);
  case Usul::Interfaces::ICommand::IID:
    return static_cast < Usul::Interfaces::ICommand*>(this);
#ifdef _DEBUG
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
#endif
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the energy of the molecule
//
///////////////////////////////////////////////////////////////////////////////

void Component::execute ( Usul::Interfaces::IUnknown *caller )
{
  typedef ReaderWriterPDB::Molecules Molecules;
  typedef osgPlugins::pdb::Molecule::Atoms Atoms;

  //Get needed interfaces
  Usul::Interfaces::INotifyString::ValidQueryPtr   notify       ( caller );
  Usul::Interfaces::IActiveView::ValidQueryPtr     activeView   ( caller );
  Usul::Interfaces::IOpenSceneGraph::ValidQueryPtr osg          ( activeView->getActiveView() );
  Usul::Interfaces::IGetDocument::ValidQueryPtr    getDocument  ( activeView->getActiveView() );
  Usul::Interfaces::IDocument::ValidQueryPtr       document     ( getDocument->getDocument() );
  Usul::Interfaces::IGetMolecules::QueryPtr        getMolecules ( document );

  //get the scene
  osg::ref_ptr<osg::Group> model ( dynamic_cast < osg::Group * > ( osg->osgReferenced() ) );

  if( getMolecules.valid() )
  {
    Usul::Interfaces::IPsfFile::ValidQueryPtr    psfFile   ( document );
    Usul::Interfaces::IParamFile::ValidQueryPtr  paramFile ( document );
    
    //Get the molecules
    Molecules molecules ( getMolecules->getMolecules() );

    //notify user if there are no molecules
    if( molecules.size() == 0 )
    {
      notify->notify( this->queryInterface( Usul::Interfaces::IUnknown::IID ), "No Molecules" );
      return;
    }

    //For now only handle one
    //Mindy only handles one molecule at this time
    osgPlugins::pdb::Molecule *molecule = molecules.at(0).get();

    //Get the atoms
    Atoms &atoms ( molecule->atoms() );
    unsigned int numAtoms ( atoms.size() );
    
    Mindy::Vector *positions = new Mindy::Vector[ numAtoms ];

    //Populate the positions of the atoms
    for( unsigned int i = 0; i < atoms.size(); ++i )
    {
      const osg::Vec3 &center ( atoms.at( i )->getCenter() );
      positions[i][0] = center[0];
      positions[i][1] = center[1];
      positions[i][2] = center[2];
    }

    //Useful typedefs
    typedef Usul::Interfaces::ILoadFileDialog ILoadFileDialog;
    typedef ILoadFileDialog::Filter Filter;
    typedef ILoadFileDialog::Filters Filters;

    ILoadFileDialog::ValidQueryPtr loadFileDialog ( caller );
    Filters filters ( 1 );

    // TODO 1482321803. Below doesn't compile now, but I think it was a quick proof of concept. Revisit and fix.
#if 0

    //Get the pdb filename from the document
    Usul::Interfaces::IDocument::Filename pdbfile ( document->filename() );

    //if document does not have filename, prompt user for pdb file
    if( pdbfile.first.empty() )
    {
      //get the pdb file
      filters.at( 0 ) = Filter( "PDB file (*.pdb)", "*.pdb" );
      pdbfile.first = loadFileDialog->getLoadFileName ( "PDB File", filters );

      //set the document pdb file to one given
      document->filename( pdbfile.first );
    }

    //get the psf file from the document
    std::string psffile ( psfFile->psfFile() );

    //if the document doesn't have one, get from user
    if( psffile.empty() )
    {
      //get the psf file
      filters.at( 0 ) = Filter( "PSF file (*psf)", "*.psf" );
      psffile = loadFileDialog->getLoadFileName ( "PSF File", filters );

      //set the document psf file to the one given
      psfFile->psfFile( psffile );
    }

    //get the param file from the document
    std::string paramfile ( paramFile->paramFile() );

    //if the document doesn't have one, get from user
    if( paramfile.empty() )
    {
      //get the param file
      filters.at( 0 ) = Filter( "Parameter file (*.params)", "*.params" );
      paramfile = ( loadFileDialog->getLoadFileName ( "Param File", filters ) );

      //set the document param file to one given
      paramFile->paramFile( paramfile );
    }

    //if we have all needed file names...
    if ( !pdbfile.first.empty() && !psffile.empty() && !paramfile.empty() )
    {
      Mindy::CalculateEnergy calcEnergy ( pdbfile.first, psffile, paramfile );

      //Get the energy values from Mindy
      Mindy::Energies energies ( calcEnergy.calculate( positions, numAtoms ) );

      std::ostringstream os;
      os << "Total Energy " << energies.total << std::endl
        << "Bond Energy " << energies.bond << std::endl
        << "Angle Energy " << energies.angle << std::endl
        << "Improper Energy " << energies.improper << std::endl
        << "Dihedral Energy " << energies.dihedral << std::endl
        << "Electrostatic Energy " << energies.electrostatic << std::endl
        << "VanDerWalls Energy " << energies.vanderwaals << std::endl;

      //notify user of the results of the energy calculations
      notify->notify( this->queryInterface( Usul::Interfaces::IUnknown::IID ), os.str() );
    }
#endif

    delete positions;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate Energy.
//
///////////////////////////////////////////////////////////////////////////////

std::string Component::menuText() const
{
  return "Calculate Energy";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string Component::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string Component::statusBarText() const
{
  return "Calculate Energy with the current positions.";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string Component::menuGroup() const
{
  return "";
}