
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/DynamicLand/DynamicLandModel/DynamicLandDocument.h"

#include "Experimental/DynamicLand/DynamicLandModel/LoadNextTimestep.h"
#include "Experimental/DynamicLand/DynamicLandModel/FirstTimestep.h"
#include "Experimental/DynamicLand/DynamicLandModel/NextTimestep.h"
#include "Experimental/DynamicLand/DynamicLandModel/PrevTimestep.h"
#include "Experimental/DynamicLand/DynamicLandModel/LoadTimestep.h"
#include "Experimental/DynamicLand/DynamicLandModel/StartAnimation.h"
#include "Experimental/DynamicLand/DynamicLandModel/StopAnimation.h"

#include "Usul/Interfaces/IColorsPerVertex.h"
#include "Usul/Interfaces/IDisplaylists.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Interfaces/IMaterials.h"
#include "Usul/Interfaces/IMemoryPool.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Resources/StatusBar.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Interfaces/IVertices.h"
#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/File/Slash.h"

#include "OsgTools/DisplayLists.h"
#include "OsgTools/Group.h"

#include "osgDB/ReadFile"
#include "osgDB/WriteFile"
#include "osg/Group"

#include <iterator>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( DynamicLandDocument, DynamicLandDocument::BaseClass );


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
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

