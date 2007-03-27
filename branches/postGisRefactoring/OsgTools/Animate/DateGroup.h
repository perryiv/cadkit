
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to animate by dates
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_ANIMATE_DATE_GROUP_H__
#define __OSGTOOLS_ANIMATE_DATE_GROUP_H__

#include "OsgTools/Export.h"

#include "OsgTools/Animate/Date.h"
#include "OsgTools/Animate/Settings.h"

#include "osg/ref_ptr"
#include "osg/Group"
#include "osgText/Text"

#include <vector>

namespace OsgTools {
namespace Animate {

class OSG_TOOLS_EXPORT DateGroup : public osg::Group
{
public:
  typedef osg::Group BaseClass;

  DateGroup();

  /// Set the time step duration
  void                                 animationSpeed ( float speed );

  /// Get the time step duration
  float                                animationSpeed () const;

  // Add a date.
  void                                 updateMinMax ( const OsgTools::Animate::Date& first, const OsgTools::Animate::Date& last );

  // Traverse the node.
  virtual void                         traverse( osg::NodeVisitor& nv );

  // Set the Text to print to.
  void                                 setText ( osgText::Text *text );

  /// Get/Set settings.
  void                                 settings( OsgTools::Animate::Settings* );
  OsgTools::Animate::Settings*         settings();
  const OsgTools::Animate::Settings*   settings() const;

protected:
  virtual ~DateGroup();

private:

  float _animationSpeed;

  double _lastTime;
  
  OsgTools::Animate::Date _lastDate;
  OsgTools::Animate::Date _minDate;
  OsgTools::Animate::Date _maxDate;

  OsgTools::Animate::Settings::RefPtr  _settings;

  osg::ref_ptr< osgText::Text > _text;
};

}
}

#endif // __OSGTOOLS_ANIMATE_DATE_GROUP_H__

