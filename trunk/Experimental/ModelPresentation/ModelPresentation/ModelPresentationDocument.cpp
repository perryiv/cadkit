
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/ModelPresentation/ModelPresentation/ModelPresentationDocument.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdMenuCommand.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdPrevTimestep.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdAnimation.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdFirstTimestep.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdNextCommand.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdLocation.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdTools.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdTimelineModel.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdDynamicModel.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdNextSequence.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdPrevSequence.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdFirstSequence.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdAnimationSpeed.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdSlowerAnimationSpeed.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdFasterAnimationSpeed.h"

#include "Usul/Interfaces/IDisplaylists.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Predicates/FileExists.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IMemoryPool.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/File/Slash.h"
#include "Usul/Print/Matrix.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Functions.h"
#include "Usul/Math/MinMax.h"
#include "Usul/System/Directory.h"
#include "Usul/System/Host.h"
#include "Usul/Adaptors/Bind.h"
#include "Usul/Convert/Convert.h"

#include "OsgTools/DisplayLists.h"
#include "OsgTools/Group.h"
#include "OsgTools/Convert.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"

#include "osgDB/ReadFile"
#include "osg/Group"
#include "osg/Switch"
#include "osg/MatrixTransform"
#include "osg/Depth"
#include "osgText/Font"
#include "osgText/Text"
#include "osgUtil/GLObjectsVisitor"


