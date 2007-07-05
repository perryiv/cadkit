
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Jobs/LoadDirectory.h"
#include "VRV/Jobs/LoadModel.h"

#include "Usul/Jobs/Manager.h"
#include "Usul/System/Sleep.h"
#include "Usul/File/Path.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/System/Directory.h"

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include <vector>
#include <string>

using namespace VRV::Jobs;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( LoadDirectory, LoadDirectory::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadDirectory::LoadDirectory( const std::string &directory, Usul::Interfaces::IUnknown *caller ) :
  BaseClass( caller ),
  _directory ( directory ),
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

LoadDirectory::~LoadDirectory()
{
  Usul::Interfaces::IProgressBar::QueryPtr progressBar ( _secondProgressBar );
  if( progressBar.valid() )
    progressBar->hideProgressBar ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all the files with the given extension.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class Container >
  void findFiles ( const boost::filesystem::path & path, const std::string& ext, Container& c )
  {
    typedef boost::filesystem::directory_iterator Iterator;

    Iterator iter ( path );
    Iterator end;
    for( ; iter != end; ++iter )
    {
      // Make a recursive call if its a directory.
      if( boost::filesystem::is_directory ( iter->status() ) )
      {
        findFiles ( iter->path(), ext, c );
      }

      // Add it to our list if its a file and the extenstion matches.
      else if ( Usul::File::extension ( iter->leaf() ) == ext )
      {
        c.push_back ( iter->leaf() );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the model.
//
///////////////////////////////////////////////////////////////////////////////


void LoadDirectory::_loadModel ( const std::string& filename )
{
  // Create a job.
  Usul::Jobs::Job::RefPtr job ( new LoadModel ( filename, this->queryInterface ( Usul::Interfaces::IUnknown::IID ), false ) );

  // Add the job to the manager.
  Usul::Jobs::Manager::instance().add ( job.get() );

  // Wait until it's done.
  while ( false == job->isDone() )
    Usul::System::Sleep::seconds( 5 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the job.
//
///////////////////////////////////////////////////////////////////////////////

void LoadDirectory::_started()
{
  typedef std::vector < std::string > Filenames;

  boost::filesystem::path path ( _directory );

  if ( boost::filesystem::is_directory ( path ) )
  {
    // Find all the files we want to load.
    Filenames filenames;
    Detail::findFiles ( path, "osg", filenames );
    Detail::findFiles ( path, "ive", filenames );

    // Scope the directory change.
    Usul::System::Directory::ScopedCwd cwd ( _directory );

    // Load all the filenames we found.
    Usul::Interfaces::IProgressBar::ShowHide showHide ( this->progress() );
    for ( Filenames::iterator iter = filenames.begin(); iter != filenames.end(); ++iter )
    {
      // Load the filename.
      this->_loadModel ( *iter );

      // Update for progress.
      this->_updateProgress ( iter - filenames.begin(), filenames.size() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the new model.
//  This will re-direct to the caller.
//
///////////////////////////////////////////////////////////////////////////////

void LoadDirectory::addModel ( osg::Node *model, const std::string& filename )
{
  VRV::Interfaces::IModelAdd::QueryPtr modelAdd ( _caller );

  if ( modelAdd.valid () )
    modelAdd->addModel ( model, filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a progress bar.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown*  LoadDirectory::createProgressBar()
{
  return _secondProgressBar.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for an interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* LoadDirectory::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IProgressBarFactory::IID:
    return static_cast < Usul::Interfaces::IProgressBarFactory* > ( this );
  case VRV::Interfaces::IModelAdd::IID:
    return static_cast < VRV::Interfaces::IModelAdd * > ( this );
  default:
    return 0x0;
  }
}

