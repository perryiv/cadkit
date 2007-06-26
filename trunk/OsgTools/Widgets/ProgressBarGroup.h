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
    LOWER_LEFT,
    LOWER_RIGHT,
    UPPER_LEFT,
    UPPER_RIGHT,
    CENTER
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

   // Return true if the progress bar at index is being animated
  bool isAnimating( unsigned int index ) { return _pbarVector.at(index)->isAnimating(); }

  // Return true if the progress bar at index is being shown
  bool isVisible( unsigned int index ) { return _pbarVector.at(index)->isVisible(); }

  osg::Node* getProgressBarGroup();
  
  void setLocation ( unsigned int loc );
  void setRelativeToAbsolute ( bool value );
  void setPosition ( const osg::Vec3f & );
  void setItemMin( unsigned int i, double min );
  void setItemMax( unsigned int i, double max );
  void setItemValue( unsigned int i, double v );
  void setMessage ( unsigned int i, const std::string& m );
  void setPadding ( float p );

  void showProgressBar ( unsigned int index );
  void hideProgressBar ( unsigned int index );
  void resetBar ( unsigned int i );

  // Append a progress bar to the stack.
  Usul::Interfaces::IUnknown*  append();

  void add ( ProgressBar* pbar );
  void add ( const std::string& m, double min, double max );
  void remove ( unsigned int pos );
  void update ();

  void clear();
	
protected:

  virtual ~ProgressBarGroup();
	void _buildProgressBarGroup();
  osg::Node* _buildBar ( unsigned int render_level , std::string tex, const osg::Vec2f& ul, const osg::Vec2f& lr, float depth  );

  void       _addProgressBar ( ProgressBar * bar );
private:

  bool _isRelativeToAbsolute;
  unsigned int _numBars;
  float _length, _height, _padding;
  double _borderZOffset;// ( -0.0003f ),

  std::vector < ProgressBar::RefPtr > _pbarVector;

  osg::ref_ptr< osg::Node > _border;
  osg::ref_ptr< osg::Group > _group;

  osg::Vec3f _pos;
  osg::Vec2f _ll;

  
  //osg::ref_ptr < UpdateProgressBarGroupCallback > _updateCallback;
  

};

} // Namespace Builders
} // Namespace OsgWidgets


#endif