#include <iterator>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iomanip>


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ModelPresentationDocument, ModelPresentationDocument::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationDocument::ModelPresentationDocument() : 
  BaseClass ( "Model Presentation Document" ),
  _root ( new osg::Group ),
  _sets ( 0x0 ),
  _update( UpdatePolicyPtr( new UpdatePolicy( 10 ) ) ),
  _checkFileSystem( UpdatePolicyPtr( new UpdatePolicy( 10 ) ) ),
  _useTimeLine( false ),
  _useModels( false ),
  _useDynamic( false ),
  _useSequence( false ),
  _isAnimating( false ),
  _showTools ( false ),
  _userSpecifiedEndTime( false ),
  _checkTimeStatus( false ),
  _globalTimelineEnd( 0 ),
  _globalCurrentTime ( 0 ),
  _textXPos( 0 ),
  _textYPos( 0 ),
  _dynamicNotLoadedTextXPos( 0 ),
  _dynamicNotLoadedTextYPos( 0 ),
  _camera( new osg::Camera ),
  _animationSpeed( 10 )
{
  USUL_TRACE_SCOPE;
  _mpdModels.models = new osg::Switch();
  _writer = new ModelPresentationLib();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationDocument::~ModelPresentationDocument()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::clear ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ModelPresentationDocument::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener* > ( this );
  case Usul::Interfaces::IMpdNavigator::IID:
    return static_cast < Usul::Interfaces::IMpdNavigator* > ( this ); 
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd * > ( this );
  case Usul::Interfaces::IMpdWriter::IID:
    return static_cast < Usul::Interfaces::IMpdWriter * > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::canExport ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::canInsert ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::canOpen ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "mpd" || ext == "drtmpd" );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::canSave ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "mpd" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

 _writer->write( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  _workingDir = Usul::File::directory( name, true );

  //_writer = new ModelPresentationLib( _workingDir + "/output.mpd" );

  
  document->load ( name );
  if ( "mpd" == document->name() )
  {
    
    this->_readParameterFile( *document, caller, progress );
    this->updateGlobalEndtime();
  }
  else
  {
    std::string ext = Usul::File::extension( name );
    std::string dir = Usul::File::directory( name, true );
    std::string base = Usul::File::base( name );
    if ( "drtmpd" == ext )
    {
      ModelPresentationLib::RefPtr drtReader ( new ModelPresentationLib() );
      drtReader->read( name, "drt", caller, progress );
      XmlTree::Document::RefPtr doc ( new XmlTree::Document );
      _workingDir = dir;

      doc->load ( dir + base + ".mpd" );
      this->_readParameterFile( *doc, caller, progress );
      this->updateGlobalEndtime();
      
    }
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationDocument::Filters ModelPresentationDocument::filtersSave() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "Model Presentation (*.mpd)", "*.mpd" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationDocument::Filters ModelPresentationDocument::filtersExport() const
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

ModelPresentationDocument::Filters ModelPresentationDocument::filtersOpen() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "Model Presentation (*.mpd)", "*.mpd" ) );
  filters.push_back ( Filter ( "Disaster Recovery Simulation (*.drtmpd)", "*.drtmpd" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationDocument::Filters ModelPresentationDocument::filtersInsert() const
{
  USUL_TRACE_SCOPE;
  return Filters();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ModelPresentationDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _root->removeChild( 0, _root->getNumChildren() );

  // Add the models
  _root->addChild( _mpdModels.models.get() );

  // start the timeline at the first step
  if( true == _useTimeLine )
  {
    this->firstStep();
  }
  // show groups that should be shown at start up
  for( unsigned int i = 0; i < _sets.size(); ++i )
  {
    this->setGroup( i, 0 );
  }
  // Add dynamic sets to the scene tree
  for( unsigned int i = 0; i < _dynamicSets.size(); ++i )
  {
    _root->addChild( _dynamicSets.at( i ).models.get() );
  }

  std::cout << "Building Display Lists..." << std::endl;
  osg::ref_ptr< osgUtil::GLObjectsVisitor > visitor( new osgUtil::GLObjectsVisitor( osgUtil::GLObjectsVisitor::COMPILE_DISPLAY_LISTS ) );
  //visitor->traverse( _root );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the time steps to see if anything needs to he hidden/shown.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_checkTimeSteps( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Set the status bar to reflect the current time
  std::string message = Usul::Strings::format( "Current Step: ", _globalCurrentTime + 1 );
  this->_setStatusText( message, _textXPos, _textYPos, 0.80, 0.05, caller );


  // Check Time Sets
  if( true == _useTimeLine )
  {
   // Set all models to hide
    for( unsigned int modelIndex = 0; modelIndex < _mpdModels.models->getNumChildren(); ++modelIndex )
    { 
      _mpdModels.models->setValue( modelIndex, false );
    }
    for( unsigned int j = 0; j < _timeSets.size(); ++j )
    {
      unsigned int currentIndex = _timeSets.at( j ).currentTime;

      if( true == _timeSets.at( j ).visible )
      {
        for( unsigned int t = 0; t < _timeSets.at( j ).groups.size(); ++t )
        {
          if( _timeSets.at( j ).groups.at( t ).startTime <= currentIndex &&
              _timeSets.at( j ).groups.at( t ).endTime > currentIndex )
          {
            for( std::map< std::string, bool >::iterator iter =  _timeSets.at( j ).groups.at( t ).visibleModelsMap.begin();
                                                         iter != _timeSets.at( j ).groups.at( t ).visibleModelsMap.end();
                                                         ++iter )
            {
              std::string modelName = (*iter).first;
              bool value            = (*iter).second;
              unsigned int mIndex   = _mpdModels.modelMap[modelName];
              _mpdModels.models->setValue( mIndex, value );
            }   
          }
        }
      }
    }
    
  }

  // Check dynamic sets
  if( true == _useDynamic )
  {

    for( unsigned int i = 0; i < _dynamicSets.size(); ++i )
    {   
      if( true == _dynamicSets.at( i ).visible )
      {
        //Set the next dynamic set to be show
        if( _dynamicSets.at( i ).currentTime >= 0 && _dynamicSets.at( i ).currentTime < _dynamicSets.at( i ).endTime )
        {
          // Display a message if the current dynamic set isn't loaded
          if( _dynamicSets.at( i ).groups.at( _dynamicSets.at( i ).currentTime ).loaded == false )
          {
            std::string message = Usul::Strings::format( "Step ", _dynamicSets.at( i ).currentTime + 1, " of ",_dynamicSets.at( i ).maxFilesToLoad, " is not loaded..." );
            this->_setStatusText( message, _dynamicNotLoadedTextXPos, _dynamicNotLoadedTextYPos, 0.5, 0.5, caller );
          }
          else
          {
            std::string message ( "" );
            this->_setStatusText( message, _dynamicNotLoadedTextXPos, _dynamicNotLoadedTextYPos, 0.5, 0.5, caller );
          }
          // If the model at current position is valid then set the visibility mask
          if( _dynamicSets.at( i ).groups.size() > 0 )
          {
            //if( true == _dynamicSets.at( i ).groups.at( _dynamicSets.at( i ).currentTime ).valid )
            {      
              if( _dynamicSets.at( i ).currentTime == 0 )
              {
                _dynamicSets.at( i ).models->setValue( _dynamicSets.at( i ).endTime - 1, false );
                _dynamicSets.at( i ).models->setValue( _dynamicSets.at( i ).currentTime, true );
              }
              else if( _dynamicSets.at( i ).currentTime == _dynamicSets.at( i ).endTime - 1 )
              {
                _dynamicSets.at( i ).models->setValue( _dynamicSets.at( i ).endTime - 2, false );
                _dynamicSets.at( i ).models->setValue( _dynamicSets.at( i ).endTime - 1, true );
              }
              else
              {
                _dynamicSets.at( i ).models->setValue( _dynamicSets.at( i ).currentTime - 1, false );
                _dynamicSets.at( i ).models->setValue( _dynamicSets.at( i ).currentTime, true );
              }       
            }
          }
        }
        else
        {
          for( unsigned int index = 0; index < _dynamicSets.at( i ).models->getNumChildren(); ++index )
          {
            if( true == _dynamicSets.at( i ).models->getValue( index ) )
            {
              _dynamicSets.at( i ).models->setValue( index, false );
            }
          }
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer just rendered.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::postRenderNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

 
  if( true == this->isAnimating() )
  {
    this->requestRedraw();
  }
   

  BaseClass::postRenderNotify ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // If we have a dynamic structure loaded check the file system for new files
  if( true == this->_dynamic() && (*_checkFileSystem)() )
  {
    unsigned int numJobs = 0;
    {
      Guard guard ( this );
      numJobs = _jobs.size();
    }

    //std::cout << "\rChecking file system." << std::flush;
    for( unsigned int index = 0; index < numJobs; ++index )
    {
      MpdDefinitions::MpdDynamicSet set;
      {
        Guard guard ( this );
        set = _dynamicSets.at( index );
      }
    
      // make sure we haven't loaded all our alloted steps
      if( set.nextIndexToLoad < set.maxFilesToLoad )
      {
        
        if( false == this->_getJobAtIndex( index ).valid() )
        {
          //std::cout << "Starting job to look at file system" << std::endl;
          Guard guard ( this );
          _jobs.at( index ) = new MpdJob( caller, _workingDir, set.header.directory, set.header.prefix, set.header.extension, set.header.modelNames );
          Usul::Jobs::Manager::instance().addJob ( _jobs.at( index ).get() );
        }
        else
        {
          if ( true == this->_getJobAtIndex( index )->isDone() )
          {
            //std::cout << "\nJob is finished.  Getting data..." << std::flush;
            // process the data from the completed job
            this->_processJobData( index, caller );
            
          }
          else
          {
            //if( false == this->_getJobAtIndex( index ).valid() )
              //std::cout << "\rJob is valid but not finished... " << std::flush;
          }
        }
      }

    }
    
  }

  // If we are animating handle this time step
  if( true == this->isAnimating() )
  {
    if( true == (*_update)() )
    {
      this->_incrementTimeStep();
      this->_checkTime( true );
    }
   
  }

  // If we have a timeline or a dynamic set check the steps
  if( true == this->_checkTime() )
  {
    Guard guard ( this );
    if( true == this->_useDynamic || true == this->_useTimeLine )
    {
      this->_checkTimeSteps( caller );
      this->_checkTime( false );
    }
  }

  // make sure sets that should be shown are shown
  for( unsigned int i = 0; i < _sets.size(); ++i )
  {
    this->setGroup( i, _sets.at( i ).index );
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the current group at index <index> in the set
//
///////////////////////////////////////////////////////////////////////////////

unsigned int ModelPresentationDocument::getCurrentGroupFromSet( unsigned int index ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  return _sets.at( index ).index;
}


///////////////////////////////////////////////////////////////////////////////
//
// Show the group in the specified set and hide all others.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::setGroup ( unsigned int set, unsigned int group )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
#if 0
  for( unsigned int modelIndex = 0; modelIndex < _mpdModels.models->getNumChildren(); ++modelIndex )
  {
    _mpdModels.models->setValue( modelIndex, false );
  }
  
  for( std::map< std::string, bool >::iterator iter =  _sets.at( set ).groups.at( group ).visibleModelsMap.begin();
                                               iter != _sets.at( set ).groups.at( group ).visibleModelsMap.end();
                                               ++iter )
  {
    std::string modelName = (*iter).first;
    bool value            = (*iter).second;
    unsigned int mIndex   = _mpdModels.modelMap[modelName];
    _mpdModels.models->setValue( mIndex, value );
  }
#else
  for( unsigned int i = 0; i < _sets.at( set ).groups.size(); ++i )
  {
    for( std::map< std::string, bool >::iterator iter =  _sets.at( set ).groups.at( i ).visibleModelsMap.begin();
                                               iter != _sets.at( set ).groups.at( i ).visibleModelsMap.end();
                                               ++iter )
    {
      std::string modelName = (*iter).first;
      bool value            = (*iter).second;
      unsigned int mIndex   = _mpdModels.modelMap[modelName];

      if( true == value )
      {
        _mpdModels.models->setValue( mIndex, false );
      }
      
    }
  }
  for( std::map< std::string, bool >::iterator iter =  _sets.at( set ).groups.at( group ).visibleModelsMap.begin();
                                               iter != _sets.at( set ).groups.at( group ).visibleModelsMap.end();
                                               ++iter )
  {
    std::string modelName = (*iter).first;
    bool value            = (*iter).second;
    unsigned int mIndex   = _mpdModels.modelMap[modelName];
    if( true == value )
    {
      _mpdModels.models->setValue( mIndex, value );
    }
  }
#endif
  _sets.at( set ).index = group;  
}


///////////////////////////////////////////////////////////////////////////////
//
// Go to the next group in the set
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::nextStep ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  // Update the global time
  if( _globalCurrentTime == _globalTimelineEnd )
  {
    _globalCurrentTime = 0;
  }
  else
  {
    _globalCurrentTime ++;
  }
 
  for( unsigned int i = 0; i < _timeSets.size(); ++i )
  {
    if( false == this->isAnimating() )
    {
      if( true == _useTimeLine )
      {

        if( _timeSets.at( i ).currentTime == _globalTimelineEnd )
          _timeSets.at( i ).currentTime = 0;
        else
          _timeSets.at( i ).currentTime ++;
      }
    }
  }
  for( unsigned int i = 0; i < _dynamicSets.size(); ++i )
  {
   if( true == _useDynamic )
    {
      for( unsigned int j = 0; j < _dynamicSets.at( i ).models->getNumChildren(); ++j )
      {
        _dynamicSets.at( i ).models->setValue( j, false );
      }
       if( _dynamicSets.at( i ).currentTime == _globalTimelineEnd )
        _dynamicSets.at( i ).currentTime = 0;
      else
        _dynamicSets.at( i ).currentTime ++;
    }
  }
  this->_checkTime( true );  
}


///////////////////////////////////////////////////////////////////////////////
//
// Go to the next group in the set
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::prevStep ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Update the global time
  if( _globalCurrentTime == 0 )
  {
    _globalCurrentTime = _globalTimelineEnd;
  }
  else
  {
    _globalCurrentTime --;
  }

  for( unsigned int i = 0; i < _timeSets.size(); ++i )
  {
    if( false == this->isAnimating() )
    {
      if( true == _useTimeLine )
      {
        if( _timeSets.at( i ).currentTime == 0 )
          _timeSets.at( i ).currentTime = _timeSets.at( i ).endTime;
        else
          _timeSets.at( i ).currentTime --;
      }
    }
  }
  for( unsigned int i = 0; i < _dynamicSets.size(); ++i )
  {
    if( true == _useDynamic )
    {
      for( unsigned int j = 0; j < _dynamicSets.at( i ).models->getNumChildren(); ++j )
      {
        _dynamicSets.at( i ).models->setValue( j, false );
      }
      if( _dynamicSets.at( i ).currentTime == 0 )
        _dynamicSets.at( i ).currentTime = _dynamicSets.at( i ).endTime;
      else
        _dynamicSets.at( i ).currentTime --;
    }
  }
  this->_checkTime( true );
}



///////////////////////////////////////////////////////////////////////////////
//
// Go to the next group in the set
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::firstStep ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Set the global time to 0 for text updating
  _globalCurrentTime = 0;

  if( false == this->isAnimating() )
  {

    if( true == _useTimeLine )
    {
      for( unsigned int i = 0; i < _timeSets.size(); ++i )
      {
        _timeSets.at( i ).currentTime = 0;
      }
      this->_checkTime( true );
    }
    if( true == _useDynamic )
    {
      for( unsigned int i = 0; i < _dynamicSets.size(); ++i )
      {
        _dynamicSets.at( i ).currentTime = 0;
        for( unsigned int j = 0; j < _dynamicSets.at( i ).models->getNumChildren(); ++j )
        {
          _dynamicSets.at( i ).models->setValue( j, false );
        }
      }
      this->_checkTime( true );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the current view matrix
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::displayViewMatrix()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  Usul::Interfaces::IViewMatrix::QueryPtr view ( Usul::Documents::Manager::instance().activeView() );

  if( true == view.valid() )
  {
    
    osg::Matrixf matrix = view->getViewMatrix();

    std::ostringstream out;
    
    std::cout << "Current view matrix: " << std::endl;
    Usul::Print::matrix ( "", matrix.ptr(), out, 20 );
    std::cout << out.str().c_str() << std::endl;
    
    std::ostringstream csvMatrix;
    Usul::Math::Matrix44f m;
    OsgTools::Convert::matrix ( matrix, m );
    csvMatrix << std::setprecision ( 4 ) << std::fixed << m[0] << "," 
              << std::setprecision ( 4 ) << std::fixed << m[4] << "," 
              << std::setprecision ( 4 ) << std::fixed << m[8]  << "," 
              << std::setprecision ( 4 ) << std::fixed << m[12] << ","
              << std::setprecision ( 4 ) << std::fixed << m[1] << "," 
              << std::setprecision ( 4 ) << std::fixed << m[5] << "," 
              << std::setprecision ( 4 ) << std::fixed << m[9]  << "," 
              << std::setprecision ( 4 ) << std::fixed << m[13] << ","
              << std::setprecision ( 4 ) << std::fixed << m[2] << "," 
              << std::setprecision ( 4 ) << std::fixed << m[6] << "," 
              << std::setprecision ( 4 ) << std::fixed << m[10] << "," 
              << std::setprecision ( 4 ) << std::fixed << m[14] << ","
              << std::setprecision ( 4 ) << std::fixed << m[3] << "," 
              << std::setprecision ( 4 ) << std::fixed << m[7] << "," 
              << std::setprecision ( 4 ) << std::fixed << m[11] << "," 
              << std::setprecision ( 4 ) << std::fixed << m[15] << std::endl;
    std::cout << "\n" << csvMatrix.str().c_str() << std::endl; 
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Return the current animation state
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::isAnimating()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _isAnimating;
}


///////////////////////////////////////////////////////////////////////////////
//
//  set the current animation set
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::isAnimating( bool value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _isAnimating = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the parameter file and parse elements.
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::_readParameterFile( XmlTree::Node &node, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  

  Attributes& attributes ( node.attributes() );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "usetools" == iter->first )
    {
      std::string value = "false";
      Usul::Strings::fromString ( iter->second, value );
      if( value == "true" )
        _showTools = true;
    }
  }

  Children& children ( node.children() );
	unsigned int set = 0;
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "set" == node->name() )
    {
      this->_parseSet( *node, caller, progress, set );
      set ++;
    }
    if ( "timeset" == node->name() )
    {
      this->_parseTimeSet( *node, caller, progress );
    }
    //if ( "static" == node->name() )
    //{
    //  //std::cout << "Parsing static entries..." << std::endl;
    //  this->_parseStatic( *node, caller, progress );
    //} 
    if ( "location" == node->name() )
    {
      //std::cout << "Parsing location entries..." << std::endl;
      MpdDefinitions::Location location;
      this->_parseLocation( *node, caller, progress, location, _locationNames );
      _locations[ location.first ] = location.second;
    } 
    if ( "dynamic" == node->name() )
    {
      //std::cout << "Parsing dynamic entries..." << std::endl;
      this->_parseDynamic( *node, caller, progress );
    }
    if ( "sequence" == node->name() )
    {
      //std::cout << "Parsing dynamic entries..." << std::endl;
      this->_parseSequence( *node, caller, progress );
    }
    if ( "models" == node->name() )
    {
      //std::cout << "Parsing dynamic entries..." << std::endl;
      this->_parseModels( *node, caller, progress );
    }
  }
  
  return true;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Parse a set of models for the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_parseModels( XmlTree::Node &node, Unknown *caller, Unknown *progress )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
           
  }
  // TODO: create a set here --
  MpdDefinitions::MpdModels models;
  models.models = new osg::Switch();
  unsigned int count = 0;
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "model" == node->name() )
    {
      // Format returns a copy, not a reference.
      std::string name = Usul::Strings::format( "Unknown",count );

      models.models->addChild( this->_parseModel( *node, caller, progress, name ), false );
      models.modelMap[name] = count;

      
    }

    // Feedback.
    this->setProgressBar ( true, count, children.size(), progress );

    ++count;
  }


#if 0
  // Turn off display lists
  OsgTools::DisplayLists dl( false );
  dl( _models.get() );
#endif

  _mpdModels = models;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a static set of models (shown in every scene).
//
///////////////////////////////////////////////////////////////////////////////
//
//void ModelPresentationDocument::_parseStatic( XmlTree::Node &node, Unknown *caller, Unknown *progress )
//{ 
//  USUL_TRACE_SCOPE;
//  Guard guard ( this->mutex() );
//  
//  Attributes& attributes ( node.attributes() );
//  Children& children ( node.children() );
//  
//  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
//  {
//           
//  }
//  // TODO: create a set here --
//  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
//  {
//    XmlTree::Node::RefPtr node ( *iter );
//    if ( "model" == node->name() )
//    {
//      std::cout << "Found static model: " << std::flush;
//      std::string name;
//      _static->addChild( this->_parseModel( *node, caller, progress, name ) );
//    }  
//  }
//#if 1
//  // Turn off display lists
//  OsgTools::DisplayLists dl( false );
//  dl( _static.get() );
//#endif
//}



///////////////////////////////////////////////////////////////////////////////
//
//  Parse a location.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_parseLocation( XmlTree::Node &node, Unknown *caller, Unknown *progress, MpdDefinitions::Location &location, MpdDefinitions::LocationNames &names )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  
  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  
  
  std::string name = "Location";
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  { 
    if ( "name" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, name );
    }  
  }
  // TODO: create destination matrix here --
  names.push_back( name );
  
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "matrix" == node->name() )
    {
      this->_parseMatrix( *node, caller, progress, name, location, names );
    }
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a matrix
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_parseMatrix( XmlTree::Node &node, Unknown *caller, Unknown *progress, const std::string& name, MpdDefinitions::Location &location, MpdDefinitions::LocationNames &names )
{
   USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  std::string type;
  std::string value;
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    
    if ( "type" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, type );
    }
    if ( "value" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, value );
    }
 
  }
  // TODO: create destination matrix here --
  osg::Matrix matrix;
  if( value.size() > 0 && type.size() > 0 )
    this->_setMatrix( &matrix, value, type );
  else
    std::cout << "Incorrect Matrix format!" << std::endl;

  location.first = name;
  location.second = matrix;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set a matrix location
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_setMatrix( osg::Matrix * matrix, const std::string& values, const std::string& type )
{
  std::istringstream is( values );
  
  osg::ref_ptr< osg::MatrixTransform > mt ( new osg::MatrixTransform );
  if( "xyzpry" == type )
  {
    //std::cout << "Found XYZ, Pitch, Roll, Yaw designation location" << std::endl;

    float x ( 0 ), y( 0 ), z( 0 ), pitch( 0 ), roll( 0 ), yaw( 0 );
    is >> x >> y >> z >> pitch >> roll >> yaw;

    
    mt->setMatrix (  osg::Matrix::rotate ( osg::inDegrees ( pitch ), 1.0f, 0.0f, 0.0f ) *
                     osg::Matrix::rotate ( osg::inDegrees ( roll ), 0.0f, 1.0f, 0.0f ) *
                     osg::Matrix::rotate ( osg::inDegrees ( yaw ), 0.0f, 0.0f, 1.0f ) *
                     osg::Matrix::translate ( osg::Vec3f(x, y, z) ) );
    matrix->setRotate( mt->getMatrix().getRotate() );
    matrix->translate( mt->getMatrix().getTrans() );
    
  }
  if( "matrix" == type )
  {
    //std::cout << "Found matrix designation location" << std::endl;
    char temp;
    osg::Matrixf::value_type m[16];
      is >> m[0] >> temp >> m[4] >> temp >> m[8] >> temp  >> m[12] >> temp;
      is >> m[1] >> temp >> m[5] >> temp >> m[9] >> temp  >> m[13] >> temp;
      is >> m[2] >> temp >> m[6] >> temp >> m[10] >> temp >> m[14] >> temp;
      is >> m[3] >> temp >> m[7] >> temp >> m[11] >> temp >> m[15];
    matrix->set( m );

  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a set.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_parseSet( XmlTree::Node &node, Unknown *caller, Unknown *progress, unsigned int setnum )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  
  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  MpdDefinitions::MpdSet set;
  set.index = 0;
  set.name = "Set";
  set.menuName = "Models";
  _useModels = true;
  
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "name" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, set.name );
    }
    if ( "numgroups" == iter->first )
    {
      //Usul::Strings::fromString ( iter->second, _header.gridSize[1] );
    }
    if ( "menu_name" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, set.menuName );
    }
        
  }
  // Add the set to the writer
  _writer->addSet( set.name, set.menuName );

  // TODO: create a set here --
  osg::ref_ptr< osg::Switch > switchNode ( new osg::Switch );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "group" == node->name() )
    {
      //std::cout << "Found group..." << std::endl;
      MpdDefinitions::MpdGroup grp;
      this->_parseGroup( *node, caller, progress, set, grp );
      set.groups.push_back( grp );
    }
  }
  

