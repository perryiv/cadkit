
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODELPRESENTATION_WRITER_H__
#define __MODELPRESENTATION_WRITER_H__

#include "MpdDefinitions.h"

#include "Usul/Commands/Command.h"
#include "Usul/Documents/Document.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/File/Slash.h"
#include "Usul/Documents/Manager.h"

#include "osg/Matrix"


#include <string>

namespace osg { class Node; }

class MpdWriter : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object BaseClass;

  USUL_DECLARE_REF_POINTERS ( MpdWriter );

  typedef Usul::Documents::Document::RefPtr DocumentPtr;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Documents::Manager DocManager;
  typedef DocManager::DocumentInfo Info;
  
  typedef std::pair< std::string, std::string > Model;
  typedef std::vector< Model > Models;

  typedef std::vector< std::string > ShowList;
  typedef std::pair< std::string, ShowList > Group;
  typedef std::map< std::string, Group > Groups;
  struct Set
  {
    Groups groups;
    std::string name;
    std::string menuName;

    Set() : groups(), name ( "" ), menuName( "" ){}
    Set( std::string n, std::string mn ) : groups(), name ( n ), menuName( mn ){}

  };
  typedef std::map< std::string, Set > Sets;

  struct TimeGroup
  {
    std::string startTime;
    std::string endTime;
    std::string name;
    ShowList shown;
    TimeGroup() : startTime( "" ), endTime( "" ), name( "" ), shown() {}
    TimeGroup( std::string st, std::string et, std::string n ) 
      : startTime( st ), endTime( et ), name( n ), shown() {}

  };
  typedef std::vector< TimeGroup > TimeGroups;

  struct TimeSet
  {
    TimeGroups group;
    std::string name;
    std::string menuName;
    std::string endTime;

    TimeSet() : group(), name( "" ), menuName( "" ), endTime( "" ) {}
    TimeSet( std::string n, std::string mn, std::string et ) 
      : group(), name( n ), menuName( mn ), endTime( et ) {}
  };
  typedef std::map< std::string, TimeSet > TimeSets;

  struct DynamicSet
  {
    std::string name;
    std::string menuName;
    std::string prefix;
    std::string extension;
    std::string directory;
    std::string maxFiles;

    DynamicSet() : 
      name( "" ),
      menuName( "" ),
      prefix( "" ),
      extension( "" ),
      directory( "" ),
      maxFiles( "" ) {}

      DynamicSet( std::string n, std::string mn, std::string p, std::string e, std::string d, std::string max) : 
      name( n ),
      menuName( mn ),
      prefix( p ),
      extension( e ),
      directory( d ),
      maxFiles( max ) {}
  };
  typedef std::vector< DynamicSet > DynamicSets;

  struct SequenceStep
  {
    std::string locationName;
    std::vector< std::string > modelList;

    SequenceStep() : locationName( "" ), modelList() {}
    SequenceStep( std::string ln ) : locationName( ln ), modelList() {}
  };
  typedef std::vector < SequenceStep > SequenceSteps;
  struct Sequence
  {
    std::string name;
    std::string menuName;
    SequenceSteps steps;

    Sequence() : name( "" ), menuName( "" ), steps() {}
    Sequence( std::string n, std::string mn ) 
      : name( n ), menuName( mn ), steps() {}
  };

  typedef std::pair< std::string, std::string > Location;
  typedef std::map< std::string, Location > Locations;


  MpdWriter ( const std::string &filename  );
  
  // Model Functions
  void                    addModel( const std::string &name, const std::string &path );

  // Set functions
  void                    addSet( const std::string &name, const std::string &menuName );
  void                    addModelToSet( const std::string &modelName,
                                   const std::string &setName,
                                   const std::string &groupName );

  void                    addGroupToSet( const std::string &setName,
                                         const std::string &groupName );

  // TimeSet functions
  void                    addTimeSet( const std::string &name, const std::string &menuName, unsigned int endTime );
  void                    addTimeSet( TimeSet timeset );
  void                    addModelsToTimeSet( std::vector< std::string > modelList,
                                             const std::string &timeSetName,
                                             unsigned int startTime,
                                             unsigned int endTime );

  // Sequence Functions
  void                    addSequence( const std::string &name, const std::string &menuName );
  void                    addStepToSequence( const std::string &locationName, std::vector< std::string > modelList );

  // Location Functions
  void                    addLocation( const std::string &name, const std::string &location );
  void                    addLocation( const std::string &name, osg::Matrix location );

  // DynamicSet Functions
  void                    addDynamicSet( const std::string &name,
                                         const std::string &menuName,
                                         const std::string &prefix,
                                         const std::string &extension,
                                         const std::string &directory,
                                         unsigned int maxFiles );

  


  void                    buildXMLString();

  void                    write() const;
  void                    write( const std::string &filename ) const;

 

protected:
  ~MpdWriter();

  void                    _writeHeader();
  void                    _writeFooter();
  void                    _writeModels();
  void                    _writeSets();
  void                    _writeTimeSets();
  void                    _writeDynamicSets();
  void                    _writeLocations();
  void                    _writeSequence();
  

private:

  Models                  _models;
  Sets                    _sets;
  TimeSets                _timeSets;
  DynamicSets             _dynamicSets;
  Locations               _locations;
  Sequence                _sequence;

  std::string             _mpdFile;
  std::string             _filename;
  
};


#endif // __MODELPRESENTATION_WRITER_H__
