
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

  // Set the time step duration
  void setDuration( float speed );

  // Show we show past events.
  void accumulate ( bool b );

  // Should there be a time window.
  void timeWindow ( bool b );

  // How long sould the time window be?
  void numDays ( unsigned int num );

  // Add a date.
  void addDate ( const OsgTools::Animate::Date& date, osg::Node* node );

  // Traverse the node.
  virtual void traverse( osg::NodeVisitor& nv );

  // Set the Text to print to.
  void setText ( osgText::Text *text );

protected:
  virtual ~DateGroup();

private:

  typedef std::pair < OsgTools::Animate::Date, osg::ref_ptr< osg::Node > > DatePair;
  typedef std::vector < DatePair > DateNodes;

  DateNodes _nodes;

  float _speed;

  double _lastTime;
  
  OsgTools::Animate::Date _lastDate;
  OsgTools::Animate::Date _minDate;
  OsgTools::Animate::Date _maxDate;

  bool _accumulate;
  bool _timeWindow;
  bool _needToSort;

  unsigned int _numDays;

  osg::ref_ptr< osgText::Text > _text;
};

}
}

#endif // __OSGTOOLS_ANIMATE_DATE_GROUP_H__

