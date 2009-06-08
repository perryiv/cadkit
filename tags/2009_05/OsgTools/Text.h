
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to make text.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_TEXT_BUILDER_H_
#define _OSG_TOOLS_TEXT_BUILDER_H_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

#include "osg/Vec4"
#include "osg/Vec3"

#include <string>


namespace OsgTools {


class OSG_TOOLS_EXPORT Text
{
public:

  Text();
  ~Text();

  void                color ( const osg::Vec4 &c ) { _color = c; }
  const osg::Vec4 &   color() const { return _color; }

  void                position ( const osg::Vec3 &p );
  const osg::Vec3 &   position() const;

  void                font ( const std::string &f );
  std::string         font() const;

  float               height() const;
  void                height ( float h );

  void                text ( const std::string &t );
  std::string         text() const;

  bool                equal ( const std::string & ) const;

  osg::Node *         operator()() const;

private:

  osg::Vec4      _color;
  std::string    _font;
  osgText::Text *_text;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_TEXT_BUILDER_H_
