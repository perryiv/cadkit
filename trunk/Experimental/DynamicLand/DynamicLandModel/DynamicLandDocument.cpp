
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "DynamicLandDocument.h"

#include "Usul/Interfaces/IColorsPerVertex.h"
#include "Usul/Interfaces/IDisplaylists.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Interfaces/IMaterials.h"
#include "Usul/Interfaces/IMemoryPool.h"
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

#include "osgDB/ReadFile"
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
    std::cout << "Comparing " << s << " with " << _s << std::endl;
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
  _cellSize ( 0 ),
  _noDataValue ( 0 ),
  _gridSize ( 0, 0 ),
  _ll ( 0, 0 ),
  _document ( 0x0 ),
  _terrain ( new osg::Group ),
  _tb ( 1000 ),
  _currFileNum ( 0 ),
  _numFilesInDirectory ( 0 ), 
  _files( 0x0 ),
  _loadNewMap ( false ),
  _dir( "" ),
  _ext ( "" ),
  _prefix ( "" )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

DynamicLandDocument::~DynamicLandDocument()
{
  USUL_TRACE_SCOPE;
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
  default:
    return BaseClass::queryInterface ( iid );
  }
}




///////////////////////////////////////////////////////////////////////////////
//
//  Cycle up our list of files matching our search criteria
//
///////////////////////////////////////////////////////////////////////////////

  void DynamicLandDocument::incrementFilePosition ()
  {
    if( _currFileNum < _files.size() - 1 && _files.size() > 0 )
    {
      ++_currFileNum;
      std::cout << "\rCurrently at file: " << _files.at( _currFileNum ) << std::flush;
    }
  }


///////////////////////////////////////////////////////////////////////////////
//
//  Cycle down our list of files matching our search criteria
//
///////////////////////////////////////////////////////////////////////////////

  void DynamicLandDocument::decrementFilePosition ()
  {
    if( _currFileNum > 0 && _files.size() > 0 )
    {
      --_currFileNum;
      std::cout << "\rCurrently at file: " << _files.at( _currFileNum ) << std::flush;
    }
  }


