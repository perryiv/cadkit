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
	

  osg::Node* getProgressBarGroup();
  const osg::Vec3f & getPosition() { return _pos; }
  const std::string& getMessage() { return _slaveText; }
  bool isRelativeToAbsolute() { return _isRelativeToAbsolute; }
  bool isCurrentItemFinished() { return _slave->isFinished(); }
  int getNumItems() { return _numItems; }
  int getCurrentItem() { return _currentItem; }
  float getLength() { return _length; }
  double getCurrentItemMin() { return _currItemMin; }
  double getCurrentItemMax() { return _currItemMax; }
  double getCurrentItemValue() { return _currItemVal; }
  
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
  int _numItems, _currentItem;
  float _length, _height, _padding;
  double _currItemMin, _currItemMax, _currItemVal;
  double _borderZOffset;// ( -0.0003f ),

  std::string _masterText, _slaveText;

  ProgressBar::RefPtr _master;
  ProgressBar::RefPtr _slave;
  osg::ref_ptr< osg::Node > _border;
  osg::ref_ptr< osg::Group > _group;
  osg::Vec3f _pos;
  osg::Vec2f _ll;
  

};

} // Namespace Builders
} // Namespace OsgTools


#endif