
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

#include <string>
#include <memory>

#include "osg/Group"
#include "osg/Matrixd"

//namespace osg { class Node; }

class MpdDefinitions 
  {
    public:
      typedef std::pair< std::string, osg::Matrixd > Location;
      typedef std::map< std::string, osg::Matrixd > Locations;
      typedef std::vector< std::string > LocationNames;

      struct MpdSet
      {
        unsigned int index;
        std::string name;
        std::string menuName;
        std::vector< std::string > groupNames;
      };
      struct MpdGroup
      {
        unsigned int setIndex;
        osg::ref_ptr< osg::Group > group;
      };
      struct MpdModel
      {
         unsigned int setIndex;
         unsigned int groupIndex;
         osg::ref_ptr< osg::Group > model;
      };
      struct MpdTimeGroup
      {
        unsigned int startTime;
        unsigned int endTime;
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
        Location location;
        std::string name;
        bool overwriteGroup;
        bool changeLocation;
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
