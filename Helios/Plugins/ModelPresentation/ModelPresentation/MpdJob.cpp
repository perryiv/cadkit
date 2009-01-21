
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/ModelPresentation/ModelPresentation/MpdJob.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Components/Manager.h"
#include "Usul/System/Host.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Functions.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Scope/CurrentDirectory.h"

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

MpdJob::MpdJob ( Usul::Interfaces::IUnknown *caller,
                 const std::string &workingDir,
                 const std::string &searchDir, 
                 const std::string &prefix, 
                 const std::string &extension,
                 Files current ) :
  BaseClass ( caller, false ),
  _root( 0 ),
  _workingDir( workingDir ),
  _searchDir( searchDir ),
  _prefix( prefix ),
  _extension( extension ),
  _currentFiles( current ),
  _foundNewData( false )
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
  this->_findFiles();
}


///////////////////////////////////////////////////////////////////////////////
//
// check animation state
//
///////////////////////////////////////////////////////////////////////////////

void MpdJob::_findFiles()
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this ); 

  MpdDefinitions::MpdDynamicSetHeader header = this->getHeader();

  Files files;

  Usul::File::find( header.directory, header.extension, files );
  //std::cout << files.size() << " file(s) found" << std::endl;
  // If we don't find any files matching our search criteria ...
  if( 0 == files.size() )
  {
    return;
  }
  
  // Make sure we have some files and that they are different from our last look
  // at the directory.
  unsigned int numFilesInDirectory = 0;

  if( files.size() > 0 && files.size() != header.modelNames.size() )
  {
    // get the number of files in the directory
    numFilesInDirectory = files.size();
    

    std::string slash ( Usul::File::slash() );

    #ifdef _MSC_VER
    std::string search ( header.directory + slash + header.prefix );
    #else
      std::string search ( header.prefix );
    #endif

    // Get all files with the prefix name.
    //std::cout << "\rChecking directory: " << header.directory + slash << " for files with prefix: " << header.prefix << std::flush;

    Files::iterator end ( std::remove_if 
                                 ( files.begin(), files.end(),  
                                 std::not1( StartsWith( search ) ) ) );

      
    // Remove the files that don't match our sort criteria
    files.erase ( end, files.end() );

    // Sort the names.
    std::sort ( files.begin(), files.end() );
    if ( false == files.empty() )
    {

       this->_parseNewFiles( files, 0x0, 0x0 );
#if 0
       if( files.size() > header.modelNames.size() )
       {
         header.modelNames = files;
       }
#endif
       //std::cout << _currentFiles.size() << " files loaded for dynamic set: " << _prefix << std::endl;
    }
    
    //this->setHeader( header );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Load new files into the dynamic set at index <index>
//
///////////////////////////////////////////////////////////////////////////////

void MpdJob::_parseNewFiles( Files files, Usul::Interfaces::IUnknown *caller, IUnknown *progress )
{
  USUL_TRACE_SCOPE;
 // Guard guard ( this ); 

  MpdDefinitions::MpdDynamicSetHeader header = this->getHeader();
  // Find all the files that are new
  Files b = header.modelNames;
  std::sort ( files.begin(), files.end() );
  std::sort ( b.begin(), b.end() );
  Files c;
  c.reserve ( b.size() );
  std::back_insert_iterator<Files> answer( c ) ;
  std::set_difference ( files.begin(), files.end(), b.begin(), b.end(), answer );

  // load each new file into model presentation
  //std::cout << Usul::Strings::format( "Found ", c.size(), " new files.  Loading new files." ) << std::endl;
#if 0
  for( Files::const_iterator iter = c.begin(); iter < c.end(); ++iter )
  {
    this->_loadNewDynamicFiles( *iter, caller, progress );
  }
#else
  if( c.size() > 0 )
  {
    this->_loadNewDynamicFiles( c.at( 0 ), caller, progress );
    header.modelNames.push_back( c.at( 0 ) );
    this->setHeader( header );
  }
#endif
  //std::cout << "All new files loaded!" << std::endl;
  this->foundNewData( true );
}


///////////////////////////////////////////////////////////////////////////////
//
// Load new files into the dynamic set at index <index>
//
///////////////////////////////////////////////////////////////////////////////

void MpdJob::_loadNewDynamicFiles( std::string filename, Usul::Interfaces::IUnknown *caller, IUnknown *progress )
{
  USUL_TRACE_SCOPE;
 // Guard guard ( this ); 
  osg::ref_ptr< osg::Group > node = new osg::Group;
  
  // load the new model
  node->addChild( this->_loadFile( filename, caller, progress ) );

  // guard and add to the root
  {
    Guard guard ( this ); 
    _root.push_back( node.get() );
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
  //Guard guard ( this->mutex() );
  osg::ref_ptr< osg::Group > group ( new osg::Group );
  //std::cout << filename << " single file loading..." << std::endl;

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
        Usul::Scope::CurrentDirectory cwd ( _workingDir );
        
        //this->_openDocument ( Usul::File::fullPath( filename ), info.document.get(), caller, progress );
        //this->_openDocument ( Usul::Strings::format ( _searchDir, '/', filename ), info.document.get(), caller, progress );
        this->_openDocument ( Usul::Strings::format ( _searchDir, '/', Usul::File::base( filename ), ".", Usul::File::extension( filename ) ), info.document.get(), caller, progress );

        // Disable Memory pools
        {
          Usul::Interfaces::IMemoryPool::QueryPtr pool ( info.document );
          if ( true == pool.valid() )
          {
            pool->usePool ( false );
          }
        }
        
        Usul::Interfaces::IBuildScene::QueryPtr build ( info.document );
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
      catch( const std::exception &e )
      {
        std::cout << "Error 1219987866: Standard exception caught while trying to open file: " << filename << ". Exception: " << e.what() << std::endl;
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
  //Guard guard ( this->mutex() );
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
    std::cout << "Done loading file: " << file << std::endl;
  
	
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the stored data for this job
//
///////////////////////////////////////////////////////////////////////////////

MpdDefinitions::Groups MpdJob::getData()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  MpdDefinitions::Groups group = _root;
  
  return group;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the header information for this job
//
///////////////////////////////////////////////////////////////////////////////

MpdDefinitions::MpdDynamicSetHeader MpdJob::getHeader()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  MpdDefinitions::MpdDynamicSetHeader header;
  header.directory = _searchDir;
  header.extension = _extension;
  header.prefix = _prefix;
  header.modelNames = _currentFiles;

  return header;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the header information for this job
//
///////////////////////////////////////////////////////////////////////////////

void MpdJob::setHeader( MpdDefinitions::MpdDynamicSetHeader h )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _searchDir = h.directory;
  _extension = h.extension;
  _prefix = h.prefix;
  _currentFiles = h.modelNames;

}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the state of new data discovery
//
///////////////////////////////////////////////////////////////////////////////

bool MpdJob::foundNewData()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  return _foundNewData;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state of new data discovery
//
///////////////////////////////////////////////////////////////////////////////

void MpdJob::foundNewData( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _foundNewData = state;
}
