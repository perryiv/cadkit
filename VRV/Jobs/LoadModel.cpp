
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

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Components/Manager.h"

#include "OsgTools/IO/Reader.h"

#include <iostream>

using namespace VRV::Jobs;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadModel::LoadModel( const std::string& filename, Usul::Interfaces::IUnknown *caller ) :
  BaseClass(),
  _filename ( filename ),
  _caller ( caller )
{
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
  typedef void (LoadModel::*Function) ( const std::string &, unsigned long, unsigned long ); 
  typedef Usul::Adaptors::MemberFunction < LoadModel*, Function > MemFun;
  typedef OsgTools::IO::Reader::ReaderCallback < MemFun > Callback;

  // Make the reader.
  OsgTools::IO::Reader reader;

  // Set the callback.
  Callback callback ( MemFun ( this, &LoadModel::_updateProgress ) );
  reader.callback ( &callback );

  // Read the file.
  reader.read( _filename );

  // Get the node.
  osg::ref_ptr < osg::Node > model ( reader.node() );

  // Do any post-processing.
  this->_postProcessModelLoad ( _filename, model.get() );

  // See if our caller implements the needed interface.
  VRV::Interfaces::IModelAdd::QueryPtr modelAdd ( _caller );

  // Add the model.
  if ( modelAdd.valid() )
    modelAdd->addModel ( model.get(), _filename );
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

void LoadModel::_updateProgress ( const std::string& filename, unsigned long bytes, unsigned long total )
{
  float percentage ( static_cast < float > ( bytes ) / total );
  unsigned int t ( static_cast < unsigned int > ( percentage * 100 ) );

  if ( t % 10 == 0 )
    std::cout << filename << ": " << t << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post-process the model loading.
//
///////////////////////////////////////////////////////////////////////////////

void LoadModel::_postProcessModelLoad ( const std::string &filename, osg::Node *model )
{
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

