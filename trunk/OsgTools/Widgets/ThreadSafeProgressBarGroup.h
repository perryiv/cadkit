
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

#include "OsgTools/Widgets/ThreadSafeProgressBar.h"

#include "Usul/Base/Object.h"
#include "Usul/Math/Vector2.h"

#include "osg/Group"
#include "osg/Node"
#include "osg/ref_ptr"

#include <vector>

namespace osg { class Node; }


namespace OsgTools {
namespace Widgets {


class OSG_TOOLS_EXPORT ThreadSafeProgressBarGroup : public Usul::Base::Object
{
public:
  
  typedef Usul::Base::Object BaseClass;
  typedef std::vector<ThreadSafeProgressBar::RefPtr> Bars;

  USUL_DECLARE_QUERY_POINTERS ( ThreadSafeProgressBarGroup );

	ThreadSafeProgressBarGroup();

  Usul::Interfaces::IUnknown*             append ();
  Usul::Interfaces::IUnknown*             append ( ThreadSafeProgressBar * );
  

  Usul::Math::Vec2f                       size() const;

  osg::Node *                             buildScene();
  osg::Node *                             getScene();
  int                                     getNumBars();
  void                                    setBarValue ( int i, double v );
  void                                    remove ( unsigned int pos );
  double                                  getBarValue( int i ); 
  Bars                                    getBars();

protected:
	
  virtual                                 ~ThreadSafeProgressBarGroup();
  void                                    _addProgressBar ( ThreadSafeProgressBar * bar );
private:

  // No copying or assignment.
  ThreadSafeProgressBarGroup ( const ThreadSafeProgressBarGroup & );
  ThreadSafeProgressBarGroup &operator = ( const ThreadSafeProgressBarGroup & );

  void                                  _setDirty ( bool );
  bool                                  _isDirty() const;

  Bars                                  _bars;
  Usul::Math::Vec2f                     _size;
  Usul::Math::Vec2f                     _groupBarSize;
  Usul::Math::Vec3f                     _pos;
  bool                                  _dirty;
  bool                                  _isRelativeToAbsolute;
  osg::ref_ptr<osg::Group>              _root;
  float                                 _padding;
  unsigned int                          _numBars;             
};


} // Namespace Builders
} // Namespace OsgWidgets


#endif // _OSGTOOLS_WIDGET_THREAD_SAFE_PROGRESS_BAR_GROUP_
