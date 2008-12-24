
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_DELETE_HANDLER_H__
#define __OSG_TOOLS_DELETE_HANDLER_H__

#include "OsgTools/Export.h"

#include "osg/DeleteHandler"

#include <string>

namespace OsgTools {
  namespace Utilities {

class OSG_TOOLS_EXPORT DeleteHandler : public osg::DeleteHandler
{
public:
  typedef osg::DeleteHandler BaseClass;

  DeleteHandler();
  virtual ~DeleteHandler();
  
  virtual void flush();
  virtual void flushAll();

  unsigned int numObjects();

protected:
  static std::string _name ( const osg::Referenced* );
};

  }
}


#endif // __OSG_TOOLS_DELETE_HANDLER_H__
