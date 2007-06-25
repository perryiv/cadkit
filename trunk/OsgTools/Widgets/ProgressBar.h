
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Jeff Conner and Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include "osg/Group"
#include <string>

#include "OsgTools/Export.h"
#include "OsgTools/Widgets/Helper/ThreadSafeText.h"
#include "OsgTools/Widgets/Helper/UpdateProgress.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include "osg/Geometry"
#include "osg/RenderInfo"

namespace OsgTools {
namespace Widgets {

class OSG_TOOLS_EXPORT ProgressBar : public Usul::Base::Referenced,
							                       public Usul::Interfaces::IStatusBar,
							                       public Usul::Interfaces::IProgressBar
{
public:
  
  typedef Usul::Base::Referenced                         BaseClass;
  typedef OsgTools::Widgets::Helper::ThreadSafeText      ThreadSafeText;
  typedef OsgTools::Widgets::Helper::UpdateProgress      UpdateProgress;

  USUL_DECLARE_QUERY_POINTERS ( ProgressBar );
	USUL_DECLARE_IUNKNOWN_MEMBERS;

	ProgressBar();
	
  ///////////////////
  //inline functions
  ///////////////////
	
  // Return the current text of the progress bar
  const std::string& getMessage() { return _barText; }

  // Return true if the bar is 100% complete ( _current == _max )
  bool isFinished() { return _isFinished; }

  // Return true if the current render mode is relative to absolute
  bool isRelativeToAbsolute() { return _isRelativeToAbsolute; }

  // Return the height of the bar object
  float getBarHeight() { return _barHeight; }
  
  // Return the length of the bar object
  float getBarLength() { return _barLength; }

  // Return the height of the border object
  float getBorderHeight() { return _borderHeight; }

  // Return the length of the border object
  float getBorderLength() { return _borderLength; }

  // Return the minimum value of the progress bar
  double getMin() { return _min; }

  // Return the maximum value of the progress bar
  double getMax() { return _max; }

  // Return the current value of the progress bar
  double getCurrent() { return _current; }

  // Return true if the progress is being animated
  bool isAnimating() { return _isAnimating; }

  // Return true if the progress bar is being shown
  bool isVisible() { return _isVisible; }


  
  osg::Vec2f getLowerLeft();
  
  float getLength();
  float getHeight();
  osg::Node* getProgressBar();
  
  void updateProgressBar();
  void setMin( double min );
  void setMax( double max );
  void setCurrent( double c );
  void setBarHeight ( float h );
  void setBarLength ( float l );
  void setRelativeToAbsolute ( bool value );
  void setMessage ( const std::string & s );
  void setLowerLeft( const osg::Vec2f & ll );
  void setAnimation ( bool value );
  void reset();

  // Show the progress bar
  virtual void showProgressBar();

  // Set the total of progress bar
  virtual void setTotalProgressBar ( unsigned int value );

  // Update the progress bar
  virtual void updateProgressBar ( unsigned int value );

  // Hide the progress bar
  virtual void hideProgressBar();
  
  // Set the status bar text.
  virtual void setStatusBarText ( const std::string &text, bool force );

  void clear();
  
protected:
	
  virtual ~ProgressBar();
  void _buildProgressBarObject();
  void _buildProgressBar();
  void _emptyProgressBar();
  std::string _getPercentComplete();

private:

  double _min, _max, _current;
  float _barSize;
  float _backSize;
  float _barLength;
  float _barHeight;
  float _barBorderThickness;
  float _barBorderZOffset;
  float _borderLength;
  float _borderHeight;
  float _borderPadding;
  float _borderZOffset;
  float _textZOffset;
  float _animationStart, _animationEnd, _animationStep;
  bool _isRelativeToAbsolute;
  bool _isFinished;
  bool _isVisible, _isAnimating;

  osg::Vec2f _ll;
  std::string _barText;
  osg::ref_ptr< osg::Node > _progressBar;
  osg::ref_ptr< osg::Node > _backgroundBar;
  osg::ref_ptr< osg::Node > _barBorder;
  osg::ref_ptr< osg::Node > _border;
  osg::ref_ptr< osg::Geode > _percent;
  osg::ref_ptr< osg::Geode > _text;
  osg::ref_ptr< osg::Group > _pbarGroup;

  osg::ref_ptr < ThreadSafeText > _labelText;
  osg::ref_ptr < ThreadSafeText > _percentText;

  osg::ref_ptr < UpdateProgress > _progressDrawable;
  osg::ref_ptr < UpdateProgress > _backgroundDrawable;
};

} // Namespace Builders
} // Namespace OsgWidgets

#endif
