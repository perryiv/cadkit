
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Jeff Conner and Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSGTOOLS_WIDGET_THREAD_SAFE_PROGRESS_BAR_
#define _OSGTOOLS_WIDGET_THREAD_SAFE_PROGRESS_BAR_

#include <string>

#include "OsgTools/Export.h"
//#include "OsgTools/Widgets/Helper/ThreadSafeText.h"

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"

#include "osg/Group"
#include "osg/ref_ptr"

namespace osg { class Node; }


namespace OsgTools {
namespace Widgets {


class OSG_TOOLS_EXPORT ProgressBar : public Usul::Base::Object,
							                       public Usul::Interfaces::IStatusBar,
							                       public Usul::Interfaces::IProgressBar
{
public:
  
  typedef Usul::Base::Object BaseClass;

  USUL_DECLARE_QUERY_POINTERS ( ProgressBar );
	USUL_DECLARE_IUNKNOWN_MEMBERS;

	ProgressBar();

  
  void                    text ( const std::string & );
  void                    setAnimation ( bool value );
  void                    setBarLengthAndHeight ( const Usul::Math::Vec2f& lh );
  void                    setLowerLeft( const Usul::Math::Vec2f& ll );
  void                    value ( double );
  void                    stepAnimation();
  void                    clear();
  void                    finished( bool value );

  bool                    isAnimating();
  bool                    isVisible();
  bool                    isFinished();

  float                   getAnimationStep ();
  float                   getAnimationEnd ();
  float                   getAnimationCurrent ();
  float                   getLength();
  float                   getHeight();

  double                  value() const;

  
  Usul::Math::Vec2f       size() const;
  Usul::Math::Vec2f       padding() const;
  Usul::Math::Vec2d       range() const;
  Usul::Math::Vec2f       getLowerLeft() const;

  osg::Node *             buildScene();

  std::string             text() const;

  ////////////////////
  // Inherited
  ////////////////////

  // Show the progress bar
  virtual void            showProgressBar();

  // Set the total of progress bar
  virtual void            setTotalProgressBar ( unsigned int value );

  // Update the progress bar
  virtual void            updateProgressBar ( unsigned int value );

  // Hide the progress bar
  virtual void            hideProgressBar();
  
  // Set the status bar text.
  virtual void            setStatusBarText ( const std::string &text, bool force );

protected:
	
  virtual                 ~ProgressBar();
  std::string             _getPercentComplete();
  void                    _layout();
  void                    _setupAnimation( float cur, float end, float step );

  osg::Node *             _emptyScene();
  osg::Node *             _buildScene();

private:

  // No copying or assignment.
  ProgressBar ( const ProgressBar & );
  ProgressBar &operator = ( const ProgressBar & );

  void                          _setDirty ( bool );
  bool                          _isDirty() const;
 

  Usul::Math::Vec2d             _range;
  double                        _value;
  double                        _barSize;
  std::string                   _text;
  Usul::Math::Vec2f             _size;
  Usul::Math::Vec2f             _padding;

  Usul::Math::Vec3f             _pos;
  Usul::Math::Vec3f             _barPos, _barBorderPos, _borderPos;
  Usul::Math::Vec2f             _barLH, _barBorderLH, _borderLH;
  Usul::Math::Vec3f             _percentPos, _labelPos;
  Usul::Math::Vec3f             _animationCurEndStep;

  osg::ref_ptr<osg::Group>      _root;
  
  bool                          _dirty;
  bool                          _isVisible, _isAnimating, _isFinished;
};


} // Namespace Builders
} // Namespace OsgWidgets


#endif // _OSGTOOLS_WIDGET_THREAD_SAFE_PROGRESS_BAR_
