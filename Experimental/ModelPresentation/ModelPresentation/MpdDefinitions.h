
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

#include "osg/Group"

//namespace osg { class Node; }

class MpdDefinitions 
  {
    public:

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
  };
  struct MpdDynamicSet
  {
    osg::ref_ptr< osg::Switch > models;
    std::vector< MpdDynamicGroup > groups;
    std::vector< std::string > modelNames;
    unsigned int currentTime;
    unsigned int endTime;
    std::string menuName;
    std::string name;
    std::string directory;
    std::string extension;
    std::string prefix;
    bool visible;    
  };

  typedef std::vector< MpdDefinitions::MpdDynamicSet > MpdDynamicSets;
  
  };


#endif // __MODELPRESENTATION_STRUCTS_H__