#if 1
  // Turn off display lists
  OsgTools::DisplayLists dl( false );
  dl( switchNode.get() );
#endif

  this->_sets.push_back( set );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a group.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ModelPresentationDocument::_parseGroup( XmlTree::Node &node, Unknown *caller, Unknown *progress, MpdDefinitions::MpdSet & set, MpdDefinitions::MpdGroup & grp )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  
  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  std::string groupName = "group";
  
  grp.name = "Unknown";
  grp.visibleModels.resize( 0 );


  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "name" == iter->first )
    {
      
      Usul::Strings::fromString ( iter->second, groupName );
      set.groupNames.push_back( groupName );
      grp.name = groupName;
    }
    if ( "numitems" == iter->first )
    {
      //Usul::Strings::fromString ( iter->second, _header.gridSize[1] );
    }
        
  }
  
  // TODO: Create group here ---
  GroupPtr group ( new osg::Group );

  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );

    if ( "show" == node->name() )
    {
      Attributes& att ( node->attributes() );
      for ( Attributes::iterator att_iter = att.begin(); att_iter != att.end(); ++att_iter )
      {
        if ( "name" == att_iter->first )
        {
          std::string name;
          Usul::Strings::fromString ( att_iter->second, name );
          grp.visibleModelsMap[name] = true;

          // add the model to the set to be shown
          _writer->addModelToSet( name, set.name, grp.name );

        }
      }
    } 
    if ( "hide" == node->name() )
    {
      Attributes& att ( node->attributes() );
      for ( Attributes::iterator att_iter = att.begin(); att_iter != att.end(); ++att_iter )
      {
        if ( "name" == att_iter->first )
        {
          std::string name;
          Usul::Strings::fromString ( att_iter->second, name );
          grp.visibleModelsMap[name] = false;
        }
      }
    } 
  }
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a time line set.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_parseTimeSet( XmlTree::Node &node, Unknown *caller, Unknown *progress )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Ok to update the status bar with time related information
  _useTimeLine = true;

  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  MpdDefinitions::MpdTimeSet timeset;
  timeset.currentTime = 0;
  timeset.endTime = 0;
  timeset.timeline = new osg::Switch;
  timeset.name = "Unknown";
  timeset.menuName = "Timelines";
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    
    if ( "endTime" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, timeset.endTime );
      _userSpecifiedEndTime = true;
    }
    if ( "name" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, timeset.name );
    }
    if ( "timelength" == iter->first )
    {
      unsigned int interval = 1;
      Usul::Strings::fromString ( iter->second, interval );
      std::cout << "Setting time update interval to " << interval << std::endl;
      _update = ( UpdatePolicyPtr( new UpdatePolicy( interval ) ) );
      
    }
    if ( "menu_name" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, timeset.menuName );
    }
    if ( "visible" == iter->first )
    {
      std::string value = "false";

      Usul::Strings::fromString ( iter->second, value );
      if( value == "true" || value == "TRUE" ||
          value == "yes"  || value == "YES" )
      {
        timeset.visible = true;
      }
      else
      {
        timeset.visible = false;
      }
      
    }
        
  }

  //Add the timeset to the writer
  _writer->addTimeSet( timeset.name, timeset.menuName, timeset.endTime );
  
  // TODO: create a set here --
  osg::ref_ptr< osg::Switch > switchNode ( new osg::Switch );
  unsigned int currentTime = 0;
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    
    XmlTree::Node::RefPtr node ( *iter );
    if ( "group" == node->name() )
    {
      this->_parseTimeGroup( *node, caller, progress, currentTime, timeset );
    }
    currentTime ++;
  }
  currentTime --;
  _globalTimelineEnd = Usul::Math::maximum( this->_globalTimelineEnd, currentTime );
  timeset.endTime = currentTime;
