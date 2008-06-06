
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "MpdWriter.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Components/Manager.h"
#include "Usul/System/Host.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Functions.h"
#include "Usul/Math/MinMax.h"
#include "Usul/System/Directory.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Strings/Format.h"

#include "Usul/Interfaces/IMemoryPool.h"
#include "Usul/Interfaces/IMpdNavigator.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "osg/Switch"

#include <fstream>

//USUL_IMPLEMENT_COMMAND ( MpdWriter );


///////////////////////////////////////////////////////////////////////////////
//
//  MpdWriter Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdWriter::MpdWriter (  const std::string &filename ) :
  BaseClass (),
  _models(),
  _sets(),
  _timeSets(),
  _dynamicSets(),
  _locations(),
  _sequence(),
  _mpdFile( "" ),
  _filename( filename )
{
  USUL_TRACE_SCOPE;
}



///////////////////////////////////////////////////////////////////////////////
//
//  MpdWriter Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdWriter::MpdWriter () :
  BaseClass (),
  _models(),
  _sets(),
  _timeSets(),
  _dynamicSets(),
  _locations(),
  _sequence(),
  _mpdFile( "" ),
  _filename()
{
  USUL_TRACE_SCOPE;
}  
///////////////////////////////////////////////////////////////////////////////
//
//  MpdWriter Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdWriter::~MpdWriter()
{
  USUL_TRACE_SCOPE;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a model
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::addModel( const std::string &name, const std::string &path )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  

  Model model( name, path );

  _models.push_back( model );
}



///////////////////////////////////////////////////////////////////////////////
//
// Create a set.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::addSet( const std::string &name, const std::string &menuName )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  
  Set set( name, menuName );

  _sets[ name ] = set;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a group to a set
//
///////////////////////////////////////////////////////////////////////////////


void MpdWriter::addGroupToSet( const std::string &setName, const std::string &groupName )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this ); 

  // check to see if the set setName exists.  If not create the set
  // and add the group to the set.
  Sets::iterator iter = _sets.find( setName );
  if( _sets.end() == iter )
    this->addSet( setName, "Models" );

  Group group;
  group.first = groupName;
  group.second.resize( 0 );

  _sets[ setName ].groups[ groupName ] = group;
  
}


///////////////////////////////////////////////////////////////////////////////
//
// Add a model to a group of a set.  Create the set if it doesn't exist.  
// Create the group if that doesn't exist.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::addModelToSet( const std::string &modelName, const std::string &setName, const std::string &groupName )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  

  // check to see if the group groupName exists in the set setName
  // if not add the group.  

  // The group will also check to see if the set exists and create it
  // if it doesn't

  Sets::iterator set = _sets.find( setName );
  if( _sets.end() == set )
    this->addSet( setName, "Models" );

  Groups::iterator i = _sets[ setName ].groups.find( groupName );
  if( _sets[ setName ].groups.end() == i )
    this->addGroupToSet( setName, groupName );
   
  _sets[ setName ].groups[ groupName ].second.push_back( modelName );
 
  

}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a time set.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::addTimeSet( const std::string &name, const std::string &menuName, unsigned int endTime )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  
  std::string et = Usul::Convert::Type< unsigned int, std::string >::convert( endTime );
  TimeSet set( name, menuName, et );

  _timeSets[ name ] = set;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a pre-created time set.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::addTimeSet( TimeSet timeset )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  
  std::string name = timeset.name;

  _timeSets[ name ] = timeset;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a model to a time set.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::addModelsToTimeSet( std::vector< std::string > modelList, const std::string &timeSetName,
                                   unsigned int startTime, unsigned int endTime )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );


  TimeGroup group;
  
  group.startTime = Usul::Convert::Type< unsigned int, std::string >::convert( startTime );
  group.endTime = Usul::Convert::Type< unsigned int, std::string >::convert( endTime );
  group.shown = modelList;

  TimeSets::iterator iter = _timeSets.find( timeSetName );
  if( _timeSets.end() != iter )
  {
    _timeSets[ timeSetName ].group.push_back( group );
  }
  else
  {
    std::cout << "Time set ( " << timeSetName << " ) not found!" << std::endl;
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a sequence.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::addSequence( const std::string &name, const std::string &menuName )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  

  _sequence.name = name;
  _sequence.menuName = menuName;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a step to a sequence.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::addStepToSequence( const std::string &locationName, std::vector< std::string > modelList )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  

  SequenceStep step( locationName );
  step.modelList = modelList;

  _sequence.steps.push_back( step );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a location from a string containing matrix information.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::addLocation( const std::string &name, const std::string &location )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  

  Location loc;
  loc.first = name;
  loc.second = location;

  _locations[ name ] = loc;
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a location from an osg matrix.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::addLocation( const std::string &name, osg::Matrix location )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  

  osg::Matrix m = location;
  
  std::string r0 = Usul::Strings::format( m(0,0), ",", m(1,0), ",", m(2,0), ",", m(3,0) );
  std::string r1 = Usul::Strings::format( m(0,1), ",", m(1,1), ",", m(2,1), ",", m(3,1) );
  std::string r2 = Usul::Strings::format( m(0,2), ",", m(1,2), ",", m(2,2), ",", m(3,2) );
  std::string r3 = Usul::Strings::format( m(0,3), ",", m(1,3), ",", m(2,3), ",", m(3,3) );

  std::string matrix = Usul::Strings::format( r0, ",", r1, ",", r2, ",", r3 );

  this->addLocation( name, matrix );

}