DynamicLandDocument::DynamicLandDocument() : 
  BaseClass ( "Dynamic Land Document" ),
  _currentDocument ( 0x0 ),
  _newDocument ( 0x0 ),
  _terrain ( new osg::Group ),
  _fileQueryDelay ( 1000 ),
  _animationDelay ( 2000 ),
  _currFileNum ( 0 ),
  _currFileLoaded ( -1 ),
  _numFilesInDirectory ( 0 ), 
  _files( 0x0 ),
  _loadNewMap ( false ),
  _dir( "" ),
  _ext ( "" ),
  _prefix ( "" ),
  _stepForward ( false ),
  _stepBackward ( false ),
  _timeStepPool ( 0x0 ),
  _timeStepWindowSize ( 1 ),
  _isAnimating ( false ),
  _updateAnimation ( false ),
  _setPoolSize ( true ),
  _jobManager ( static_cast<JobManager *> ( 0x0 ) )
{
  USUL_TRACE_SCOPE;
  _header.cellSize = 0;
  _header.noDataValue = 0;
  _header.gridSize = Usul::Math::Vec2ui( 0, 0 );
  _header.ll = Usul::Math::Vec2f( 0, 0 );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

DynamicLandDocument::~DynamicLandDocument()
{
  USUL_TRACE_SCOPE;
  _jobManager.reset();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::clear ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *DynamicLandDocument::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener* > ( this );
  case Usul::Interfaces::IDldNavigator::IID:
    return static_cast < Usul::Interfaces::IDldNavigator* > ( this );
  case Usul::Interfaces::ICommandList::IID:
    return static_cast < Usul::Interfaces::ICommandList* > ( this ); 
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cycle up our list of files matching our search criteria
//
///////////////////////////////////////////////////////////////////////////////

bool DynamicLandDocument::incrementFilePosition ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if( true == _files.empty() )
  {
    std::cout << "_files invalid" << std::endl;
    return false;
  }
  else
  {
    if( _currFileNum < _files.size() - 1 && _files.size() > 0 )
    {
      if( ( this->_timeStepWindowSize * 2 ) + 1 < this->numFiles() )
      {
        int index = this->currentFilePosition() - this->_timeStepWindowSize;
        if( index < 0 )
           index = this->numFiles() + index;
        

        //Spawn a job to kill the time step at <index>
        //KillJob::RefPtr job ( new KillJob ( this, 0x0, index ) );
        //Usul::Jobs::Manager::instance().add ( job.get() );
        
      }

      ++_currFileNum;
      std::cout << "\rCurrently at file: " << _files.at( _currFileNum ) << std::flush;
      return true;
    }
    else
    {
      if( _files.size() > 0 )
      {
        _currFileNum = 0;
        return true;
      }
      else
        return false;
    }
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Cycle down our list of files matching our search criteria
//
///////////////////////////////////////////////////////////////////////////////

bool DynamicLandDocument::decrementFilePosition ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if( true == _files.empty() )
  {
    std::cout << "_files invalid" << std::endl;
    return false;
  }
  else
  {
    if( _currFileNum > 0 && _files.size() > 0 )
    {
      if( ( this->_timeStepWindowSize * 2 ) + 1 < this->numFiles() )
      {
        int index = this->currentFilePosition() + this->_timeStepWindowSize;
        if( index > static_cast< int > ( this->numFiles() ) )
           index = 0 + index - static_cast< int > ( this->numFiles() );
        

        //Spawn a job to kill the time step at <index>
        //KillJob::RefPtr job ( new KillJob ( this, 0x0, index ) );
        //Usul::Jobs::Manager::instance().add ( job.get() );
      }
      --_currFileNum;
      std::cout << "\rCurrently at file: " << _files.at( _currFileNum ) << std::flush;
      return true;
    }
    else
    {
      if( _files.size() > 0 )
      {
        _currFileNum = _files.size() - 1;
        return true;
      }
      else
        return false;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the current file.
//
///////////////////////////////////////////////////////////////////////////////

  bool DynamicLandDocument::loadCurrentFile( bool loadFile )
  {
    USUL_TRACE_SCOPE;
    Guard guard ( this->mutex() );
    if ( _files.size() > 0 && _currFileNum < _files.size() )
    {
      _loadNewMap = loadFile;
      if( loadFile )
        std::cout << "\nLoading file: " << _files.at( _currFileNum ) << "..." << std::endl;
      return true;
    }
    else
      return false;
  }



///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool DynamicLandDocument::canExport ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool DynamicLandDocument::canInsert ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool DynamicLandDocument::canOpen ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "dld" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool DynamicLandDocument::canSave ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "dld" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::write ( const std::string &name, Unknown *caller  ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::read ( const std::string &name, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  document->load ( name );

  //TODO: change name to dld
  if ( "asc" == document->name() )
  {
    // if the parameter file is loaded correctly go ahead and load the 1st map
    this->_loadNewMap = this->_readParameterFile( *document, caller );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

DynamicLandDocument::Filters DynamicLandDocument::filtersSave() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "Dynamic Land (*.dld)", "*.dld" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

DynamicLandDocument::Filters DynamicLandDocument::filtersExport() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

DynamicLandDocument::Filters DynamicLandDocument::filtersOpen() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "Dynamic Land (*.dld)", "*.dld" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

DynamicLandDocument::Filters DynamicLandDocument::filtersInsert() const
{
  USUL_TRACE_SCOPE;
  return this->filtersOpen();
}

DynamicLandDocument::DocumentPtr DynamicLandDocument::document()
{
  Guard guard ( this->mutex() );
  if( 0x0 != this->_newDocument )
    return _newDocument.get();
  else
    return _currentDocument.get();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *DynamicLandDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  
  try
  {
    
    //this->_buildScene();
    {
      Guard guard ( this->mutex() );
      return _terrain.get();
    }
  }
  catch ( const std::exception& e )
  {
    std::cout << "Error 2642125610: Standard exceptiong caught: " << e.what() << std::endl;
  }
  catch ( ... )
  {
    std::cout << "Error 5186009000: Unknown exceptiong caught: " << std::endl;
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* DynamicLandDocument::LoadDataJob::_buildScene( Usul::Documents::Document* document )
{
  USUL_TRACE_SCOPE;
  osg::ref_ptr< osg::Group > group ( new osg::Group );
#if 1
  //Guard guard ( this->mutex() );
  
  // Disable materials in the TriangleSet
  

  //Debugging...
  std::cout << "Query Interface..." << std::endl;

   // Get interface to build scene.
  Usul::Interfaces::IBuildScene::QueryPtr build ( document );
  
  if ( true == build.valid() )
  {
    //Debugging...
    std::cout << "Build Valid... Removing Children..." << std::endl;
    //Remove the old triangle set from the group
    
    
    //Debugging...
    std::cout << "Children removed.  Setting options..." << std::endl;
    // enable shading-per-vertex options to pass to the TriangleSet
    OsgTools::Triangles::TriangleSet::Options opt;
    opt[ "normals" ] = "per-vertex";
    opt[ "colors" ]  = "per-vertex";

    //Debugging...
    std::cout << "Options set. Adding children..." << std::endl;

    // tell Triangle set to build its scene and assign the resulting node
    // to our internal node.
    
    group->addChild( build->buildScene( opt, _caller ) );
    

    // Disable Material Usage
    /*Usul::Interfaces::IMaterials::QueryPtr mat ( _document );
    if( true == mat.valid() )
    mat->useMaterial( false );*/

    // Turn off display lits.
    //Usul::Interfaces::IDisplaylists::QueryPtr dl ( _document );
    //dl->displayList( false );

    //OsgTools::DisplayLists dl( false );
    //dl( _terrain.get() );

    //Debugging...
    std::cout << "Children added.  Deleting options..." << std::endl;

    // Done with opt.
    {
      OsgTools::Triangles::TriangleSet::Options temp;
      std::swap( temp, opt );
    }

    //Debugging...
    std::cout << "Done with buildscene!" << std::endl;
    //std::cout << "\nNumber of triangles loaded: " << _triangles->numTriangles() << std::endl;

#if 0 // Show normals
  osg::ref_ptr < OsgTools::Utilities::FindNormals > visitor ( new OsgTools::Utilities::FindNormals );
  osg::BoundingBox bb;
  bb.expandBy( _terrain->getBound() );
  visitor->boundingBox ( bb );
  _terrain->accept( *visitor );
  _terrain->addChild ( visitor->normals() );
#endif

 
 
  }
#endif
  return group.release();
}




///////////////////////////////////////////////////////////////////////////////
//
//  Write a TDF file.
//
///////////////////////////////////////////////////////////////////////////////

bool DynamicLandDocument::writeTDF ( const std::string& filename, Usul::Interfaces::IUnknown *caller, Document* document )
{
  USUL_TRACE_SCOPE;
#if 1
  //Guard guard ( this->mutex() );
  // Ask the document write a TDF file.
  std::string slash ( Usul::File::slash() );
    #ifdef _MSC_VER
      std::string file ( filename );
    #else  
      std::string file ( _dir + slash + filename );
    #endif

  if( !Usul::Predicates::FileExists::test ( file + ".tdf" ) )
  {
    const std::string output = file + ".tdf";
    if ( 0x0 == document )
      return false;

    std::cout << "Saving file: " << output << " ... " << std::endl;
    document->saveAs ( output, caller );
    std::cout << "Done" << std::endl;
  }
  else
    std::cout << filename + ".tdf" << " already exists." << std::endl;
#endif
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  
  // If the wait time has expired...
  if ( false == _fileQueryDelay() )
    return;

  // Set the status bar text
  {
    std::ostringstream text;
    if( this->currentFilePosition() != this->currentFileLoaded() )
    {
      text << "  Year " << this->currentFilePosition() + 1 << " (Not Loaded)";
    }
    else
    {
      text << "  Year " << this->currentFilePosition() + 1;
    }
    this->_setStatusBar ( text.str(), caller );
  }

  if( true == _animationDelay() && true == this->_updateAnimationFrame() )
  {    
      this->incrementFilePosition();
      this->loadCurrentFile( true );
      this->_updateAnimationFrame( false );
   }
  
  if( true == this->isValid( this->currentFilePosition() ) &&
      false == this->isLoading( this->currentFilePosition() ) &&
      true == this->loadCurrentFile() )
  {
    // current time step is ready to be shown
    this->_loadNextTimeStep();
    this->_currentFileLoaded( this->currentFilePosition() );
  }
  
  // Search for files matching our criteria
  this->_findFiles();

  // Start jobs to load time steps in the background
  this->_loadJobs( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Launch Jobs to load time steps
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::_loadJobs( Usul::Interfaces::IUnknown *caller )
{
  if( this->numFiles() > 0 )
  {
    //Load files...
    int start = this->currentFilePosition() - this->_timeStepWindowSize;
    int end = this->currentFilePosition() + this->_timeStepWindowSize;
    //first job checked and loaded should always be the current position.
    if( false == this->isValid( this->currentFilePosition() ) &&
          false == this->isLoading( this->currentFilePosition() ) &&
          false == this->isLoaded( this->currentFilePosition() ) )
    {
      // Get the file name
      std::string filename = this->getFilenameAtIndex( this->currentFilePosition() );
      // strip the extension from the map file name
      std::string root = filename.substr( 0, filename.size() - 4 );
      this->isLoading( this->currentFilePosition(), true );
      #if 1 // with progress bars
        LoadDataJob::RefPtr job ( new LoadDataJob ( this, root, caller, this->currentFilePosition() ) );
      #else // without progress bars
        LoadDataJob::RefPtr job ( new LoadDataJob ( this, root, 0x0, this->currentFilePosition() ) );
      #endif
      this->_getJobManager()->add( job.get() );
      
    }
    for( int i = start; i <= end; ++i )
    {
      unsigned int index = 0;
      if( i < 0 )
      {
        index = this->numFiles() + i;
      }
      else
      {
        if( i > static_cast< int > ( this->numFiles() ) )
        {
	        index = 0 + i - this->numFiles(); 
        }
        else
        {
          index = static_cast< unsigned int > ( i );
	      }
      }
      if( false == this->isValid( index ) &&
          false == this->isLoading( index ) &&
          false == this->isLoaded( index ) )
      {
        // Get the file name
        std::string filename = this->getFilenameAtIndex( index );
        // strip the extension from the map file name
        std::string root = filename.substr( 0, filename.size() - 4 );
        this->isLoading( index, true );
        #if 1 // with progress bars
          LoadDataJob::RefPtr job ( new LoadDataJob ( this, root, caller, index ) );
        #else // without progress bars
          LoadDataJob::RefPtr job ( new LoadDataJob ( this, root, 0x0, index ) );
        #endif
        this->_getJobManager()->add( job.get() );
        //Usul::Jobs::Manager::instance().add ( job.get() );
      }
    }
  }
 
}



///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to set status bar and flush events.
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::_setStatusBar ( const std::string &text, Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IStatusBar::QueryPtr status ( caller );
  if ( status.valid() )
    status->setStatusBarText ( text, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Search for files matching the search criteria
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::_findFiles()
{
  // Get a list of files in the directory _dir, with the extension _ext
  std::string dir, ext, prefix;
  Files files;
  {
    Guard guard ( this->mutex() );
    dir = _dir;
    ext = _ext;
    prefix = _prefix;
  }
  // Get all the files in the directory "dir"
  //std::cout << "Checking " << dir << " for files with extension " << ext << std::endl;
  Usul::File::find( dir, ext, files );
  //std::cout << files.size() << " file(s) found" << std::endl;
  // If we don't find any files matching our search criteria ...
  if( 0 == files.size() )
  {
    this->clear();
    Guard guard ( this->mutex() );
    _numFilesInDirectory = 0;
  }
  

  // Make sure we have some files and that they are different from our last look
  // at the directory.
  
  if( files.size() != _numFilesInDirectory && files.size() > 0 )
  {
    // get the number of files in the directory
    {
      Guard guard ( this->mutex() );
      _numFilesInDirectory = files.size();
    }

    std::string slash ( Usul::File::slash() );
    #ifdef _MSC_VER
      std::string search ( dir + slash + prefix );
    #else
      std::string search ( prefix );
    #endif

    // Get all files with the prefix name.
    //std::cout << "Checking directory: " << dir + slash << " for files with prefix: " << prefix << std::endl;

    StringVector::iterator end ( std::remove_if 
                                 ( files.begin(), files.end(),  
                                 std::not1( StartsWith( search ) ) ) );

      
    // Remove the files that don't match our sort criteria
    files.erase ( end, files.end() );

    // Sort the names.
    std::sort ( files.begin(), files.end() );
    if ( false == files.empty() )
    {
      // set the file size to the current number of files that 
      // match our parameters from the directory search
      {
        Guard guard ( this->mutex() );
        if( _currFileNum > files.size() )
        {
          _currFileNum = files.size();
        }
      
       std::swap( files, _files );
      
       std::cout << "\nNumber of files loaded: " << _files.size() << std::endl;
       std::cout << "\rCurrently at file: " << _files.at( _currFileNum ) << std::flush;
      }
    }
    

  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load a terrain file.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Documents::Document *DynamicLandDocument::LoadDataJob::load (  const std::string& filename, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  #ifdef _MSC_VER
    std::string file ( filename );
  #else
    std::string slash ( Usul::File::slash() );
    std::string file ( _document->dir() + slash + filename );
  #endif
    
  if( Usul::Predicates::FileExists::test ( file + ".tdf" ) )
  {
    std::cout << "\nFound " << file + ".tdf" << std::endl;
    return this->_load( file + ".tdf", caller );
  }
  else if( Usul::Predicates::FileExists::test ( file + ".grs" ) ) 
  {
    std::cout << "\n" << file + ".tdf" << " not found." << std::endl;
    std::cout << "\nFound " << file + ".grs" << std::endl;
    return this->_load( file + ".grs", caller );
  }
  else if( Usul::Predicates::FileExists::test ( file + ".asc" ) )
  {
    std::cout << "\n" << file + ".tdf" << " not found." << std::endl;
    std::cout << "\n" << file + ".grs" << " not found." << std::endl;
    std::cout << "\nFound " << file + ".asc" << std::endl;
    return this->_load( file + ".asc", caller );
  }
  else
  {
    std::cout << "No suitable files found to load!" << std::endl;
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load a model
//
///////////////////////////////////////////////////////////////////////////////

Usul::Documents::Document * DynamicLandDocument::LoadDataJob::_load( const std::string &filename, Usul::Interfaces::IUnknown *caller ) 
{
  USUL_TRACE_SCOPE;


#if 0
#ifdef _DEBUG
  const std::string pluginName ( "TriangleModel.plugd" );
#else
  const std::string pluginName ( "TriangleModel.plug" );
#endif

  const Usul::CommandLine::Arguments::Args args ( Usul::CommandLine::Arguments::instance().args() );
  const std::string pluginPath ( Usul::File::directory ( args.at(0), true ) + pluginName );

  // Load the plugin
  Usul::Components::Manager::instance().load ( Usul::Interfaces::IPlugin::IID, pluginPath );
#endif
  // This will create a new document.
  Info info ( DocManager::instance().find ( filename, caller ) );

  // Check to see if we created a document.
  if ( false == info.document.valid() )
  {
    throw std::runtime_error ( "Error: Could not find document for file: " + filename );
  }

  // Disable Memory pools
  {
    Usul::Interfaces::IMemoryPool::QueryPtr pool ( info.document );
    pool->usePool ( false );
  }

  // Ask the document to open the file.
  this->_openDocument ( filename, info.document.get(), caller );

  // Reference count should go from 1->0 but not delete.
  return info.document.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::LoadDataJob::_openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this->mutex() );
  if ( 0x0 == document )
    return;

  std::cout << "Opening file: " << file << " ... " << std::endl;
  document->open ( file, caller );
  std::cout << "Done" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load an image file to use as a texture file to the model.
//  An actual texture object is not created.  Instead, we use
//  the color information contained in the image to set the 
//  corresponding color at a given point in the terrain file.
//
///////////////////////////////////////////////////////////////////////////////

bool DynamicLandDocument::LoadDataJob::_loadTexture ( Usul::Documents::Document *document, const std::string& filename, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if( 0x0 == document )
  {
    USUL_ASSERT ( false );
    return false;
  }
  #ifdef _MSC_VER
    std::string file ( filename );
  #else
    std::string slash ( Usul::File::slash() );
    std::string file ( _document->dir() + slash + filename );
  #endif
  if( Usul::Predicates::FileExists::test ( file ) )
  {
   
    // Get color array
    Usul::Interfaces::IColorsPerVertex::QueryPtr colorsV ( document );
    osg::ref_ptr< osg::Vec4Array > colors = colorsV->getColorsV ( true );

    // Get vertices
    Usul::Interfaces::IVertices::QueryPtr vertices ( document );
    osg::ref_ptr< osg::Vec3Array > v = vertices->vertices();

    // make sure the number of colors is correct.
    colors->resize ( v->size() );

    // load the image
    osg::ref_ptr< osg::Image > tex ( new osg::Image );
    tex = osgDB::readImageFile( file );
    Usul::Math::Vec2f ll;
    Usul::Math::Vec2ui gridSize;
    float cellSize;
    std::cout << "Reading texture image file: " << file << std::endl;
    {
      Guard guard ( this->mutex() );
      ll = _header.ll;
      gridSize = _header.gridSize;
      cellSize = _header.cellSize;
    }
    for( unsigned int i = 0; i < v->size(); ++i )
    {
      // get the current grid location of the point at index i
      float vi = v->at(i)[0] - ll[1];
      float vj = v->at(i)[1] - ll[0];
      float rsize = ( float ( gridSize[0] ) * cellSize ) - 1;
      float csize = ( float ( gridSize[1] ) * cellSize ) - 1;
      
      // get the color information from the image for the point at index i
      unsigned int pixelRow = static_cast < unsigned int >( ( rsize - vi ) / cellSize );
      unsigned int pixelCol = static_cast < unsigned int >( vj / cellSize );
      unsigned char * pixel = tex->data(  pixelCol, pixelRow  );

      // get the color values to use
      osg::Vec4f pixelColor ( 0, 0, 0, 1 );
      pixelColor[0] = static_cast< float > ( pixel[0] ) / 255.0f ;
      pixelColor[1] = static_cast< float > ( pixel[1] ) / 255.0f ;
      pixelColor[2] = static_cast< float > ( pixel[2] ) / 255.0f ;
  
      // set the color for the point at index i
      colors->at(i) = pixelColor;
    }

    // The colors are not dirty.
    colorsV->dirtyColorsV ( false );

    std::cout << "\nTexture file reading complete!" << std::endl;

    return true;
    }
  else
  {
    std::cout << "Unable to load image file:" << file << ".  File does not exist!" << std::endl;
    return false;
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Parse.
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::_parseHeader( XmlTree::Node &node, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  typedef XmlTree::Document::Attributes Attributes;
  Attributes& attributes ( node.attributes() );

  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "ncols" == iter->first )
      {
        Usul::Strings::fromString ( iter->second, _header.gridSize[1] );
      }
      if ( "nrows" == iter->first )
      {
        Usul::Strings::fromString ( iter->second, _header.gridSize[0] );
      }
      if ( "xllcorner" == iter->first )
      {
        Usul::Strings::fromString ( iter->second, _header.ll[0] );
      }
      if ( "yllcorner" == iter->first )
      {
        Usul::Strings::fromString ( iter->second, _header.ll[1] );
      }
      if ( "cellsize" == iter->first )
      {
        Usul::Strings::fromString ( iter->second, _header.cellSize);
      }
      if ( "nodata" == iter->first )
      {
        Usul::Strings::fromString ( iter->second, _header.noDataValue );
      }
      if ( "windowsize" == iter->first )
      {
        Usul::Strings::fromString ( iter->second, this->_timeStepWindowSize ); 
      }
      if ( "numsteps" == iter->first )
      {
        
        Usul::Strings::fromString ( iter->second, this->_numTimeSteps );
        _timeStepPool.resize( this->_numTimeSteps );
        for( unsigned int i = 0; i < _numTimeSteps; i ++ )
        {
          TimeStep temp;
          temp.isValid = false;
          temp.isLoading = false;
          temp.isLoaded = false;
          temp.group = new osg::Group();

          _timeStepPool.at( i ) = temp;
        }

      }
  }
}


bool DynamicLandDocument::_readParameterFile( XmlTree::Node &node, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;

  Attributes& attributes ( node.attributes() );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "directory" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _dir );
    }
    if ( "prefix" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _prefix );
    }
    if ( "extension" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _ext );
    }
  }

  Children& children ( node.children() );
	
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "header" == node->name() )
    {
      this->_parseHeader( *node, caller );
    }
    
    
  }
  
  return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Dynamic Land Model Job Constructor..
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::_loadCurrentFileFromDocument( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  std::cout << "Building Scene..." << std::endl;
  //_currentDocument->clear();
  _currentDocument = _newDocument;
  //_newDocument->clear();
  _newDocument = 0x0;
  Usul::Interfaces::IBuildScene::QueryPtr build ( _currentDocument );
  if ( true == build.valid() )
  {
    OsgTools::Group::removeAllChildren ( _terrain.get() );
    OsgTools::Triangles::TriangleSet::Options opt;
    opt[ "normals" ] = "per-vertex";
    opt[ "colors" ]  = "per-vertex";
    _terrain->addChild ( build->buildScene ( opt, caller ) );
  }
  else
  {
    std::cout << "Build Invalid!" << std::endl;
  }
  std::cout << "Scene Built!" << std::endl;
  if( true == this->isAnimating() )
    this->_updateAnimationFrame( true );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the next time step
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::_loadNextTimeStep()
{

  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  try
  {
    _terrain->removeChild( 0, _terrain->getNumChildren() );
    osg::ref_ptr< osg::Group > group ( new osg::Group );
    group = _timeStepPool.at( this->currentFilePosition() ).group;
    _terrain->addChild( group.get() );
    _timeStepPool.at( this->currentFilePosition() ).isLoading = false;
    _timeStepPool.at( this->currentFilePosition() ).isValid = true;
    this->loadCurrentFile ( false );
    this->isLoaded( this->currentFilePosition(), true );
    if( true == this->isAnimating() )
      this->_updateAnimationFrame( true );
  }
  catch ( const std::exception &e )
  {
    std::cout << "Error 1999395957: Application::preFrame(): exception: " + std::string ( e.what() ) << std::endl;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Load the previous time step  
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::_loadPrevTimeStep()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data.
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::_currentFileLoaded (int index)
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _currFileLoaded = index;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data.
//
///////////////////////////////////////////////////////////////////////////////

int DynamicLandDocument::currentFileLoaded ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return this->_currFileLoaded;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int DynamicLandDocument::currentFilePosition ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return this->_currFileNum;
}


///////////////////////////////////////////////////////////////////////////////
//
// start/stop animation
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::animate( bool a )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _isAnimating = a;
  this->_updateAnimationFrame( a );
}


///////////////////////////////////////////////////////////////////////////////
//
// Got to time step <pos>
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::setCurrentFilePosition( unsigned int pos )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _currFileNum = pos;
}


///////////////////////////////////////////////////////////////////////////////
//
// Got to time step <pos>
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::newFrameLoaded( bool l )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _updateAnimation = l;
}


///////////////////////////////////////////////////////////////////////////////
//
// Is TimeStep in position pos currently loading
//
///////////////////////////////////////////////////////////////////////////////

bool DynamicLandDocument::isLoading( unsigned int pos )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return this->_timeStepPool.at( pos ).isLoading;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set TimeStep in position pos to value of loading
//
///////////////////////////////////////////////////////////////////////////////


void DynamicLandDocument::isLoading( unsigned int pos, bool loading )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  this->_timeStepPool.at( pos ).isLoading = loading;
}


///////////////////////////////////////////////////////////////////////////////
//
// Is current TimeStep valid
//
///////////////////////////////////////////////////////////////////////////////

bool DynamicLandDocument::isValid( unsigned int pos )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  //std::cout << "Checking if position: " << pos << " is valid." << std::endl;
  if( pos < _timeStepPool.size() )
    return this->_timeStepPool.at( pos ).isValid;

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set TimeStep in position pos to value of loading
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::isValid( unsigned int pos, bool valid )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  this->_timeStepPool.at( pos ).isValid = valid;
}



///////////////////////////////////////////////////////////////////////////////
//
// Check to see if we are supposed to load the current file
//
///////////////////////////////////////////////////////////////////////////////

bool DynamicLandDocument::loadCurrentFile()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return this->_loadNewMap;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the number of files with the matching prefix
//
///////////////////////////////////////////////////////////////////////////////

unsigned int DynamicLandDocument::numFiles()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  //std::cout << "Number of files is: " << _files.size() << std::endl;
  return _files.size();
}

///////////////////////////////////////////////////////////////////////////////
//
// Return the filename at <index>
//
///////////////////////////////////////////////////////////////////////////////

std::string DynamicLandDocument::getFilenameAtIndex( unsigned int index )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  //this->numFiles();
  //std::cout << "Found filename: " << this->_files.at( index ) << " at index: " << index << std::endl;
  return this->_files.at( index );
}


///////////////////////////////////////////////////////////////////////////////
//
// Is the time step at pos loaded (is it the current time step)
//
///////////////////////////////////////////////////////////////////////////////

bool DynamicLandDocument::isLoaded( unsigned int pos )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return this->_timeStepPool.at( pos ).isLoaded;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the status of the time step at position pos as the current time step
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::isLoaded( unsigned int pos, bool loaded )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  this->_timeStepPool.at( pos ).isLoaded = loaded;
}


///////////////////////////////////////////////////////////////////////////////
//
// Setup the current animation frame
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::setTimeStepFrame( unsigned int i, osg::Group * group )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _timeStepPool.at( i ).isValid = true;
  _timeStepPool.at( i ).isLoading = false;
  _timeStepPool.at( i ).group = group;
  //_timeStepPool.at( i ).group = dynamic_cast<osg::Group *> ( group->clone ( osg::CopyOp::DEEP_COPY_ALL ) );
}

///////////////////////////////////////////////////////////////////////////////
//
// Check to see if we need to update the animation frame
//
///////////////////////////////////////////////////////////////////////////////

bool DynamicLandDocument::_updateAnimationFrame()
{
  USUL_TRACE_SCOPE;
  return _updateAnimation;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set to update the animation frame
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::_updateAnimationFrame( bool u)
{
  USUL_TRACE_SCOPE;
  _updateAnimation = u;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dynamic Land Model Job Constructor..
//
///////////////////////////////////////////////////////////////////////////////

DynamicLandDocument::LoadDataJob::LoadDataJob ( DynamicLandDocument* document, const std::string& filename, Usul::Interfaces::IUnknown *caller, unsigned int i ) :
  BaseClass ( caller ),
  _document ( document ),
  _filename ( filename ),
  _caller ( caller ),
  _index ( i )
{
  USUL_TRACE_SCOPE;
  _header = document->getHeaderInfo();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data.
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::LoadDataJob::_started ()
{
  USUL_TRACE_SCOPE;

  Usul::Interfaces::IProgressBar::ShowHide showHide( this->progress() );
  // Set the label.
  this->_setLabel ( "Loading file: " + _filename );


 
// Process all the requests. 


  Usul::Documents::Document::RefPtr document = this->load( _filename, this->progress() );

  // if the map file is valid, load the image file for coloring
  if ( true == document.valid() )
  {
    // load the image file
    this->_loadTexture( document.get(), _filename + ".png", this->progress() );
    
    osg::ref_ptr< osg::Group > group ( new osg::Group );
    group->addChild( this->_buildScene( document.get() ) );
    _document->setTimeStepFrame( _index, group.get() );

    // write a tdf of the loaded terrain for faster loading on revisit
    //_document->writeTDF ( _filename, this->progress(), document.get() );

#if 0
    Usul::Interfaces::IBuildScene::QueryPtr build ( _document );
    if ( true == build.valid() )
    {
      OsgTools::Group::removeAllChildren ( _terrain.get() );
      OsgTools::Triangles::TriangleSet::Options opt;
      opt[ "normals" ] = "per-vertex";
      opt[ "colors" ]  = "per-vertex";
      _document->_timeStepPool.at ( _document->currentFilePosition() )->addChild ( build->buildScene ( opt, caller ) );
    }
    
#endif

  }
}


DynamicLandDocument::CommandList DynamicLandDocument::getCommandList()
{
  USUL_TRACE_SCOPE;
  CommandList cl;

  Usul::Interfaces::IUnknown::QueryPtr me ( this );
  std::cout << me.get () << std::endl;

  //cl.push_back( new LoadNextTimestep( me.get() ) );
  cl.push_back( new NextTimestep( me.get() ) );
  cl.push_back( new PrevTimestep( me.get() ) );
  cl.push_back( new LoadTimestep( me.get() ) );
  cl.push_back( new FirstTimestep( me.get() ) );
  cl.push_back( new StartAnimation( me.get() ) );
  cl.push_back( new StopAnimation( me.get() ) );
  return cl;

}

///////////////////////////////////////////////////////////////////////////////
//
// Clear out the time step at <index>
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::removeTimeStepAtIndex( unsigned int index )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  this->_timeStepPool.at( index ).isLoaded = false;
  this->_timeStepPool.at( index ).isLoading = false;
  this->_timeStepPool.at( index ).isValid = false;
  this->_timeStepPool.at( index ).group = new osg::Group();
}


///////////////////////////////////////////////////////////////////////////////
//
// Clear out the time step at <index>
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::KillJob::removeTimeStep()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  std::cout << "Calling removeTimeStep() on index: " << _index << std::endl;
  _document->removeTimeStepAtIndex( _index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dynamic Land Model Job Constructor..
//
///////////////////////////////////////////////////////////////////////////////

DynamicLandDocument::KillJob::KillJob ( DynamicLandDocument* document, Usul::Interfaces::IUnknown *caller, unsigned int i ) :
  BaseClass ( caller ),
  _document ( document ),
  _index ( i )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the job.
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::KillJob::_started ()
{
  USUL_TRACE_SCOPE;
  while( true == _document->isLoading( _index ) )
  {
  }
  this->removeTimeStep();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the job.
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::_setThreadPoolSize( unsigned int size )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  //Usul::Jobs::Manager::instance().poolResize ( 3 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the directory where the files are
//
///////////////////////////////////////////////////////////////////////////////

std::string DynamicLandDocument::dir()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the job manager.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Jobs::Manager *DynamicLandDocument::_getJobManager()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( 0x0 == _jobManager.get() )
  {
    _jobManager = JobManagerPtr ( new Usul::Jobs::Manager ( 1 ) );
  }
  return _jobManager.get();
}