#if 1
  // Turn off display lists
  OsgTools::DisplayLists dl( false );
  dl( switchNode.get() );
#endif
  //timeset.timeline = switchNode.release();
  
  timeset.visible = true;

  _timeSets.push_back( timeset );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a time line group.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ModelPresentationDocument::_parseTimeGroup( XmlTree::Node &node, Unknown *caller, Unknown *progress, unsigned int &currentTime, MpdDefinitions::MpdTimeSet &timeset )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
   
  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  
  MpdDefinitions::MpdTimeGroup timeGroup;
  timeGroup.startTime = currentTime;
  timeGroup.endTime = currentTime + 1;
  
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "startTime" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, timeGroup.startTime );
    }
    if ( "endTime" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, timeGroup.endTime );
      currentTime = timeGroup.endTime;
    }    

  }
  if( false == _userSpecifiedEndTime )
  {
    _globalTimelineEnd = Usul::Math::maximum( currentTime, _globalTimelineEnd );
    
  }
  
  GroupPtr group ( new osg::Group );


  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
   
    if ( "show" == node->name() )
    {
      Attributes& att ( node->attributes() );
      for ( Attributes::iterator att_iter = att.begin(); att_iter != att.end(); ++att_iter )
      {
        if ( "name" == att_iter->first )
        {
          std::string name;
          Usul::Strings::fromString ( att_iter->second, name );
          timeGroup.visibleModelsMap[name] = true;

          
        }

      }
    }  

  }

  timeset.groups.push_back( timeGroup );
  // add the time group to the writer
  std::vector< std::string > modelList;
  for( std::map< std::string, bool >::iterator iter = timeGroup.visibleModelsMap.begin();
                                               iter != timeGroup.visibleModelsMap.end();
                                               ++iter )
  {
    modelList.push_back( (*iter).first );
  }
  _writer->addModelsToTimeSet( modelList, timeset.name, timeGroup.startTime, timeGroup.endTime );

  return group.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Parse a dynamic load structure.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_parseDynamic( XmlTree::Node &node, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Enable Dynamic Features
  _useDynamic = true;

  Attributes& attributes ( node.attributes() );
  std::string path;
  osg::ref_ptr< osg::Group > group ( new osg::Group );

  // Create a new MpdDynamicSet
  MpdDefinitions::MpdDynamicSet dset;
  dset.currentTime = 0;
  dset.header.directory = ".";
  dset.endTime = 0;
  dset.maxFilesToLoad = 1;
  dset.nextIndexToLoad = 0;
  dset.header.extension = "*";
  dset.menuName = "DynamicSets";
  dset.name = "Unknown";
  dset.models = new osg::Switch;
  dset.header.prefix = "";
  dset.absolute = false;
  if( 0 == _dynamicSets.size() )
  {
    dset.visible = true;
  }

  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "prefix" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, dset.header.prefix );
    }
    if ( "directory" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, dset.header.directory );
    }
    if ( "extension" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, dset.header.extension );
    }
    if ( "menuName" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, dset.menuName );
    }
    if ( "name" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, dset.name );
    }      
    if ( "max" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, dset.maxFilesToLoad );
    }
    if ( "absolute" == iter->first )
    {
      std::string value( "" );
      Usul::Strings::fromString ( iter->second, value );
      if( "true" == value || "TRUE" == value )
      {
        dset.absolute = true;
      }
    }  
    if ( "position" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, dset.position );
    }
  }
  for( unsigned int i = 0; i < dset.maxFilesToLoad; ++i )
  {
    MpdDefinitions::MpdDynamicGroup grp;
    grp.filename = "";
    grp.loaded = false;
    grp.valid = false;
    std::string text = Usul::Strings::format( "Step ", i + 1, " of ",dset.maxFilesToLoad, " is not loaded..." );
    dset.models->addChild( this->_createProxyGeometry( text, caller ), false );
    dset.groups.push_back( grp );
  }
  dset.models->setValue( 0, true );
  dset.endTime = dset.maxFilesToLoad;
  MpdJob::RefPtr job;
  _jobs.push_back( job );
  _dynamicSets.push_back( dset );

  // Add dynamic set to the writer
  _writer->addDynamicSet( dset.name,
                          dset.menuName, 
                          dset.header.prefix, 
                          dset.header.extension, 
                          dset.header.directory, 
                          dset.maxFilesToLoad );

  
}

