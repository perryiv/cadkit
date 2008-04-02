
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "ModelPresentationLib.h"
#include "DrtSim/DRTSimReader.h"

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


//USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ModelPresentationLib, ModelPresentationLib::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationLib::ModelPresentationLib() : 
  BaseClass (),
  _writer( new MpdWriter( "output.mpd" ) ),
  _workingDir( "" )
{
  USUL_TRACE_SCOPE;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationLib::~ModelPresentationLib()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationLib::clear ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationLib::read( const std::string &filename, const std::string &type, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  if( "drt" == type )
  {
    DrtSimReader::RefPtr drtReader ( new DrtSimReader() );
    drtReader->read( filename, caller, progress );
  }

}

///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ModelPresentationLib::buildScene ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  osg::ref_ptr< osg::Group > group ( new osg::Group );

  return group.get();
}



///////////////////////////////////////////////////////////////////////////////
//
// Get the current working directory variable
//
///////////////////////////////////////////////////////////////////////////////

std::string ModelPresentationLib::_getWorkingDir()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  return _workingDir;
}



// Model Functions

///////////////////////////////////////////////////////////////////////////////
//
//  Add a model
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationLib::addModel( const std::string &name, const std::string &path )
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
void ModelPresentationLib::addSet( const std::string &name, const std::string &menuName )
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

void ModelPresentationLib::addModelToSet( const std::string &modelName, const std::string &setName, const std::string &groupName )
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

void ModelPresentationLib::addGroupToSet( const std::string &setName, const std::string &groupName )
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
void ModelPresentationLib::addTimeSet( const std::string &name, const std::string &menuName, unsigned int endTime )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _writer->addTimeSet( name, menuName, endTime );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a model to a time set.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationLib::addModelsToTimeSet( std::vector< std::string > modelList, const std::string &timeSetName, unsigned int startTime, unsigned int endTime )
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
void ModelPresentationLib::addSequence( const std::string &name, const std::string &menuName )
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

void ModelPresentationLib::addStepToSequence( const std::string &locationName, std::vector< std::string > modelList )
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
void ModelPresentationLib::addLocation( const std::string &name, const std::string &location )
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

void ModelPresentationLib::addLocation( const std::string &name, osg::Matrix location )
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
void ModelPresentationLib::addDynamicSet( const std::string &name, const std::string &menuName, const std::string &prefix,
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

void ModelPresentationLib::buildXMLString()
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

void ModelPresentationLib::write() const
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

void ModelPresentationLib::write( const std::string &filename ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  _writer->write( filename );
}
