
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class to record time.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_RECORD_TIME_H_
#define _OPEN_SCENE_GRAPH_TOOLS_RECORD_TIME_H_

#include <string>


namespace OsgTools {
namespace Render {


template < class ViewerType > struct RecordTime
{
  // Constructor adds start time.
  RecordTime ( ViewerType &v, const std::string &name ) : _v ( v ), _name ( name )
  {
    _v.timeStart ( _name );
  }

  // Destructor adds stop time.
  ~RecordTime()
  {
    _v.timeStop ( _name );
  }

private:

  // No copying or assignment.
  RecordTime ( const RecordTime & );
  RecordTime &operator = ( const RecordTime & );

  ViewerType &_v;
  std::string _name;
};


} // namespace Render
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_RECORD_TIME_H_
