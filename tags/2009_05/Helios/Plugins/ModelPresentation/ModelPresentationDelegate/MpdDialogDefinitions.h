
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODELPRESENTATION_DIALOG_DEFINITIONS_H__
#define __MODELPRESENTATION_DIALOG_DEFINITIONS_H__

#include <string>
#include <memory>
#include <utility>
#include <vector>

struct MpdDialogDefinitions 
{ 
    typedef std::pair< std::string, std::string > Model;
    typedef std::vector< Model > ModelList;

    typedef std::pair< std::string, ModelList > Group;
    typedef std::vector< Group > GroupList;

    struct Set
    {
      std::string name;
      std::string menuName;
      GroupList groups;
    };
    typedef std::vector< Set > SetList;
};


#endif // __MODELPRESENTATION_DIALOG_DEFINITIONS_H__
