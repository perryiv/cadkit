///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include "osg/Group"
#include <string>

#include "OsgTools/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"


namespace OsgTools {
namespace Widgets {

class OSG_TOOLS_EXPORT ProgressBar : public Usul::Base::Referenced,
							                       public Usul::Interfaces::IStatusBar,
							                       public Usul::Interfaces::IProgressBar
{
public:
  
  typedef Usul::Base::Referenced BaseClass;

  USUL_DECLARE_QUERY_POINTERS ( ProgressBar );
	USUL_DECLARE_IUNKNOWN_MEMBERS;

	ProgressBar();
	
	osg::Node* getProgressBar();
  const std::string& getMessage() { return _barText; }
  const osg::Vec2f& getLowerLeft() { return _ll; }
  bool isFinished() { return _isFinished; }
  bool isRelativeToAbsolute() { return _isRelativeToAbsolute; }
  float getBarHeight() { return _barHeight; }
  float getBarLength() { return _barLength; }
  float getBorderHeight() { return _borderHeight; }
  float getBorderLength() { return _borderLength; }
  double getMin() { return _min; }
  double getMax() { return _max; }
  double getCurrent() { return _current; }
  
  void updateProgressBar();
  void setMin( double min );
  void setMax( double max );
  void setCurrent( double c );
  void setBarHeight ( float h );
  void setBarLength ( float l );
  void setRelativeToAbsolute ( bool value );
  void setMessage ( const std::string & s );
  void setLowerLeft( const osg::Vec2f & ll );
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

  

  
protected:
	
  virtual ~ProgressBar();
  void _buildProgressBarObject();
  void _buildProgressBar();
  std::string _getPercentComplete();
  osg::Node* _buildBar (  int render_level , std::string tex, const osg::Vec2f& ul, const osg::Vec2f& lr, float depth  );
  osg::Geode* _drawTextAtPosition ( const osg::Vec3f & p, const std::string & s );

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
  bool _isRelativeToAbsolute;
  bool _isFinished;
  osg::Vec2f _ll;
  std::string _barText;
  osg::ref_ptr< osg::Node > _progressBar;
  osg::ref_ptr< osg::Node > _backgroundBar;
  osg::ref_ptr< osg::Node > _barBorder;
  osg::ref_ptr< osg::Node > _border;
  osg::ref_ptr< osg::Geode > _percent;
  osg::ref_ptr< osg::Geode > _text;
  osg::ref_ptr< osg::Group > _pbarGroup;

};
} // Namespace Builders
} // Namespace OsgTools

#endif