///////////////////////////////////////////////////////////////////////////////
//
//  Parse a sequence.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_parseSequence( XmlTree::Node &node, Unknown *caller, Unknown *progress )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

   // Ok to update the status bar with time related information
  _useSequence = true;

  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  //MpdDefinitions::MpdSequence sequence;
  _sequence.name = "Unknown";
  _sequence.menuName = "Sequence";
  _sequence.groups = new osg::Switch;

  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "name" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _sequence.name );
    }

    if ( "menu_name" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _sequence.menuName );
    }   
  }
  // add the sequence to the writer
  _writer->addSequence( _sequence.name, _sequence.menuName );

  // TODO: create a step here --
  //osg::ref_ptr< osg::Switch > switchNode ( new osg::Switch );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    
    XmlTree::Node::RefPtr node ( *iter );
    //if ( "groups" == node->name() )
    //{
    //  this->_parseSequenceGroups( *node, caller, progress );
    //}
    if ( "step" == node->name() )
    {
      MpdDefinitions::MpdSequenceStep step;
      this->_parseSequenceStep( *node, caller, progress, step );

      _sequence.steps.push_back( step );

      // Add the sequence step to the writer
      std::vector< std::string > modelList;
      for( std::map< std::string, bool >::iterator iter = step.visibleModelsMap.begin();
                                                   iter != step.visibleModelsMap.end();
                                                   ++iter )
      {
        modelList.push_back( (*iter).first );
      }
      _writer->addStepToSequence( step.locationName, modelList );

    }
  }
#if 0
  // Turn off display lists
  OsgTools::DisplayLists dl( false );
  dl( switchNode.get() );
#endif

}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a sequence set of groups
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_parseSequenceGroups( XmlTree::Node &node, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
  }

  MpdDefinitions::MpdSet set;
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "group" == node->name() )
    {
      MpdDefinitions::MpdGroup grp;
      this->_parseGroup( *node, caller, progress, set, grp );
    }  
    
  }
  
}
///////////////////////////////////////////////////////////////////////////////
//
//  Parse a sequence step
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_parseSequenceStep( XmlTree::Node &node, Unknown *caller, Unknown *progress, MpdDefinitions::MpdSequenceStep &step )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Attributes& attributes ( node.attributes() );
  Children& children ( node.children() );
  step.changeLocation = true;
  step.overwriteGroup = true;
  step.name = "Unknown";
  step.locationName = "";
  
  
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "name" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, step.name );
    }
    if ( "overwriteGroup" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, step.overwriteGroup );
    }
    if ( "changeLocation" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, step.changeLocation );
    }

  }
 
  // initialize all the visible groups in this step to false
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "location" == node->name() )
    {
      Attributes& att ( node->attributes() );
      for ( Attributes::iterator att_iter = att.begin(); att_iter != att.end(); ++att_iter )
      {
        if ( "name" == att_iter->first )
        {
          std::string name;
          Usul::Strings::fromString ( att_iter->second, name );
          step.locationName = name;
        }
      }
    } 
    if( "show" == node->name() )
    {
      Attributes& att ( node->attributes() );
      for ( Attributes::iterator att_iter = att.begin(); att_iter != att.end(); ++att_iter )
      {
        if ( "name" == att_iter->first )
        {
          std::string name;
          Usul::Strings::fromString ( att_iter->second, name );
          step.visibleModelsMap[name] = true;
        }
      }
     
    }
    
  }
  

}
  
