
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODELPRESENTATION_STRUCTS_H__
#define __MODELPRESENTATION_STRUCTS_H__

#include "Usul/Commands/Command.h"
#include "Usul/Documents/Document.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"

#include "osg/Group"
#include "osg/Matrixd"
#include "osg/Switch"

#include <map>
#include <string>
#include <memory>


//namespace osg { class Node; }

class MpdDefinitions 
{
public:
  enum Action
  {
    SHOW,
    HIDE,
    DO_NOTHING
  };
  typedef std::pair< std::string, osg::Matrixd > Location;
  typedef std::map< std::string, osg::Matrixd > Locations;
  typedef std::vector< std::string > LocationNames;


  struct MpdGroup
  {
    unsigned int setIndex;
    std::string name;
    std::vector< bool > visibleModels;
    std::map< std::string, bool > visibleModelsMap;
    MpdGroup() : setIndex( 0 ), name( "Unknown" ), visibleModels(), visibleModelsMap() {}
  };
  struct MpdSet
  {
    unsigned int index;
    bool visible;
    std::string type;
    std::string name;
    std::string menuName;
    std::vector < MpdGroup > groups;
    std::vector< std::string > groupNames;

    MpdSet() : index( 0 ), visible( true ), type( "radio" ), name ( "Set" ), menuName ( "Models" ), groups(), groupNames(){}
  };
  struct MpdModels
  {
    std::map< std::string, unsigned int > modelMap;
    osg::ref_ptr< osg::Switch > models;
  };

  struct MpdTimeGroup
  {
    unsigned int startTime;
    unsigned int endTime;
    std::vector< bool > visibleModels;
    std::map< std::string, bool > visibleModelsMap;

    MpdTimeGroup() : startTime( 0 ), endTime( 0 ), visibleModels(), visibleModelsMap() {}
  };
  struct MpdTimeSet
  {
    osg::ref_ptr< osg::Switch > timeline;
    unsigned int currentTime;
    unsigned int endTime;
    std::vector< MpdTimeGroup > groups;
    bool visible;
    std::string name;
    std::string menuName;

    MpdTimeSet() : timeline( new osg::Switch ), currentTime( 0 ), endTime( 0 ), groups(), visible( false ),
      name( "Unknown" ), menuName( "Timelines" ) {}

  };
  struct MpdDynamicGroup
  {
    std::string filename;
    bool valid;
    bool loaded;

    MpdDynamicGroup() : filename( "" ), valid( false ), loaded( false ) {}
  };
  struct MpdDynamicSetHeader
  {
    std::string directory;
    std::string extension;
    std::string prefix;
    std::vector< std::string > modelNames;

    MpdDynamicSetHeader() : directory( "" ), extension( "" ), prefix( "" ), modelNames() {}
    
  };
  struct MpdDynamicSet
  {
    MpdDynamicSetHeader header;
    osg::ref_ptr< osg::Switch > models;
    std::vector< MpdDynamicGroup > groups;
    unsigned int currentTime;
    unsigned int endTime;
    unsigned int nextIndexToLoad;
    std::string menuName;
    std::string name;
    bool visible;  
    unsigned int maxFilesToLoad;
    bool absolute;
    std::string position;

    MpdDynamicSet() : header(),  models( new osg::Switch ), groups(), currentTime( 0 ),
                      endTime( 0 ), nextIndexToLoad( 0 ), menuName( "DynamicSets" ),
                      name( "Unknown" ), visible( false ), maxFilesToLoad( 1 ),
                      absolute( false ), position( "" ) {}
  };
  struct MpdSequenceStep
  {
    std::string locationName;
    std::string name;
    bool overwriteGroup;
    bool changeLocation;
    std::vector< bool > visibleGroups;
    std::map< std::string, bool > visibleModelsMap;
    
    MpdSequenceStep() : locationName( "" ), name( "Unknown" ), overwriteGroup( true ), changeLocation( true ),
      visibleGroups(), visibleModelsMap() {}
  };
  struct MpdSequence
  {
    std::vector< MpdSequenceStep > steps;
    osg::ref_ptr< osg::Switch > groups;
    std::string name;
    std::string menuName;
    unsigned int current;

    MpdSequence() : steps(), groups( new osg::Switch ), name( "Unknown" ),
                    menuName( "Sequence" ), current( 0 ) {}

  };


  typedef std::vector< MpdDefinitions::MpdDynamicSet > MpdDynamicSets;

  typedef osg::ref_ptr< osg::Group > Group;
  typedef std::vector< Group > Groups;

};


#endif // __MODELPRESENTATION_STRUCTS_H__
