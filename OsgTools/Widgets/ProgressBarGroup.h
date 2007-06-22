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
#include <vector>
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

	enum
  {
    LOWER_LEFT = 0x00000001,
    LOWER_RIGHT = 0x00000002,
    UPPER_LEFT = 0x00000004,
    UPPER_RIGHT = 0x000000008,
    CENTER = 0x00000010
  };
  ///////////////////
  //inline functions
  ///////////////////

  // Get the position of the progress bar group
  const osg::Vec3f & getPosition() { return _pos; }

  // Get the bar message on the slave bar
  const std::string& getMessage( int index ) { return _pbarVector.at(index)->getMessage(); }

  // Return true if the bar group is rendering relative to absolute
  bool isRelativeToAbsolute() { return _isRelativeToAbsolute; }

  // Return true if the slave is 100% complete
  bool isItemFinished( int index ) { return _pbarVector.at(index)->isFinished(); }

  // Return the number of items ( _max ) on the master bar
  int getNumItems() { return _numBars; }

  // Return the length of the bar group
  float getLength() { return _length; }

  // Return the min value on the slave bar
  double getItemMin( int index ) { return _pbarVector.at(index)->getMin(); }

  // Return the max value on the slave bar
  double getItemMax( int index ) { return _pbarVector.at(index)->getMax(); }

  // Return the current value on the slave bar
  double getItemValue( int index ) { return _pbarVector.at(index)->getCurrent(); }

  //Return the padding value that is set to space out bars
  float getPadding() { return _padding; }

  osg::Node* getProgressBarGroup();
  
  void setLocation ( unsigned int loc );
  void setRelativeToAbsolute ( bool value );
  void setPosition ( const osg::Vec3f & );
  void setItemMin( int i, double min );
  void setItemMax( int i, double max );
  void setItemValue( int i, double v );
  void setMessage ( int i, const std::string& m );
  void setPadding ( float p );
  void resetBar ( int i );

  void add ( ProgressBar* pbar );
  void add ( const std::string& m, double min, double max );
  void remove ( int pos );
	
protected:

  virtual ~ProgressBarGroup();
	void _buildProgressBarGroup();
  osg::Node* _buildBar ( int render_level , std::string tex, const osg::Vec2f& ul, const osg::Vec2f& lr, float depth  );

private:

  bool _isRelativeToAbsolute;
  int _numBars;
  float _length, _height, _padding;
  double _borderZOffset;// ( -0.0003f ),

  std::vector < ProgressBar::RefPtr > _pbarVector;

  osg::ref_ptr< osg::Node > _border;
  osg::ref_ptr< osg::Group > _group;

  osg::Vec3f _pos;
  osg::Vec2f _ll;
  

};

} // Namespace Builders
} // Namespace OsgWidgets


#endif
