#ifndef PROGRESSBARGROUP_H
#define PROGRESSBARGROUP_H

#include "osg/Group"
#include <string>
#include "ProgressBar.h"
#include "OsgTools/Export.h"

namespace OsgTools {
namespace Widgets {

class OSG_TOOLS_EXPORT ProgressBarGroup : public osg::Referenced
{
public:

	ProgressBarGroup();
	virtual ~ProgressBarGroup();

  osg::Node* getProgressBarGroup();

  bool isRelativeToAbsolute()
  {
    return _isRelativeToAbsolute;
  };
  const osg::Vec3f & getPosition()
  {
    return _pos;
  };

  double getCurrentItemMin()
  {
    return _currItemMin;
  };
  double getCurrentItemMax()
  {
    return _currItemMax;
  };
  double getCurrentItemValue()
  {
    return _currItemVal;
  };
  int getNumItems()
  {
    return _numItems;
  };
  int getCurrentItem()
  {
    return _currentItem;
  };
  float getLength()
  {
    return _length;
  };
  const std::string& getMessage()
  {
    return _slaveText;
  };
  bool isCurrentItemFinished()
  {
    return _slave->isFinished();
  };

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

	void _buildProgressBarGroup();
  void _init();
  std::string _getPercentComplete();
  osg::Node* _buildBar(  int render_level , std::string tex, const osg::Vec2f& ul, const osg::Vec2f& lr, float depth  );
  void _updateMasterMessage();

private:

  bool _isRelativeToAbsolute;
  int _numItems, _currentItem;
  float _length, _height, _padding;
  double _currItemMin, _currItemMax, _currItemVal;
  double _borderZOffset;// ( -0.0003f ),

  std::string _masterText, _slaveText;

  osg::ref_ptr< ProgressBar > _master, _slave;
  osg::ref_ptr< osg::Node > _border;
  osg::ref_ptr< osg::Group > _group;
  osg::Vec3f _pos;
  osg::Vec2f _ll;
  

};

} // Namespace Builders
} // Namespace OsgTools


#endif