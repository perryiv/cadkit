
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_IO_STL_WRITER_H__
#define __OSGTOOLS_IO_STL_WRITER_H__

#include "OsgTools/Export.h"

#include <string>

namespace osg { class Node; }

namespace OsgTools {
namespace IO {

class OSG_TOOLS_EXPORT STLWriter
{
public:
  STLWriter ( const std::string& filename );

  void header ( const std::string& header );

  void writeASCII( const osg::Node &node );

  void writeBinary( const osg::Node &node );

private:
  STLWriter();

  osg::Node*  _copyScene ( const osg::Node& node );

  std::string _filename;
  std::string _header;
};

}
}

#endif //__OSGTOOLS_IO_STL_WRITER_H__

