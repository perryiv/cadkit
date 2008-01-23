
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "MpdJob.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Components/Manager.h"
#include "Usul/System/Host.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Functions.h"
#include "Usul/Math/MinMax.h"
#include "Usul/System/Directory.h"

#include "Usul/Interfaces/IMemoryPool.h"
#include "Usul/Interfaces/IMpdNavigator.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "osg/Switch"

//USUL_IMPLEMENT_COMMAND ( MpdJob );


///////////////////////////////////////////////////////////////////////////////
//
//  Model Presentation Job Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdJob::MpdJob ( Usul::Documents::Document* document, 
                 Usul::Interfaces::IUnknown *caller, 
                 unsigned int index,
                 const std::string &lockfile,
                 const std::string &workingDir,
                 MpdDefinitions::MpdDynamicSets &dynamicSets ) :
  BaseClass ( caller ),
  _document ( document ),
  _caller( caller ),
  _index ( index ),
  _lockfile( lockfile ),
  _workingDir( workingDir ),
  _dynamicSets( dynamicSets )
{
  USUL_TRACE_SCOPE;
}

  
///////////////////////////////////////////////////////////////////////////////
//
//  Model Presentation Job Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdJob::~MpdJob()
{
  USUL_TRACE_SCOPE;

  // clean up circular references
  _document.release();
  _caller.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns true if a string starts with the string passed in
//
///////////////////////////////////////////////////////////////////////////////

struct StartsWith : std::unary_function<std::string,bool>
{
  StartsWith ( const std::string &s ) : _s ( s ){} 
  bool operator () ( const std::string &s ) const
  {
    
    if ( s.size() < _s.size() )
      return false;
    const std::string temp ( s.begin(), s.begin() + _s.size() );
    return ( temp == _s );
  }
private:
  std::string _s;

};


///////////////////////////////////////////////////////////////////////////////
//
//  Start the job.
//
///////////////////////////////////////////////////////////////////////////////

void MpdJob::_started ()
{
  USUL_TRACE_SCOPE;
  // Get needed interfaces.
  IDataSync::QueryPtr dataSync ( Usul::Components::Manager::instance().getInterface ( IDataSync::IID ) );

  std::string hostname = Usul::System::Host::name();
//  dataSync->setDataFlag( hostname, _lockfile );

#if 0
  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( _document );
  nav->findFiles( _index, _caller );
  //nav->validateDynamicSets();
#else
  this->_findFiles( _index, _caller );
#endif
  dataSync->resetData( hostname, _lockfile );

}


///////////////////////////////////////////////////////////////////////////////
//
// check animation state
//
///////////////////////////////////////////////////////////////////////////////

void MpdJob::_findFiles( unsigned int index, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  
   // Get all the files in the directory "dir"
  //std::cout << "Checking " << dir << " for files with extension " << ext << std::endl;
  std::string dir = _dynamicSets.at( index ).directory;
  std::string ext = _dynamicSets.at( index ).extension;
  std::string prefix = _dynamicSets.at( index ).prefix;
  Files files;

  Usul::File::find( dir, ext, files );
  //std::cout << files.size() << " file(s) found" << std::endl;
  // If we don't find any files matching our search criteria ...
  if( 0 == files.size() )
  {
    return;
  }
  

  // Make sure we have some files and that they are different from our last look
  // at the directory.
  unsigned int numFilesInDirectory = 0;

  if( files.size() > 0 && files.size() != _dynamicSets.at( index ).groups.size() )
  {
    // get the number of files in the directory
    numFilesInDirectory = files.size();
    

    std::string slash ( Usul::File::slash() );

    #ifdef _MSC_VER
      std::string search ( dir + slash + prefix );
    #else
      std::string search ( prefix );
    #endif

    // Get all files with the prefix name.
    std::cout << "Checking directory: " << dir + slash << " for files with prefix: " << prefix << std::endl;

    Files::iterator end ( std::remove_if 
                                 ( files.begin(), files.end(),  
                                 std::not1( StartsWith( search ) ) ) );

      
    // Remove the files that don't match our sort criteria
    files.erase ( end, files.end() );

    // Sort the names.
    std::sort ( files.begin(), files.end() );
    if ( false == files.empty() )
    {
      

       this->_parseNewFiles( files, index, caller );
       if( files.size() > _dynamicSets.at( index ).modelNames.size() )
       {
        _dynamicSets.at( index ).modelNames = files;
       }

       std::cout << _dynamicSets.at( index ).groups.size() << " files loaded for dynamic set: " << _dynamicSets.at( index ).name << std::endl;
    }
    

  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Load new files into the dynamic set at index <index>
//
///////////////////////////////////////////////////////////////////////////////

void MpdJob::_parseNewFiles( Files files, unsigned int index, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  // Find all the files that are new
  Files b = _dynamicSets.at( index ).modelNames;
  std::sort ( files.begin(), files.end() );
  std::sort ( b.begin(), b.end() );
  Files c;
  c.reserve ( b.size() );
  std::back_insert_iterator<Files> answer( c ) ;
  std::set_difference ( files.begin(), files.end(), b.begin(), b.end(), answer );

  // load each new file into model presentation
  std::cout << Usul::Strings::format( "Found ", c.size(), " new files.  Loading new files." ) << std::endl;
  for( Files::const_iterator iter = c.begin(); iter < c.end(); ++iter )
  {
    this->_loadNewDynamicFiles( *iter, index, caller );
  }
  std::cout << "All new files loaded!" << std::endl;

}


///////////////////////////////////////////////////////////////////////////////
//
// Load new files into the dynamic set at index <index>
//
///////////////////////////////////////////////////////////////////////////////

void MpdJob::_loadNewDynamicFiles( std::string filename, unsigned int index, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  MpdDefinitions::MpdDynamicGroup group;
  group.filename = filename;
  group.valid = false;
  osg::ref_ptr< osg::Group > node = new osg::Group;

  // Get needed interfaces.
  IDataSync::QueryPtr dataSync ( Usul::Components::Manager::instance().getInterface ( IDataSync::IID ) );
  if( false == dataSync.valid() )
    throw std::runtime_error ( "Error 3960013514: Failed to find a valid interface to Usul::Interfaces::IDataSync " );

  std::string hostname = Usul::System::Host::name();
  std::string lockfile = Usul::File::base( filename );

  // lock here
  dataSync->setDataFlag( hostname, lockfile );
  
  // load the new model
  node->addChild( this->_loadFile( filename, caller, 0x0 ) );
  _dynamicSets.at( index ).groups.push_back( group );
  _dynamicSets.at( index ).models->addChild( node.get(), false );
  _dynamicSets.at( index ).endTime = _dynamicSets.at( index ).groups.size() - 1;
//  _globalTimelineEnd = Usul::Math::maximum( _globalTimelineEnd, _dynamicSets.at( index ).endTime - 1 );

  // unlock here
  dataSync->resetData( hostname, lockfile );

  //build the scene
  Usul::Interfaces::IBuildScene::QueryPtr build ( _document );
  if( true == build.valid() )
  {
    Usul::Interfaces::IBuildScene::Options opt;
    build->buildScene( opt, _caller );
  }

  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( _document );
  if( true == nav.valid() )
  {
    nav->updateGlobalEndtime();
  }
  

}


///////////////////////////////////////////////////////////////////////////////
//
//  Load a single model.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* MpdJob::_loadFile( const std::string& filename, IUnknown *caller, IUnknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  osg::ref_ptr< osg::Group > group ( new osg::Group );
  std::cout << filename << " single file loading..." << std::endl;

  try
  {
     // This will create a new document.
    Info info ( DocManager::instance().find ( filename, caller ) );
    
    // Check to see if we created a document.
    if ( true == info.document.valid() && info.document->canOpen( filename ) )
    {
      
      // Ask the document to open the file.
      try
      {
        Usul::System::Directory::ScopedCwd cwd ( _workingDir );
        
        this->_openDocument ( Usul::File::fullPath( filename ), info.document.get(), caller, progress );

        // Disable Memory pools
        {
          Usul::Interfaces::IMemoryPool::QueryPtr pool ( info.document );
          if ( true == pool.valid() )
          {
            pool->usePool ( false );
          }
        }
        
        Usul::Interfaces::IBuildScene::QueryPtr build ( info.document.get() );
        if ( true == build.valid() )
        {

          OsgTools::Triangles::TriangleSet::Options opt;
          opt[ "normals" ] = "per-vertex";
          opt[ "colors" ]  = "per-vertex";

          // tell Triangle set to build its scene and assign the resulting node
          // to our internal node.

          // Reference count should go from 1->0 but not delete.
          group->addChild( build->buildScene( opt, caller ) );
          
         }
      }
      catch( ... )
      {
        std::cout << "\tUnable to open file: " << filename << std::endl;
      }
    }
    else
      std::cout << "\tUnknown file format for: " << filename << std::endl;
  }
  catch( ... )
  {
    std::cout << "\tUnknown file format for: " << filename << std::endl;
  }

  return group.release();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void MpdJob::_openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller, IUnknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( 0x0 == document )
    return;
#if 1
  if( true == document->canOpen( file ) )
    document->open ( file, caller, progress );
  else
    std::cout << "Unable to open document for file: " << file << std::endl;
#else
    document->open ( file, caller );
#endif
  std::cout << "Done" << std::endl;
  
	
}