///////////////////////////////////////////////////////////////////////////////
//
//  Parse a model.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ModelPresentationDocument::_parseModel( XmlTree::Node &node, Unknown *caller, Unknown *progress, std::string &name )
{ 
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Attributes& attributes ( node.attributes() );
  std::string path;
  osg::ref_ptr< osg::Group > group ( new osg::Group );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {

    if ( "file" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, path );
      group->addChild( this->_loadFile( path, caller, progress ) );
    }
    if ( "directory" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, path );
      group->addChild( this->_loadDirectory( path, caller, progress ) );
    }
    if ( "name" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, name );
    }

   
  }
  // add a model to the writer
  _writer->addModel( name, path );
  return group.release();  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load a single model.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ModelPresentationDocument::_loadFile( const std::string& filename, Unknown *caller, Unknown *progress )
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
        
        this->_openDocument ( Usul::File::fullPath( filename ), info.document.get(), caller, 0x0 );

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
      catch( const std::exception &e )
      {
        std::cout << "Error 2680794098: Standard exception caught while trying to open file: " << filename << ". Exception: " << e.what() << std::endl;

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
//  Load a directory of models.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ModelPresentationDocument::_loadDirectory( const std::string& dir, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
 
  //std::cout << dir << " directory loading..." << std::endl;
  Files files;
  Usul::File::find( dir, "*", files );
  
  osg::ref_ptr< osg::Group> group ( new osg::Group );
  std::cout << "Found " << files.size() << " files in " << dir << std::endl;
  for( unsigned int i = 0; i < files.size(); ++i )
  {
   
    std::string filename = "";//files.at(i).c_str();
     #ifdef _MSC_VER
        filename = files.at(i).c_str();
      #else
        filename =  dir + files.at(i).c_str() ;
      #endif
    std::cout << "Loading file: " << filename << std::endl;

    try
    {
       // This will create a new document.
      Info info ( DocManager::instance().find ( filename, caller ) );

      // Check to see if we created a document.
      if ( true == info.document.valid() && true == info.document->canOpen( filename ) )
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

            group->addChild( build->buildScene( opt, caller ) );
            
           }
        }
        catch( const std::exception &e )
        {
          std::cout << "Error 1994767425: Standard exception caught while trying to open file: " << filename << ". Exception: " << e.what() << std::endl;
        }
      }
      else
        std::cout << "\tUnknown file format for: " << filename << std::endl;
    }
    catch ( ... )
    {
      std::cout << "\tUnknown file format for: " << filename << std::endl;
    }
  }

  return group.release();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller, Unknown *progress )
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
//  Convenience function to set status bar and flush events.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_setStatusBar ( const std::string &text, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Usul::Interfaces::IStatusBar::QueryPtr status ( caller );
  if ( status.valid() )
    status->setStatusBarText ( text, true );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Build an animation path 
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationDocument::MatrixVec ModelPresentationDocument::_getInterpolationMatrices ( const osg::Matrixd &m1, const osg::Matrixd &m2 ) const
{
  USUL_TRACE_SCOPE;
  // Guard guard ( this ); Nothing to guard.
  
  osg::Vec3d m1trans ( m1.getTrans() );
  osg::Vec3d m2trans ( m2.getTrans() );

  osg::Vec3d m3trans ( osg::Vec3d( m1trans[0], m1trans[1], m1trans[2] + 1.0 ) );

  // ( p3 - p2 ) ^ ( p1 - p2 )
  osg::Vec3d normal ( ( m3trans - m2trans ) ^ ( m1trans - m2trans ) );
  normal.normalize();
  double d = Usul::Math::sqrt( Usul::Math::pow( ( m2trans[0] - m1trans[0] ), 2.0 ) +
                   	       Usul::Math::pow( ( m2trans[1] - m1trans[1] ), 2.0 ) +
                               Usul::Math::pow( ( m2trans[2] - m1trans[2] ), 2.0 ) );

  /*osg::Vec3d interpolated1 ( ( m2trans[0] - m1trans[0] ) * 0.25 ,
                             ( m2trans[1] - m1trans[1] ) * 0.25, 
                             ( m2trans[2] - m1trans[2] ) * 0.25 ); 

  osg::Vec3d interpolated2 ( ( m2trans[0] - m1trans[0] ) * 0.75 ,
                             ( m2trans[1] - m1trans[1] ) * 0.75, 
                             ( m2trans[2] - m1trans[2] ) * 0.75 ); */
  osg::Vec3d interpolated1( m1trans + ( ( m2trans - m1trans ) * 0.25 ) );
  osg::Vec3d interpolated2( m1trans + ( ( m2trans - m1trans ) * 0.75 ) );

  osg::Vec3d intpos1 ( interpolated1 + ( normal * ( d /2 ) ) );
  osg::Vec3d intpos2 ( interpolated2 + ( normal * ( d /2 ) ) );

  osg::Matrixd mprime1;
  osg::Matrixd mprime2;

  mprime1.setRotate( m1.getRotate() );
  mprime1.setTrans( intpos1 );

  mprime2.setRotate( m2.getRotate() );
  mprime2.setTrans( intpos2 );

  MatrixVec mvec;

  mvec.push_back( mprime1 );
  mvec.push_back( mprime2 );

  return mvec;  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build an animation path 
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::setAnimationPath ( const std::string& name )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  typedef IAnimatePath::PackedMatrices PackedMatrices;
  typedef IAnimatePath::PackedMatrix PackedMatrix;

  bool useSequence = false;

  // Get needed interfaces.
  IAnimatePath::QueryPtr path ( Usul::Components::Manager::instance().getInterface ( IAnimatePath::IID ) );
  Usul::Interfaces::IViewMatrix::QueryPtr view ( Usul::Documents::Manager::instance().activeView() );
  if ( false == path.valid() || false == view.valid() )
    return;

  // The matrices.
  PackedMatrices matrices;

  // Get current matrix and append.
  const osg::Matrixd m0 ( view->getViewMatrix() );
  matrices.push_back ( PackedMatrix ( m0.ptr(), m0.ptr() + 16 ) );

  // Get final matrix but don't append yet.
  osg::Matrixd m1;

  m1 = this->_locations[name];

  // Append final matrix.
  matrices.push_back ( PackedMatrix ( m1.ptr(), m1.ptr() + 16 ) );

  // Animate through these matrices.
  path->animatePath ( matrices );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller )
{
  typedef MenuKit::ToggleButton ToggleButton;
  typedef MenuKit::Button       Button;
  typedef MenuKit::RadioButton  Radio;

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Add the model menu
  if( _useModels )
  {
    for( unsigned int i = 0; i < _sets.size(); ++i )
    {
      const std::string menuName ( _sets.at( i ).menuName );
      MenuKit::Menu::RefPtr ModelMenu ( menu.find ( menuName, true ) );
      MenuKit::Menu::RefPtr ModelSubMenu ( new MenuKit::Menu ( _sets.at(i).name, MenuKit::Menu::VERTICAL ) );
     
      for( unsigned int j = 0; j < _sets.at( i ).groups.size(); ++j )
      {
         ModelSubMenu->append ( new Radio ( new MpdMenuCommand( me.get(), _sets.at( i ).groups.at( j ).name, i, j ) ) );
      }
      ModelMenu->append( ModelSubMenu.get() );
    }
    
  }

  // Add the Timeline menu if needed
  if( true == _useTimeLine )
  {
    // Timeline Models
   
    for( unsigned int i = 0; i < _timeSets.size(); ++i )
    {
      const std::string menuName ( _timeSets.at(i).menuName );
       MenuKit::Menu::RefPtr TimelineModelsMenu ( menu.find ( menuName, true ) );
    
      TimelineModelsMenu->append ( new ToggleButton ( new MpdTimelineModel ( me.get(), _timeSets.at( i ).name, i ) ) );
    }
    //menu.append ( TimelineModelsMenu );
    
  }


  // Add the Dynamic Menu
  if( true == _useDynamic )
  {
    for( unsigned int i = 0; i < _dynamicSets.size(); ++i )
    {
      const std::string menuName ( _dynamicSets.at( i ).menuName );
      MenuKit::Menu::RefPtr dynamicMenu ( menu.find ( menuName, true ) );
    
      dynamicMenu->append ( new ToggleButton ( new MpdDynamicModel ( me.get(), _dynamicSets.at( i ).name, i ) ) );
    }
  }
  // Activate the timeline animation controls if either a dynamic or a timeline is added
  if( true == _useDynamic || true == _useTimeLine )
  {
     // Timeline navigation controls
    MenuKit::Menu::RefPtr TimelineMenu ( new MenuKit::Menu ( "Animation", MenuKit::Menu::VERTICAL ) );

    TimelineMenu->append ( new Button ( new MpdNextCommand( me.get() ) ) );
    TimelineMenu->append ( new Button ( new MpdPrevTimestep( me.get() ) ) );
    TimelineMenu->append ( new Button ( new MpdFirstTimestep( me.get() ) ) );
    TimelineMenu->append ( new ToggleButton ( new MpdAnimation( me.get() ) ) );

    MenuKit::Menu::RefPtr TimelineSubMenu ( new MenuKit::Menu ( "Speed", MenuKit::Menu::VERTICAL ) );
    //TimelineSubMenu->append( new Button ( new MpdSlowerAnimationSpeed( me.get(), 5, "Slower x5" ) ) );
    //TimelineSubMenu->append( new Button ( new MpdSlowerAnimationSpeed( me.get(), 2, "Slower x2" ) ) );
    TimelineSubMenu->append( new Radio ( new MpdAnimationSpeed( me.get(), 60, "Every 60 Frames" ) ) );
    TimelineSubMenu->append( new Radio ( new MpdAnimationSpeed( me.get(), 30, "Every 30 Frames" ) ) );
    TimelineSubMenu->append( new Radio ( new MpdAnimationSpeed( me.get(), 10, "Every 10 Frames" ) ) );
    TimelineSubMenu->append( new Radio ( new MpdAnimationSpeed( me.get(), 5,  "Every 05 Frames" ) ) );
    TimelineSubMenu->append( new Radio ( new MpdAnimationSpeed( me.get(), 1,  "Every Frame" ) ) );
    //TimelineSubMenu->append( new Button ( new MpdFasterAnimationSpeed( me.get(), 2, "Faster x2" ) ) );
    //TimelineSubMenu->append( new Button ( new MpdFasterAnimationSpeed( me.get(), 2, "Faster x5" ) ) );
    TimelineMenu->append( TimelineSubMenu.get() );


    menu.append ( TimelineMenu );
  }

  if( true == _useSequence )
  {
    const std::string menuName ( _sequence.menuName );
    const std::string name ( _sequence.name );
    MenuKit::Menu::RefPtr SequenceMenu ( menu.find ( menuName, true ) );

    SequenceMenu->append( new Button ( new MpdNextSequence( me.get() ) ) );
    SequenceMenu->append( new Button ( new MpdPrevSequence( me.get() ) ) );
    SequenceMenu->append( new Button ( new MpdFirstSequence( me.get() ) ) );
     
  }

  if( _locationNames.size() > 0 )
  {
    MenuKit::Menu::RefPtr locationMenu ( new MenuKit::Menu ( "Locations", MenuKit::Menu::VERTICAL ) );
    for( unsigned int i = 0; i < _locationNames.size(); ++i )
    {  
       locationMenu->append( new Button ( new MpdLocation( me.get(), _locationNames.at( i )  ) ) );
    }
    menu.append( locationMenu );
  }
  if( true == _showTools )
  {
    MenuKit::Menu::RefPtr toolsMenu ( new MenuKit::Menu ( "Tools", MenuKit::Menu::VERTICAL ) );
    toolsMenu->append ( new Button ( new MpdTools( me.get(), "ShowMatrix" ) ) );
    menu.append( toolsMenu );
  }
  

}


