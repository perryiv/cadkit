
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Commands/LoadDocument.h"
#include "VRV/Interfaces/IModelAdd.h"
#include "VRV/Interfaces/IPostModelLoad.h"

#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/Interfaces/IUpdateSubject.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Components/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/System/Directory.h"
#include "Usul/File/Path.h"

#include "OsgTools/DisplayLists.h"

#include "osg/ref_ptr"
#include "osg/Node"

#include <iostream>

using namespace VRV::Commands;

USUL_IMPLEMENT_COMMAND ( LoadDocument );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadDocument::LoadDocument ( const Filenames& filenames, Usul::Interfaces::IUnknown * caller ) : 
  BaseClass ( caller ),
  _filenames ( filenames )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadDocument::~LoadDocument ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void LoadDocument::_execute()
{
  // Create a job.
  LoadJob::RefPtr job ( new LoadJob ( _filenames, this->caller() ) );

  // Add the job to the manager.
  Usul::Jobs::Manager::instance().addJob ( job.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadDocument::LoadJob::LoadJob( const Filenames& filenames, Usul::Interfaces::IUnknown *caller ) :
  BaseClass( caller ),
  _filenames ( filenames ),
  _caller ( caller ),
  _secondProgressBar ( static_cast < Usul::Interfaces::IUnknown* > ( 0x0 ) )
{
  Usul::Interfaces::IProgressBarFactory::QueryPtr factory ( caller );

  if ( factory.valid() )
    _secondProgressBar = factory->createProgressBar().get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadDocument::LoadJob::~LoadJob()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Job has started.
//
///////////////////////////////////////////////////////////////////////////////

void LoadDocument::LoadJob::_started()
{
  USUL_TRACE_SCOPE;

  /// Progress.
  Usul::Interfaces::IProgressBar::ShowHide showHide  ( this->progress()   );
  Usul::Interfaces::IProgressBar::ShowHide showHide2 ( _secondProgressBar.get() );

  // Set the label.
  this->_setLabel ( "Total file loading progress..." );

  // Loop through each file and insert them into the document.
  for ( Filenames::iterator iter = _filenames.begin(); iter != _filenames.end(); ++iter )
  {
    // The filename
    const std::string filename ( *iter );

    // Load the model.
    this->_loadModel ( filename );

    // Update for progress.
    this->_updateProgress ( iter - _filenames.begin(), _filenames.size(), true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the model.
//
///////////////////////////////////////////////////////////////////////////////

void LoadDocument::LoadJob::_loadModel( const std::string& filename )
{
  USUL_TRACE_SCOPE;

  // Find a document that can load the first filename.
  Usul::Documents::Document::RefPtr document ( Usul::Documents::Manager::instance().find ( filename ).document );

  if ( 0x0 == document.get() )
    return;

  Usul::Interfaces::IBuildScene::QueryPtr buildScene ( document );

  // Make sure the document can build a scene.
  if ( false == buildScene.valid() )
    return;

  // Set the label.
  Usul::Interfaces::IStatusBar::UpdateStatusBar label ( _secondProgressBar.get() );
  label ( "Loading filename: " + filename, true );
  std::cout << "Loading filename: " + filename << std::endl;

  if ( document->canOpen ( filename ) )
  {
    document->open ( filename, _caller.get(), _secondProgressBar.get() );
  }

  Usul::Interfaces::IView::QueryPtr view ( _caller );

  // Set the view.
  if ( view.valid() )
    document->addView ( view );

  // Get the node.
  osg::ref_ptr < osg::Node > model ( buildScene->buildScene ( document->options() ) );

  // Do any post-processing.
  this->_postProcessModelLoad ( filename, model.get() );

  // See if our caller implements the needed interface.
  VRV::Interfaces::IModelAdd::QueryPtr modelAdd ( _caller );

  // Add the model.
  if ( modelAdd.valid() )
    modelAdd->addModel ( model.get(), filename );

  // Add the document.
  Usul::Documents::Manager::instance().add ( document );

  // Make the document active.
  Usul::Documents::Manager::instance().activeDocument ( document );

  // See if the caller is an update subject.
  Usul::Interfaces::IUpdateSubject::QueryPtr subject ( _caller );
  if ( subject.valid () )
    subject->addUpdateListener ( document->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post-process the model loading.
//
///////////////////////////////////////////////////////////////////////////////

void LoadDocument::LoadJob::_postProcessModelLoad ( const std::string &filename, osg::Node *model )
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

