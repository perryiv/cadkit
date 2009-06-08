
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_WIDGETS_AXES_H__
#define __OSG_TOOLS_WIDGETS_AXES_H__

#include "OsgTools/Export.h"

#include "osg/MatrixTransform"
#include "osg/NodeCallback"
#include "osgText/Text"

namespace OsgTools {
namespace Widgets {

class OSG_TOOLS_EXPORT Axes : public osg::MatrixTransform
{
public:
  typedef osg::MatrixTransform BaseClass;
  
  Axes();

  /// Get/Set the text for the x label.
  void                  xText ( const std::string& );
  std::string           xText() const;

  /// Get/Set the text for the y label.
  void                  yText ( const std::string& );
  std::string           yText() const;

  /// Get/Set the text for the z label.
  void                  zText ( const std::string& );
  std::string           zText() const;

protected:

  /// Use reference counting.
  virtual ~Axes();

  /// Build an osgText::Text.
  osgText::Text*        _buildText( const osg::Vec4& color, const osg::Vec3& position, const std::string& text, double size ) const;

  class AxesSort : public osg::NodeCallback
  {
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );
  };

private:
  osg::ref_ptr < osgText::Text > _xText;
  osg::ref_ptr < osgText::Text > _yText;
  osg::ref_ptr < osgText::Text > _zText;
};

}
}

#endif // __OSG_TOOLS_WIDGETS_AXES_H__

