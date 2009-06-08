
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Set the display-list usage.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_SET_DISPLAY_LIST_USAGE_H_
#define _OSG_TOOLS_SET_DISPLAY_LIST_USAGE_H_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"


namespace OsgTools {


class OSG_TOOLS_EXPORT DisplayLists
{
public:

  DisplayLists ( bool use );
  ~DisplayLists();
  void operator () ( osg::Node * );

protected:

  void          _set ( osg::Geode * );

private:

  DisplayLists ( const DisplayLists & );
  DisplayLists &operator = ( const DisplayLists & );

  bool _use;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_SET_DISPLAY_LIST_USAGE_H_