///////////////////////////////////////////////////////////////////////////////
//
//  Load the current file.
//
///////////////////////////////////////////////////////////////////////////////

  void DynamicLandDocument::loadCurrentFile( bool loadFile )
  {
    if ( _files.size() > 0 )
    {
      _loadNewMap = loadFile;
      if( loadFile )
        std::cout << "\nLoading file: " << _files.at( _currFileNum ) << "..." << std::endl;
    }
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
    this->_buildScene();

    return _terrain.get();
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

void DynamicLandDocument::_buildScene()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Disable materials in the TriangleSet


   // Get interface to build scene.
  Usul::Interfaces::IBuildScene::QueryPtr build ( _document );
  
  if ( true == build.valid() )
  {
 
    //Remove the old triangle set from the group
    _terrain->removeChild( 0, _terrain->getNumChildren() );

    // enable shading-per-vertex options to pass to the TriangleSet
    OsgTools::Triangles::TriangleSet::Options opt;
    opt[ "normals" ] = "per-vertex";
    opt[ "colors" ]  = "per-vertex";

    

    

    // tell Triangle set to build its scene and assign the resulting node
    // to our internal node.
    _terrain->addChild( build->buildScene( opt, 0x0 ) );

    // Disable Material Usage
    /*Usul::Interfaces::IMaterials::QueryPtr mat ( _document );
    if( true == mat.valid() )
    mat->useMaterial( false );*/

    // Turn off display lits.
     //Usul::Interfaces::IDisplaylists::QueryPtr dl ( _document );
    //dl->displayList( false );

    // Done with opt.
    {
      OsgTools::Triangles::TriangleSet::Options temp;
      std::swap( temp, opt );
    }

    
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
  
}




///////////////////////////////////////////////////////////////////////////////
//
//  Write a TDF file.
//
///////////////////////////////////////////////////////////////////////////////

bool DynamicLandDocument::writeTDF ( const std::string& filename, Usul::Interfaces::IUnknown *caller )
{
  // Ask the document write a TDF file.
  if( !Usul::Predicates::FileExists::test ( filename + ".tdf" ) )
  {
    const std::string output = filename + ".tdf";
    if ( 0x0 == _document )
      return false;

    std::cout << "Saving file: " << output << " ... " << std::endl;
    _document->saveAs ( output, 0x0 );
    std::cout << "Done" << std::endl;
  }
  else
    std::cout << filename + ".tdf" << " already exists." << std::endl;

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
    if( _tb() )
    {
        
      // Get a list of files in the directory _dir, with the extension _ext
      Files files;
      Usul::File::find( _dir, _ext, files );

      // If we don't find any files matching our search criteria ...
      if( 0 == files.size() )
      {
        this->clear();
        _numFilesInDirectory = 0;
      }

      // Debugging -- output the files found in the given directory
      //std::copy ( files.begin(), files.end(), std::ostream_iterator<std::string> ( std::cout, "; " ) );
      //std::cout << std::endl;

      // Make sure we have some files and that they are different from our last look
      // at the directory.
      if( files.size() != _numFilesInDirectory && files.size() > 0 )
      {
        // get the number of files in the directory
        _numFilesInDirectory = files.size();
        
	      std::string slash ( Usul::File::slash() );

	      #ifdef _MSC_VER
	        std::string search ( _dir + slash + _prefix );
	      #else
	        std::string search ( _prefix );
	      #endif

        // Get all files with the prefix name.
	      std::cout << "Checking directory: " << _dir + slash << " for files with prefix: " << _prefix << std::endl;

        StringVector::iterator end ( std::remove_if 
                                     ( files.begin(), files.end(),  
                                     std::not1( StartsWith( search ) ) ) );

          
        // Remove the files that don't match our sort criteria
        files.erase ( end, files.end() );

        // Sort the names.
        std::sort ( files.begin(), files.end() );
	      if ( false == files.empty() )
	      {
          if( _currFileNum > files.size() )
            _currFileNum = files.size();
                
           std::swap( files, _files );
                
           std::cout << "\nNumber of files loaded: " << _files.size() << std::endl;
           std::cout << "\rCurrently at file: " << _files.at( _currFileNum ) << std::flush;
	      }
	      else
	      {
	        std::cout << "\nNo Files found matching search criteria" << std::endl;
	      }

      }
   
      if( _loadNewMap && _files.size() > 0 )
      {
        // Do not load a anther map
        this->loadCurrentFile( false );

        // Get the file name
        std::string filename = _files.at( _currFileNum );

        // strip the extension from the map file name
        std::string root = filename.substr( 0, filename.size() - 4 );
            
        // load the map file
        LoadDataJob::RefPtr job ( new LoadDataJob ( this, root, caller ) );
        Usul::Jobs::Manager::instance().add ( job.get() );  
      } 
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load a terrain file.
//
///////////////////////////////////////////////////////////////////////////////

bool DynamicLandDocument::load ( const std::string& filename, Usul::Interfaces::IUnknown *caller )
{
  #ifdef _MSC_VER
    std::string file ( filename );
  #else
    std::string slash ( Usul::File::slash() );
    std::string file ( _dir + slash + filename );
  #endif
  if( Usul::Predicates::FileExists::test ( file + ".tdf" ) )
  {
    std::cout << "\nFound " << file + ".tdf" << std::endl;
    this->_load( file + ".tdf", caller );
  }
  else
  {
    std::cout << "\n" << file + ".tdf" << " not found." << std::endl;
    this->_load( file + ".asc", caller );
  }

  return true;
}


bool DynamicLandDocument::_load( const std::string& filename, Usul::Interfaces::IUnknown *caller )
{
#ifdef _DEBUG
  const std::string pluginName ( "TriangleModel.plugd" );
#else
  const std::string pluginName ( "TriangleModel.plug" );
#endif

  const Usul::CommandLine::Arguments::Args args ( Usul::CommandLine::Arguments::instance().args() );
  const std::string pluginPath ( Usul::File::directory ( args.at(0), true ) + pluginName );

  // Load the plugin
  Usul::Components::Manager::instance().load ( Usul::Interfaces::IPlugin::IID, pluginPath );

  // This will create a new document.
  Info info ( DocManager::instance().find ( filename, caller ) );
  Document::RefPtr document ( info.document );

  // Check to see if we created a document.
  if ( false == document.valid() )
  {
    throw std::runtime_error ( "Error: Could not find document for file: " + filename );
  }

  // Disable Memory pools
  Usul::Interfaces::IMemoryPool::QueryPtr pool ( document );
  pool->usePool( false );

  // Ask the document to open the file.
  this->_openDocument ( filename, document.get(), caller );

  _document = document;

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::_openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller )
{
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

bool DynamicLandDocument::_loadTexture ( const std::string& filename, Usul::Interfaces::IUnknown *caller )
{
  if( Usul::Predicates::FileExists::test ( filename ) )
  {
    // Get color array
    Usul::Interfaces::IColorsPerVertex::QueryPtr colorsV ( _document );
    osg::ref_ptr< osg::Vec4Array > colors = colorsV->getColorsV ( true );

    // Get vertices
    Usul::Interfaces::IVertices::QueryPtr vertices ( _document );
    osg::ref_ptr< osg::Vec3Array > v = vertices->vertices();

    // make sure the number of colors is correct.
    colors->resize ( v->size() );

    // load the image
    osg::ref_ptr< osg::Image > tex ( new osg::Image );
    tex = osgDB::readImageFile( filename );
    
    std::cout << "Reading texture image file: " << filename << std::endl;

    for( unsigned int i = 0; i < v->size(); ++i )
    {
      // get the current grid location of the point at index i
      float vi = v->at(i)[0] - _ll[1];
      float vj = v->at(i)[1] - _ll[0];
      float rsize = ( float ( _gridSize[0] ) * _cellSize ) - 1;
      float csize = ( float ( _gridSize[1] ) * _cellSize ) - 1;
      
      // get the color information from the image for the point at index i
      unsigned int pixelRow = static_cast < unsigned int >( ( rsize - vi ) / _cellSize );
      unsigned int pixelCol = static_cast < unsigned int >( vj / _cellSize );
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
    std::cout << "Unable to load image file:" << filename << ".  File does not exist!" << std::endl;
    return false;
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Parse a channel.
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::_parseHeader( XmlTree::Node &node, Unknown *caller )
{
  typedef XmlTree::Document::Attributes Attributes;
  Attributes& attributes ( node.attributes() );

  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "ncols" == iter->first )
      {
        Usul::Strings::fromString ( iter->second, _gridSize[1] );
      }
      if ( "nrows" == iter->first )
      {
        Usul::Strings::fromString ( iter->second, _gridSize[0] );
      }
      if ( "xllcorner" == iter->first )
      {
        Usul::Strings::fromString ( iter->second, _ll[0] );
      }
      if ( "yllcorner" == iter->first )
      {
        Usul::Strings::fromString ( iter->second, _ll[1] );
      }
      if ( "cellsize" == iter->first )
      {
        Usul::Strings::fromString ( iter->second, _cellSize);
      }
      if ( "nodata" == iter->first )
      {
        Usul::Strings::fromString ( iter->second, _noDataValue );
      }
  }
}


bool DynamicLandDocument::_readParameterFile( XmlTree::Node &node, Unknown *caller )
{
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

DynamicLandDocument::LoadDataJob::LoadDataJob ( DynamicLandDocument* document, const std::string& filename, Usul::Interfaces::IUnknown *caller ) :
  BaseClass (),
  _document ( document ),
  _filename ( filename ),
  _caller ( caller )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data.
//
///////////////////////////////////////////////////////////////////////////////

void DynamicLandDocument::LoadDataJob::_started ()
{
  USUL_TRACE_SCOPE;
  Usul::Interfaces::IProgressBar::ShowHide showHide  ( this->progress()   );
  
  // Set the label.
  this->_setLabel ( "Loading file: " + _filename );
  

  // Process all the requests.
  bool valid = _document->load( _filename, _caller.get() );

        // if the map file is valid, load the image file for coloring
  if ( valid )
  {
    // load the image file
    _document->_loadTexture( _filename + ".png", _caller.get() );

    // write a tdf of the loaded terrain for faster loading on revisit
    //this->writeTDF ( root );

    // build our scene
    _document->_buildScene();
  }

  
  
}