///////////////////////////////////////////////////////////////////////////////
//
// toggle animation
//
///////////////////////////////////////////////////////////////////////////////


void ModelPresentationDocument::animate( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _isAnimating = state;
}


///////////////////////////////////////////////////////////////////////////////
//
// check animation state
//
///////////////////////////////////////////////////////////////////////////////


bool ModelPresentationDocument::animate()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  return _isAnimating;
}


///////////////////////////////////////////////////////////////////////////////
//
// check animation state
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::timelineModelState( unsigned int i, bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _timeSets.at( i ).visible = state;
}


///////////////////////////////////////////////////////////////////////////////
//
// check animation state
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::timelineModelState( unsigned int i )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  

  return _timeSets.at( i ).visible;
}


///////////////////////////////////////////////////////////////////////////////
//
// check animation state
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_incrementTimeStep()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  
  if( _globalCurrentTime >= _globalTimelineEnd )
    _globalCurrentTime = 0;
  else
    _globalCurrentTime += 1;

  if( true == this->_useTimeLine )
  {
    for( unsigned int i = 0; i < _timeSets.size(); ++i )
    {
      if( _timeSets.at( i ).currentTime == _globalTimelineEnd )
        _timeSets.at( i ).currentTime = 0;
      else
        _timeSets.at( i ).currentTime += 1;
    }
  }
  
  if( true == this->_useDynamic )
  {
    for( unsigned int i = 0; i < _dynamicSets.size(); ++i )
    {
      if( _dynamicSets.at( i ).currentTime == _globalTimelineEnd )
        _dynamicSets.at( i ).currentTime = 0;
      else
        _dynamicSets.at( i ).currentTime += 1;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Load new files into the dynamic set at index <index>
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::validateDynamicSets()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  // Get needed interfaces.
  IDataSync::QueryPtr dataSync ( Usul::Components::Manager::instance().getInterface ( IDataSync::IID ) );
  
  for( MpdDynamicSets::iterator iter = _dynamicSets.begin(); iter < _dynamicSets.end(); ++iter )
  {
    for( unsigned int i = 0; i < (*iter).groups.size(); ++i )
    {
      std::string lockfile = Usul::File::base( (*iter).groups.at( i ).filename );
      if( true == dataSync->queryDataState( lockfile ) )
      {
        (*iter).groups.at( i ).valid = true;

      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the visibility state of a Dynamic Model at index <index>
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::dynamicModelState  ( unsigned int index, bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _dynamicSets.at( index ).visible = state;
  if( false == state )
  {
    for( unsigned int i = 0; i < _dynamicSets.at( index ).models->getNumChildren(); ++i )
    {
      _dynamicSets.at( index ).models->setValue( i, false );
    }
  }
  else
  {
    _dynamicSets.at( index ).models->setValue( _dynamicSets.at( index ).currentTime, true );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
// Get the visibility state of a Dynamic Model at index <index>
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::dynamicModelState  ( unsigned int index )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  return _dynamicSets.at( index ).visible;
  

}


///////////////////////////////////////////////////////////////////////////////
//
// Return whether or not we have loaded a dynamic set
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::_dynamic()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  return _useDynamic;
}


///////////////////////////////////////////////////////////////////////////////
//
// Update the global end time for animation
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::updateGlobalEndtime()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  if( true == this->_dynamic() )
  {
    for( unsigned int i = 0; i < _dynamicSets.size(); ++i )
    {
      _globalTimelineEnd = Usul::Math::maximum( _dynamicSets.at( i ).endTime - 1, _globalTimelineEnd );
    }
  }

  if( true == this->isAnimating() )
  {
    for( unsigned int i = 0; i < _timeSets.size(); ++i )
    {
       _globalTimelineEnd = Usul::Math::maximum( _timeSets.at( i ).endTime, _globalTimelineEnd );

    }
  }
 
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the current working directory variable
//
///////////////////////////////////////////////////////////////////////////////

std::string ModelPresentationDocument::_getWorkingDir()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  return _workingDir;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the current working directory variable
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_processJobData( unsigned int index, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  MpdJob::RefPtr job ( ( index < _jobs.size() ) ? _jobs.at ( index ) : 0x0 );
  if ( false == job.valid() )
  {
    //std::cout << "Job is invalid." << std::endl;
    return;
  }

  if ( false == job->foundNewData() )
  {
    //std::cout << "Job has no new data." << std::endl;
    // Assign to null.
    _jobs.at( index ) = 0x0;
    return;
  }
  //std::cout << "Job is valid." << std::endl;
  // Ask job for it's stuff and rebuild scene, etc.
  MpdDefinitions::Groups groups = job->getData();
  for( unsigned int i = 0; i < groups.size(); ++i )
  {
    MpdDefinitions::MpdDynamicGroup grp;
    MpdDefinitions::MpdDynamicSetHeader header = job->getHeader();
    grp.valid = false;
    grp.loaded = true;
    grp.filename="";

    // Sanity Check to see if the child at position nextIndexToLoad exists
    if( _dynamicSets.at( index ).models->getNumChildren() > _dynamicSets.at( index ).nextIndexToLoad )
    {
      _dynamicSets.at( index ).models->setChild( _dynamicSets.at( index ).nextIndexToLoad, groups.at( i ).get() );
    }
    else
    {
      std::cout << Usul::Strings::format( "When trying to setChild: The group at position ", _dynamicSets.at( index ).nextIndexToLoad, " does not exist!" ) << std::endl;
      return;
    }
    // Sanity Check to see if the group at position nextIndexToLoad exists
    if( _dynamicSets.at( index ).groups.size() > _dynamicSets.at( index ).nextIndexToLoad )
    {
      _dynamicSets.at( index ).groups.at( _dynamicSets.at( index ).nextIndexToLoad ) = grp;
    }
    else
    {
      std::cout << Usul::Strings::format( "When trying to set the group: The group at position ", _dynamicSets.at( index ).nextIndexToLoad, " does not exist!" ) << std::endl;
      return;
    }

    _dynamicSets.at( index ).header = header;
    _dynamicSets.at( index ).nextIndexToLoad ++;
    std::cout << Usul::Strings::format( "Loaded ", _dynamicSets.at( index ).nextIndexToLoad, " of ", _dynamicSets.at( index ).maxFilesToLoad, " steps." ) << std::endl;
  }
  
  // Set the rest of the steps to be this last loaded step and the "not loaded" text
  for( unsigned int i = _dynamicSets.at( index ).nextIndexToLoad; i < _dynamicSets.at( index ).models->getNumChildren(); ++i )
  {
    GroupPtr grpPtr ( new osg::Group );
    grpPtr->addChild( _dynamicSets.at( index ).models->getChild( _dynamicSets.at( index ).nextIndexToLoad - 1 ) );        
    _dynamicSets.at( index ).models->setChild( i, grpPtr.get() );

  }

  // Assign to null.
  _jobs.at( index ) = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Return the job at index <index>
//
///////////////////////////////////////////////////////////////////////////////

MpdJob::RefPtr ModelPresentationDocument::_getJobAtIndex( unsigned int index )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  if( _jobs.size() <= index )
    return 0x0;

  return _jobs.at( index );
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the sequence to the first step
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::firstSequenceStep()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _sequence.current = 0;
  this->_handleSequenceEvent();
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the sequence to the next step
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::nextSequenceStep()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  if( _sequence.current < _sequence.steps.size() - 1 )
  {
    _sequence.current ++;
  }
  else
  {
    _sequence.current = 0;
  }
  this->_handleSequenceEvent();
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the sequence to the previous step
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::prevSequenceStep()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  if( _sequence.current > 0  )
  {
    _sequence.current --;
  }
  else
  {
    _sequence.current = _sequence.steps.size() - 1;
  }
  this->_handleSequenceEvent();
}


///////////////////////////////////////////////////////////////////////////////
//
// Handle sequence events
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_handleSequenceEvent()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  unsigned int index = _sequence.current;
  MpdDefinitions::MpdSequenceStep step = _sequence.steps.at( index );

  // Change location to the current step if we should do so
  if( true == step.changeLocation )
  {
    this->setAnimationPath( step.locationName );
  }

  // Hide all groups then show the current step if we should do so

  for( unsigned int modelIndex = 0; modelIndex < _mpdModels.models->getNumChildren(); ++modelIndex )
  {
    _mpdModels.models->setValue( modelIndex, false );
  }
  unsigned int sIndex = _sequence.current;
  for( std::map< std::string, bool >::iterator iter =  _sequence.steps.at( sIndex ).visibleModelsMap.begin();
                                               iter != _sequence.steps.at( sIndex ).visibleModelsMap.end();
                                               ++iter )
  {
    std::string modelName = (*iter).first;
    bool value            = (*iter).second;
    unsigned int mIndex   = _mpdModels.modelMap[modelName];
    _mpdModels.models->setValue( mIndex, value );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
// Create a proxy geometry to display when no dynamic steps are loaded
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ModelPresentationDocument::_createProxyGeometry( const std::string &message, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  GroupPtr group ( new osg::Group );


  //group->addChild( mt.get() );
  return group.release();

}


///////////////////////////////////////////////////////////////////////////////
//
// Update the status text
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_setStatusText( const std::string message, unsigned int &textXPos, unsigned int &textYPos, double xmult, double ymult, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  ITextMatrix::QueryPtr textMatrix ( caller );
  if( false == textMatrix.valid() )
    throw std::runtime_error ( "Error 3793514250: Failed to find a valid interface to Usul::Interfaces::ITextMatrix " );

  IViewport::QueryPtr viewPort( caller );
  if( false == viewPort.valid() )
    throw std::runtime_error ( "Error 2482359443: Failed to find a valid interface to Usul::Interfaces::IViewport " );

  double xpos = 0, ypos = 0;
  textMatrix->removeText( static_cast< unsigned int > ( textXPos ),
                          static_cast< unsigned int > ( textYPos ) );
   

  xpos = floor( viewPort->width() * xmult );
  ypos = floor( viewPort->height() * ymult );

#if 0
  osg::Vec4f fcolor (  0.841, 0.763, 0.371, 1 );
  osg::Vec4f bcolor (  0.841, 0.763, 0.371, 1 );
#else
  osg::Vec4f fcolor (  1.0, 1.0, 1.0, 1 );
  osg::Vec4f bcolor (  0.0, 0.0, 0.0, 1 );
#endif

  textMatrix->setText( static_cast< unsigned int > ( xpos ), 
                       static_cast< unsigned int > ( ypos ), 
                       message, 
                       fcolor, bcolor );
  textXPos = xpos;
  textYPos = ypos;
  

}


///////////////////////////////////////////////////////////////////////////////
//
// Check timesteps?
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDocument::_checkTime()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  return _checkTimeStatus;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set to check time steps
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::_checkTime( bool value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _checkTimeStatus = value;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set to check time steps
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3d ModelPresentationDocument::_getPosition( const std::string &position, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  osg::Vec3d pos ( 0.0, 0.0, 0.0 );

  IViewport::QueryPtr viewPort( caller );

  if( false == viewPort.valid() )
    throw std::runtime_error ( "Error 2482359443: Failed to find a valid interface to Usul::Interfaces::IViewport " );

  double xpos = 0, ypos = 0;   

  if( "ul" == position || "UL" == position )
  {
    xpos = floor( viewPort->width() * 0.1 );
    ypos = floor( viewPort->height() * 0.1 );
    pos.x() = xpos;
    pos.y() = ypos;
  }

  return pos;
}


// Model Functions

///////////////////////////////////////////////////////////////////////////////
//
//  Add a model
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::addModel( const std::string &name, const std::string &path )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 
  _writer->addModel( name, path );
}


///////////////////////////////////////////////////////////////////////////////
//
// Create a set.
//
///////////////////////////////////////////////////////////////////////////////

// Set functions
void ModelPresentationDocument::addSet( const std::string &name, const std::string &menuName )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _writer->addSet( name, menuName );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a group to a set
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::addModelToSet( const std::string &modelName, const std::string &setName, const std::string &groupName )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _writer->addModelToSet( modelName, setName, groupName );
}


///////////////////////////////////////////////////////////////////////////////
//
// Add a model to a group of a set.  Create the set if it doesn't exist.  
// Create the group if that doesn't exist.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::addGroupToSet( const std::string &setName, const std::string &groupName )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _writer->addGroupToSet( setName, groupName );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a time set.
//
///////////////////////////////////////////////////////////////////////////////

// TimeSet functions
void ModelPresentationDocument::addTimeSet( const std::string &name, const std::string &menuName, unsigned int endTime )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

//  _writer->addTimeSet( name, menuName, endTime );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a model to a time set.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::addModelsToTimeSet( std::vector< std::string > modelList, const std::string &timeSetName, unsigned int startTime, unsigned int endTime )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _writer->addModelsToTimeSet( modelList, timeSetName, startTime, endTime );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a sequence.
//
///////////////////////////////////////////////////////////////////////////////

// Sequence Functions
void ModelPresentationDocument::addSequence( const std::string &name, const std::string &menuName )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _writer->addSequence( name, menuName );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a step to a sequence.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::addStepToSequence( const std::string &locationName, std::vector< std::string > modelList )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _writer->addStepToSequence( locationName, modelList );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a location from a string containing matrix information.
//
///////////////////////////////////////////////////////////////////////////////

// Location Functions
void ModelPresentationDocument::addLocation( const std::string &name, const std::string &location )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _writer->addLocation( name, location );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a location from an osg matrix.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::addLocation( const std::string &name, osg::Matrix location )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _writer->addLocation( name, location );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a dynamic set.
//
///////////////////////////////////////////////////////////////////////////////

// DynamicSet Functions
void ModelPresentationDocument::addDynamicSet( const std::string &name, const std::string &menuName, const std::string &prefix,
                                               const std::string &extension, const std::string &directory, unsigned int maxFiles )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _writer->addDynamicSet( name, menuName, prefix, extension, directory, maxFiles );
}


///////////////////////////////////////////////////////////////////////////////
//
// Create the xml string to for the mpd file
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::buildXMLString()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

 _writer->buildXMLString();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the mpd file.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::write() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _writer->write();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the mpd file to filename <filename>.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::write( const std::string &filename ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _writer->write( filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the animation speed to <speed>
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDocument::setAnimationSpeed( unsigned int speed )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 
  _animationSpeed = speed;
  _update = UpdatePolicyPtr( new UpdatePolicy( speed ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the animation speed
//
///////////////////////////////////////////////////////////////////////////////

unsigned int ModelPresentationDocument::getAnimationSpeed()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 
  return _animationSpeed;
}