///////////////////////////////////////////////////////////////////////////////
//
//  Add a dynamic set.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::addDynamicSet( const std::string &name, const std::string &menuName, const std::string &prefix,
                               const std::string &extension, const std::string &directory, unsigned int maxFiles )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  

  std::string max = Usul::Convert::Type< unsigned int, std::string >::convert( maxFiles );
  DynamicSet dset( name, menuName, prefix, extension, directory, max );
  _dynamicSets.push_back( dset );

}


///////////////////////////////////////////////////////////////////////////////
//
// Create the xml string to for the mpd file
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::buildXMLString()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  

  _mpdFile = "";

  this->_writeHeader();
  this->_writeModels();
  this->_writeLocations();
  this->_writeSets();
  this->_writeTimeSets();
  this->_writeSequence();
  this->_writeDynamicSets();
  this->_writeFooter();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the mpd file.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::write() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  

  this->write( _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the mpd file to filename <filename>.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::write( const std::string &filename ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );  

  std::ofstream ofs;
  ofs.open( filename.c_str() );
  if( !ofs )
  {
    // TODO: throw an exception instead
    std::cout << "Could not open file: \"" << filename << "\" for writing!" << std::endl;
    ofs.close();
    return;
  }


  const std::string xml = _mpdFile;
  ofs << xml;
  ofs.close();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the header information.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::_writeHeader()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _mpdFile = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n <mpd usetools=\"true\" >\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the footer information.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::_writeFooter()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _mpdFile += "</mpd>";

}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the models.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::_writeModels()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  _mpdFile += "  <models>\n";
  for( unsigned int i = 0; i < _models.size(); ++i )
  {
    std::string tag ( "" );
    tag += "    <model ";
    tag += "name=\"" + _models.at( i ).first + "\" ";
    tag += "file=\"" + _models.at( i ).second + "\" ";
    tag += "/>\n";

    _mpdFile += tag;
    
  }
  _mpdFile += "  </models>\n\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the sets.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::_writeSets()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // parse sets
  for( Sets::iterator i = _sets.begin(); i != _sets.end(); ++i )
  {
     _mpdFile = Usul::Strings::format( _mpdFile, "  <set name=\"" , (*i).second.name , "\" menu_name=\"" , (*i).second.menuName , "\" >\n" );

    // parse groups in the set
    for( Groups::iterator j = (*i).second.groups.begin(); j != (*i).second.groups.end(); ++j )
    { 
      _mpdFile += "    <group name=\"" + (*j).second.first + "\" >\n";

      // parse models to be show in the group
      for( unsigned int k = 0; k < (*j).second.second.size(); ++k )
      {
        _mpdFile += "      ";
        _mpdFile += "<show name=\"" + (*j).second.second.at( k ) + "\" />\n";
      }
      _mpdFile += "    </group>\n";

    }
    _mpdFile += "  </set>\n";

  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the time sets.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::_writeTimeSets()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  for( TimeSets::iterator i = _timeSets.begin(); i != _timeSets.end(); ++i )
  {
    _mpdFile += "  <timeset name=\"" + (*i).second.name + "\" menu_name=\"" + (*i).second.menuName + "\" >\n";

    for( unsigned int j = 0; j < (*i).second.group.size(); ++j )
    {
      _mpdFile += "    ";
      _mpdFile += "<group startTime=\"" + (*i).second.group.at( j ).startTime + "\" endTime=\"" + (*i).second.group.at( j ).endTime + "\" >\n";
      for( unsigned int k = 0; k < (*i).second.group.at( j ).shown.size(); ++k )
      {
        _mpdFile += "      ";
        _mpdFile += "<show name=\"" + (*i).second.group.at( j ).shown.at( k ) + "\" />\n";
      }
      _mpdFile += "    </group>\n";
    }
    _mpdFile += "  </timeset>\n\n";
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the dynamic sets.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::_writeDynamicSets()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  for( DynamicSets::iterator i = _dynamicSets.begin(); i != _dynamicSets.end(); ++i )
  {
    _mpdFile += "  ";
    _mpdFile += "<dynamic\n";
    _mpdFile += "    name=\"" + (*i).name + "\" \n";
    _mpdFile += "    menuName=\"" + (*i).menuName + "\" \n";
    _mpdFile += "    prefix=\"" + (*i).prefix + "\" \n";
    _mpdFile += "    extension=\"" + (*i).extension + "\" \n";
    _mpdFile += "    directory=\"" + (*i).directory + "\" \n";
    _mpdFile += "    max=\"" + (*i).maxFiles + "\" \n";
    _mpdFile += " />\n\n";
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the locations.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::_writeLocations()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  for( Locations::iterator i = _locations.begin(); i != _locations.end(); ++i )
  {
    _mpdFile += "  <location name=\"" + (*i).second.first + "\" >\n";
    _mpdFile += "    <matrix type=\"matrix\" value=\"" + (*i).second.second + "\" />\n";
    _mpdFile += "  </location>\n\n";
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the sequence.
//
///////////////////////////////////////////////////////////////////////////////

void MpdWriter::_writeSequence()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  if( _sequence.steps.size() > 0 )
  {
    _mpdFile += "  <sequence name=\"" + _sequence.name + "\" menu_name=\"" + _sequence.menuName + "\" >\n";
    for( unsigned int i = 0; i < _sequence.steps.size(); ++i )
    {
      _mpdFile += "    <step>\n";
      _mpdFile += "      <location name=\"" + _sequence.steps.at( i ).locationName + "\" />\n";
      for( unsigned int j = 0; j < _sequence.steps.at( i ).modelList.size(); ++j )
      {
        _mpdFile += "      ";
        _mpdFile += "<show name=\"" + _sequence.steps.at( i ).modelList.at( j ) + "\" />\n";
      }
      _mpdFile += "    </step>\n";
    }
    _mpdFile += "  </sequence>\n\n";
  }
}

