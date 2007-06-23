
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to draw text with optional polygon background and anchor.
//  Not working yet.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_BUILDERS_SIGN_H__
#define __OSG_TOOLS_BUILDERS_SIGN_H__

#include "OsgTools/Export.h"

#include "osg/Vec3"

#include <string>

namespace osg { class Node; }

namespace OsgTools {
namespace Builders {

class OSG_TOOLS_EXPORT Sign
{
public:
  Sign();
  ~Sign();

  osg::Node*  operator() ();

protected:
  osg::Node*  _buildText();
  osg::Node*  _buildSign();
  osg::Node*  _buildPost();
  osg::Node*  _buildAnchor();

private:
  std::string _text;

  osg::Vec3 _start;
  osg::Vec3 _end;

  bool _drawAnchor;
  bool _drawPost;
  bool _drawSign;
};


}
}

#endif // __OSG_TOOLS_BUILDERS_SIGN_H__
