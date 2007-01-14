
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

  void setDuration( float speed ) { _speed = speed; }

  void accumulate ( bool b ) { _accumulate = b; }
  void timeWindow ( bool b ) { _timeWindow = b; }

  void numDays ( unsigned int num ) { _numDays = num; }

  void addDate ( const OsgTools::Animate::Date& date, osg::Node* node );

  virtual void traverse( osg::NodeVisitor& nv );

  void setText ( osgText::Text *text ) { _text = text; }

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

