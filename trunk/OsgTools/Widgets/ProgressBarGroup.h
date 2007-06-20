///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROGRESSBARGROUP_H
#define PROGRESSBARGROUP_H

#include "osg/Group"
#include <string>
#include "OsgTools/Widgets/ProgressBarGroup.h"
#include "OsgTools/Widgets/ProgressBar.h"
#include "OsgTools/Export.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

namespace OsgTools {
namespace Widgets {

  class OSG_TOOLS_EXPORT ProgressBarGroup : public Usul::Base::Referenced
{
public:
  USUL_DECLARE_REF_POINTERS ( ProgressBarGroup );
	ProgressBarGroup();
	
  ///////////////////
  //inline functions
  ///////////////////

  // Get the position of the progress bar group
  const osg::Vec3f & getPosition() { return _pos; }

  // Get the bar message on the slave bar
  const std::string& getMessage() { return _slave->getMessage(); }

  // Return true if the bar group is rendering relative to absolute
  bool isRelativeToAbsolute() { return _isRelativeToAbsolute; }

  // Return true if the slave is 100% complete
  bool isCurrentItemFinished() { return _slave->isFinished(); }

  // Return the number of items ( _max ) on the master bar
  int getNumItems() { return _master->getMax(); }

  // Get the current item on the master bar
  int getCurrentItem() { return _master->getCurrent(); }

  // Return the length of the bar group
  float getLength() { return _length; }

  // Return the min value on the slave bar
  double getCurrentItemMin() { return _slave->getMin(); }

  // Return the max value on the slave bar
  double getCurrentItemMax() { return _slave->getMax(); }

  // Return the current value on the slave bar
  double getCurrentItemValue() { return _slave->getCurrent(); }


  osg::Node* getProgressBarGroup();
  
  void setRelativeToAbsolute ( bool value );
  void setPosition ( const osg::Vec3f & );
  void setCurrentItemMin( double min );
  void setCurrentItemMax( double max );
  void setCurrentItemValue( double v );
  void setNumItems ( int n );
  void setCurrentItem ( int i );
  void setMessage ( const std::string& m );
  void updateProgressBarGroup();
  void reset();
	
protected:

  virtual ~ProgressBarGroup();
	void _buildProgressBarGroup();
  void _init();
  std::string _getPercentComplete();
  osg::Node* _buildBar ( int render_level , std::string tex, const osg::Vec2f& ul, const osg::Vec2f& lr, float depth  );
  void _updateMasterMessage();

private:

  bool _isRelativeToAbsolute;
  float _length, _height, _padding;
  double _borderZOffset;// ( -0.0003f ),

  ProgressBar::RefPtr _master;
  ProgressBar::RefPtr _slave;

  osg::ref_ptr< osg::Node > _border;
  osg::ref_ptr< osg::Group > _group;

  osg::Vec3f _pos;
  osg::Vec2f _ll;
  

};

} // Namespace Builders
} // Namespace OsgWidgets


#endif