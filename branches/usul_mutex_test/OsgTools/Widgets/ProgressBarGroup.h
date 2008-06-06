
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Jeff Conner and Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSGTOOLS_WIDGET_THREAD_SAFE_PROGRESS_BAR_GROUP_
#define _OSGTOOLS_WIDGET_THREAD_SAFE_PROGRESS_BAR_GROUP_

#include <string>

#include "OsgTools/Widgets/ProgressBar.h"

#include "Usul/Base/Object.h"
#include "Usul/Math/Vector2.h"

#include "osg/Group"
#include "osg/Node"
#include "osg/ref_ptr"

#include <vector>

namespace osg { class Node; }


namespace OsgTools {
namespace Widgets {


class OSG_TOOLS_EXPORT ProgressBarGroup : public Usul::Base::Object
{
public:
  
  typedef Usul::Base::Object BaseClass;
  typedef std::vector < ProgressBar::RefPtr > Bars;

  USUL_DECLARE_QUERY_POINTERS ( ProgressBarGroup );

	ProgressBarGroup();

  Usul::Interfaces::IUnknown*             append ();
  Usul::Interfaces::IUnknown*             append ( ProgressBar * );
  

  Usul::Math::Vec2f                       size() const;

  osg::Node *                             buildScene();
  osg::Node *                             getScene();
  int                                     getNumBars();
  
  void                                    removeFinishedProgressBars ( );
  
  Bars                                    getBars();

  void                                    position ( const Usul::Math::Vec3f& position );
  Usul::Math::Vec3f                       position () const;

  void                                    defaultProgressBarSize ( const Usul::Math::Vec2f& size );
  Usul::Math::Vec2f                       defaultProgressBarSize () const;

protected:
	
  virtual                                 ~ProgressBarGroup();
  void                                    _addProgressBar    ( ProgressBar * bar );
  void                                    _removeProgressBar ( ProgressBar * bar );
private:

  // No copying or assignment.
  ProgressBarGroup ( const ProgressBarGroup & );
  ProgressBarGroup &operator = ( const ProgressBarGroup & );

  void                                  _setDirty ( bool );
  bool                                  _isDirty() const;

  Bars                                  _bars;
  Usul::Math::Vec2f                     _defaultProgressBarSize;
  Usul::Math::Vec3f                     _position;
  bool                                  _dirty;
  bool                                  _isRelativeToAbsolute;
  osg::ref_ptr<osg::Group>              _root;
  float                                 _padding;
  unsigned int                          _numBars;
};


} // Namespace Builders
} // Namespace OsgWidgets


#endif // _OSGTOOLS_WIDGET_THREAD_SAFE_PROGRESS_BAR_GROUP_
