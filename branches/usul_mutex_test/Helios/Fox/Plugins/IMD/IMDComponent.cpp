
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "IMDComponent.h"
#include "IMD.h"

#include "osg/MatrixTransform"
#include "osg/Matrix"

#include "osgPlugins/pdb/Atom.h"
#include "osgPlugins/pdb/Molecule.h"

#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Functions/App.h"

#include "FoxTools/Headers/App.h"
#include "FoxTools/Headers/DialogBox.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/Matrix.h"
#include "FoxTools/Headers/Label.h"
#include "FoxTools/Headers/TextField.h"
#include "FoxTools/Headers/HorizontalFrame.h"
#include "FoxTools/Headers/Button.h"

#include "Usul/Cast/Cast.h"
#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/IOpenSceneGraph.h"
#include "Usul/Interfaces/IStatusBar.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/IGetMolecules.h"
#include "Usul/Interfaces/INotify.h"
#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IFlushEvents.h"
#include "Usul/System/Socket.h"
#include "Usul/Endian/Endian.h"

#include <sstream>

using namespace OsgFox;
using namespace OsgFox::Plugins;
using namespace OsgFox::Plugins::IMD;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( IMDComponent , IMDComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( IMDComponent ) MessageMap[] = 
{
  //          Message Type     ID                                   Message Handler.
  FXMAPFUNC ( FX::SEL_COMMAND, IMDComponent::ID_FIX_ATOM,     IMDComponent::onCommandFixAtom  ),
};

FOX_TOOLS_IMPLEMENT ( IMDComponent, FX::FXObject, MessageMap, ARRAYNUMBER ( MessageMap ) );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

IMDComponent::IMDComponent() : BaseClass(),
_caller( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

IMDComponent::~IMDComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *IMDComponent::queryInterface ( unsigned long iid )
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
//  Add atom info tool to the active view.
//
///////////////////////////////////////////////////////////////////////////////

void IMDComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  _caller = caller;

  Usul::Interfaces::IActiveView::QueryPtr activeView( caller );
  
  if( activeView.valid() )
  {
    FX::FXRegistry &reg = FoxTools::Functions::application()->reg();

    const FX::FXchar *hostname   = reg.readStringEntry  ( "IMD Connect", "Hostname", "" );
    const FX::FXchar *portnumber = reg.readStringEntry  ( "IMD Connect", "Port", "" );

    std::auto_ptr <FX::FXDialogBox> dialog ( new FX::FXDialogBox ( reinterpret_cast < FX::FXWindow*> ( FoxTools::Functions::mainWindow() ) , "IMD Connect" ) );

    FX::FXVerticalFrame* theFrame ( new FX::FXVerticalFrame ( dialog.get(), LAYOUT_FILL_X|LAYOUT_FILL_Y ) );

    FX::FXMatrix *matrix ( new FXMatrix( theFrame ,2, LAYOUT_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y|MATRIX_BY_ROWS) );

    //Labels
    FX::FXLabel* hostName   ( new FX::FXLabel  ( matrix, "Host Name:",   0x0, FX::LAYOUT_RIGHT ) );
    FX::FXLabel* portNumber ( new FX::FXLabel  ( matrix, "Port Number:", 0x0, FX::LAYOUT_RIGHT ) );
 

    FX::FXTextField *host ( new FX::FXTextField ( matrix, 75, 0x0, 0, FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP|LAYOUT_LEFT,0,0,0,0 ) );
    FX::FXTextField *port ( new FX::FXTextField ( matrix, 75, 0x0, 0, FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP|LAYOUT_LEFT,0,0,0,0 ) );

    host->setText( hostname );
    port->setText( portnumber );

    //Accept and cancel buttons
    FX::FXHorizontalFrame*  buttonFrame ( new FX::FXHorizontalFrame ( theFrame, LAYOUT_FILL_X|LAYOUT_FILL_Y ) );
    FX::FXButton *accept ( new FX::FXButton ( buttonFrame, "Accept",NULL, dialog.get(), FX::FXDialogBox::ID_ACCEPT,FX::LAYOUT_LEFT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20) );
    FX::FXButton *cancel ( new FX::FXButton ( buttonFrame, "Cancel",NULL, dialog.get(), FX::FXDialogBox::ID_CANCEL,FX::LAYOUT_RIGHT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20) );

    dialog->create();

    if ( dialog->execute( FX::PLACEMENT_OWNER ) )
    {
      std::string hostname ( (char*) host->getText().text() );
      std::string portnum ( (char*) port->getText().text() );

      if( !hostname.empty() && !portnum.empty() )
      {
        reg.writeStringEntry( "IMD Connect", "Hostname", hostname.c_str() );
        reg.writeStringEntry( "IMD Connect", "Port", portnum.c_str() );
        this->_simulate ( hostname, portnum );
      }
    }
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu text.
//
///////////////////////////////////////////////////////////////////////////////

std::string IMDComponent::menuText() const
{
  return "IMD Connect...";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string IMDComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string IMDComponent::statusBarText() const
{
  return "Connect to running NAMD simulation";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string IMDComponent::menuGroup() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle when fixed atom is clicked.
//
///////////////////////////////////////////////////////////////////////////////

long IMDComponent::onCommandFixAtom  ( FX::FXObject *object, FX::FXSelector, void *data )
{

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the simulation
//
///////////////////////////////////////////////////////////////////////////////

void IMDComponent::_simulate( const std::string& hostName, const std::string& portNumber )
{
  std::istringstream in ( portNumber );
  unsigned int port;
  in >> port;

  //create socket and handshake with NAMD
  OsgFox::Plugins::IMD::IMD imd ( hostName, port );

  if( !imd.receiveHandshake() )
  {
    Usul::Interfaces::INotifyString::ValidQueryPtr notify ( _caller );
    notify->notify( this->queryInterface( Usul::Interfaces::IUnknown::IID ), "Could not establish a handshake." );
    return;
  }

  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
  Usul::Interfaces::ITextMatrix::ValidQueryPtr textMatrix( activeView->getActiveView() );
  textMatrix->createMatrix( 0, 1, 10, 2, 30, 320 );

  textMatrix->setText( 0, 1, 0, 0, "Total Energy: " );
  textMatrix->setText( 0, 1, 1, 0, "Bond Energy: " );
  textMatrix->setText( 0, 1, 2, 0, "Angle Energy: " );
  textMatrix->setText( 0, 1, 3, 0, "Dihedral Energy: " );
  textMatrix->setText( 0, 1, 4, 0, "VanDerWaals Energy: " );
  textMatrix->setText( 0, 1, 5, 0, "ElectroStatic Energy: " );

  Usul::Interfaces::IFlushEvents::ValidQueryPtr flush ( _caller );

  IMD::IMDType type;
  OsgFox::Plugins::IMD::int32 length;
  do
  {
    type = imd.receiveHeader( length );

    if( type == IMD::IOERROR )
      break;

    switch ( type )
    {
    case IMD::ENERGIES:
      this->_processEnergies( length, imd, textMatrix.get() );
      break;
    case IMD::FCOORDS:
      this->_processCoordinates( length, imd );
      break;
    }
    
    flush->flushEventQueue();

  } while ( type != IMD::IOERROR || type != IMD::DISCONNECT);

  textMatrix->removeMatrix( 0, 1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the coordinates from NAMD
//
///////////////////////////////////////////////////////////////////////////////

void IMDComponent::_processCoordinates ( int32 length, const OsgFox::Plugins::IMD::IMD& imd )
{
  //Get needed interfaces
  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
  Usul::Interfaces::IGetDocument::ValidQueryPtr getDocument( activeView->getActiveView() );
  Usul::Interfaces::IDocument::ValidQueryPtr document ( getDocument->getDocument() ); 
  Usul::Interfaces::IOpenSceneGraph::ValidQueryPtr osg ( activeView->getActiveView() );
  Usul::Interfaces::IGetMolecules::ValidQueryPtr getMolecules ( document );

  const unsigned int size ( 3 * length );
  float *coord = new float[ size ];

  //Get the coordinates from the socket
  imd.receiveCoords( length, coord );

  if( imd.need2Flip() )
  {
    //Flip the bytes
    for( unsigned int i = 0; i < size; ++i )
      Usul::Endian::reverseBytes ( ( Usul::Types::Uint32& ) coord[i] );
  }

  //Useful typedefs
  typedef Usul::Interfaces::IGetMolecules::Molecules Molecules;
  typedef Usul::Interfaces::IGetMolecules::MoleculePtr MoleculePtr;
  typedef osgPlugins::pdb::Molecule::Atoms Atoms;

  //Get the molecules
  Molecules molecules ( getMolecules->getMolecules() );

  //For now handle one molecule
  //NAMD only handles calculations for one molecule
  MoleculePtr molecule ( molecules.at(0 ) );

  //Get the atom list
  Atoms &atoms ( molecule->atoms() );

  unsigned int numAtoms ( atoms.size() );

  //Loop through the atoms and change their centers
  for( unsigned int i = 0; i < atoms.size(); ++i )
  {
    osg::MatrixTransform *mt ( atoms.at(i)->getMatrix() );
    osg::Matrix matrix ( mt->getMatrix() );

    unsigned int index ( i * 3 );
    osg::Vec3 center ( coord[i], coord[i + 1], coord[i + 2] );
    matrix.makeTranslate ( center );

    mt->setMatrix( matrix );
  }


  //Redraw
  Usul::Interfaces::IRedraw::ValidQueryPtr redraw ( activeView->getActiveView() );
  redraw->redraw();

  delete [] coord;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the energies from NAMD
//
///////////////////////////////////////////////////////////////////////////////

void IMDComponent::_processEnergies    ( int32 length, const OsgFox::Plugins::IMD::IMD& imd, Usul::Interfaces::ITextMatrix* textMatrix )
{
  Energies energy;

  //get the energies values from socket
  imd.receiveEnergies(&energy);

  if( imd.need2Flip() )
  {
    //swap Energies values
    Usul::Endian::reverseBytes ( energy.timestep );
    Usul::Endian::reverseBytes ( ( Usul::Types::Uint32& ) energy.temperature   );
    Usul::Endian::reverseBytes ( ( Usul::Types::Uint32& ) energy.total         );
    Usul::Endian::reverseBytes ( ( Usul::Types::Uint32& ) energy.potential     );
    Usul::Endian::reverseBytes ( ( Usul::Types::Uint32& ) energy.vanderwaals   );
    Usul::Endian::reverseBytes ( ( Usul::Types::Uint32& ) energy.electrostatic );
    Usul::Endian::reverseBytes ( ( Usul::Types::Uint32& ) energy.bond          );
    Usul::Endian::reverseBytes ( ( Usul::Types::Uint32& ) energy.angle         );
    Usul::Endian::reverseBytes ( ( Usul::Types::Uint32& ) energy.dihedral      );
    Usul::Endian::reverseBytes ( ( Usul::Types::Uint32& ) energy.improper      );
  }

  std::ostringstream os;

  os << energy.total;

  textMatrix->setText( 0, 1, 0, 1, os.str() );

  os.str("");
  os << energy.bond;

  textMatrix->setText( 0, 1, 1, 1, os.str() );

  os.str("");
  os << energy.angle;

  textMatrix->setText( 0, 1, 2, 1, os.str() );

  os.str("");
  os << energy.dihedral;

  textMatrix->setText( 0, 1, 3, 1, os.str() );

  os.str("");
  os << energy.vanderwaals;

  textMatrix->setText( 0, 1, 4, 1, os.str() );

  os.str("");
  os << energy.electrostatic;

  textMatrix->setText( 0, 1, 5, 1, os.str() );
}

