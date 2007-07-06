
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Jobs/LoadModel.h"
#include "VRV/Interfaces/IModelAdd.h"
#include "VRV/Interfaces/IPostModelLoad.h"

#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Components/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/System/Directory.h"
#include "Usul/File/Path.h"

#include "OsgTools/IO/Reader.h"

#include <iostream>

using namespace VRV::Jobs;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadModel::LoadModel( const Filenames& filenames, Usul::Interfaces::IUnknown *caller ) :
  BaseClass( caller ),
  _filenames ( filenames ),
  _caller ( caller ),
  _secondProgressBar ( static_cast < Usul::Interfaces::IUnknown* > ( 0x0 ) )
{
  Usul::Interfaces::IProgressBarFactory::QueryPtr factory ( caller );

  if ( factory.valid() )
    _secondProgressBar = factory->createProgressBar ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadModel::~LoadModel()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Job has started.
//
///////////////////////////////////////////////////////////////////////////////

void LoadModel::_started()
{
  USUL_TRACE_SCOPE;

  Usul::Interfaces::IProgressBar::ShowHide showHide  ( this->progress()   );
  Usul::Interfaces::IProgressBar::ShowHide showHide2 ( _secondProgressBar.get() );

  // Set the label.
  this->_setLabel ( "Total file loading progress..." );

  for ( Filenames::iterator iter = _filenames.begin(); iter != _filenames.end(); ++iter )
  {
    // Load the model.
    this->_loadModel ( *iter );

    // Update for progress.
    this->_updateProgress ( iter - _filenames.begin(), _filenames.size(), true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the model..
//
///////////////////////////////////////////////////////////////////////////////

void LoadModel::_loadModel( const std::string& filename )
{
  USUL_TRACE_SCOPE;

  typedef void (LoadModel::*Function) ( const std::string &, unsigned long, unsigned long ); 
  typedef Usul::Adaptors::MemberFunction < LoadModel*, Function > MemFun;
  typedef OsgTools::IO::Reader::ReaderCallback < MemFun > Callback;

  std::string directory ( Usul::File::directory ( filename, false ) );

  // Scope the directory change.
  Usul::System::Directory::ScopedCwd cwd ( directory );

  // Set the label.
  Usul::Interfaces::IStatusBar::UpdateStatusBar label ( _secondProgressBar.get() );
  label ( "Loading filename: " + filename, true );

  // Make the reader.
  OsgTools::IO::Reader reader;

  // Set the callback.
  Callback callback ( MemFun ( this, &LoadModel::_updateProgressCallback ) );
  reader.callback ( &callback );

  // Read the file.
  reader.read( filename );

  // Get the node.
  osg::ref_ptr < osg::Node > model ( reader.node() );

  // Do any post-processing.
  this->_postProcessModelLoad ( filename, model.get() );

  // See if our caller implements the needed interface.
  VRV::Interfaces::IModelAdd::QueryPtr modelAdd ( _caller );

  // Add the model.
  if ( modelAdd.valid() )
    modelAdd->addModel ( model.get(), filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Job has finished.
//
///////////////////////////////////////////////////////////////////////////////

void LoadModel::_finished()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress.
//
///////////////////////////////////////////////////////////////////////////////

void LoadModel::_updateProgressCallback ( const std::string& filename, unsigned long bytes, unsigned long total )
{
  USUL_TRACE_SCOPE;

  Usul::Interfaces::IProgressBar::QueryPtr progressBar ( _secondProgressBar );

  // Report progress.
  if ( progressBar.valid() )
  {
    const double n ( static_cast < double > ( bytes ) );
    const double d ( static_cast < double > ( total ) );
    const float fraction ( n / d );
    progressBar->updateProgressBar ( static_cast < unsigned int > ( fraction * 100 ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post-process the model loading.
//
///////////////////////////////////////////////////////////////////////////////

void LoadModel::_postProcessModelLoad ( const std::string &filename, osg::Node *model )
{
  USUL_TRACE_SCOPE;

  // Sanity check...
  if ( true == filename.empty() || 0x0 == model )
    return;

  // To shorten the lines.
  typedef Usul::Components::Manager Manager;
  typedef Manager::UnknownSet UnknownSet;
  typedef VRV::Interfaces::IPostModelLoad PostProcess;

  // See if we have any appropriate plugins.
  UnknownSet unknowns ( Manager::instance().getInterfaces ( PostProcess::IID ) );

  // Loop through the found plugins.
  for ( UnknownSet::iterator i = unknowns.begin(); i != unknowns.end(); ++i )
  {
    // Grab the pointer.
    Usul::Interfaces::IUnknown::ValidRefPtr u ( *i );

    // Query for the one we need. This should always work.
    PostProcess::ValidQueryPtr pp ( u.get() );

    // Call the function.
    pp->postProcessModelLoad ( filename, model );
  }
}

