
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
#include "Usul/Interfaces/IDataSync.h"

#include "osg/Group"
#include "osg/Matrixd"

#include <map>
#include <string>
#include <memory>


//namespace osg { class Node; }

class MpdDefinitions 
  {
    public:
      typedef std::pair< std::string, osg::Matrixd > Location;
      typedef std::map< std::string, osg::Matrixd > Locations;
      typedef std::vector< std::string > LocationNames;


      struct MpdGroup
      {
        unsigned int setIndex;
        std::string name;
        std::vector< bool > visibleModels;
        std::map< std::string, bool > visibleModelsMap;
      };
      struct MpdSet
      {
        unsigned int index;
        std::string name;
        std::string menuName;
        std::vector < MpdGroup > groups;
        std::vector< std::string > groupNames;
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
      };
      struct MpdDynamicGroup
      {
        std::string filename;
        bool valid;
        bool loaded;
      };
      struct MpdDynamicSetHeader
      {
        std::string directory;
        std::string extension;
        std::string prefix;
        std::vector< std::string > modelNames;
        
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
      };
      struct MpdSequenceStep
      {
        std::string locationName;
        std::string name;
        bool overwriteGroup;
        bool changeLocation;
        std::vector< bool > visibleGroups;
        std::map< std::string, bool > visibleModelsMap;
      };
      struct MpdSequence
      {
        std::vector< MpdSequenceStep > steps;
        osg::ref_ptr< osg::Switch > groups;
        std::string name;
        std::string menuName;
        unsigned int current;
      };
  

  typedef std::vector< MpdDefinitions::MpdDynamicSet > MpdDynamicSets;
  
  typedef osg::ref_ptr< osg::Group > Group;
  typedef std::vector< Group > Groups;
  
  };


#endif // __MODELPRESENTATION_STRUCTS_H__
