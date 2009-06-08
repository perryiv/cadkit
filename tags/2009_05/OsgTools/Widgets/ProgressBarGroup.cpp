
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Jeff Conner and Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Widgets/ProgressBarGroup.h"

#include "Usul/Trace/Trace.h"

#include "osg/Group"
#include "osg/MatrixTransform"

#include <algorithm>

#ifdef _MSC_VER 
# ifdef _DEBUG
#  include <windows.h>
# endif
#endif


using namespace OsgTools::Widgets;

///////////////////////////////////////////////////////////////////////////////
//
//  Update Callback for the progress bar group to remove finished progress
//  bars.
//
///////////////////////////////////////////////////////////////////////////////

class ProgressBarGroupCallback : public osg::NodeCallback
{
  public:
  typedef osg::NodeCallback   BaseClass;

  ProgressBarGroupCallback ( ProgressBarGroup * pbarGroup ) : 
    BaseClass(),
    _pbarGroup ( pbarGroup )
  {
  }
  
  virtual void operator() (osg::Node* node, osg::NodeVisitor* nv)
  {
    USUL_TRACE_SCOPE;
    if( 0x0 != _pbarGroup )
    {
      _pbarGroup->removeFinishedProgressBars ();
    }
    traverse(node,nv);
  }

protected:
  virtual ~ProgressBarGroupCallback()
  {
    _pbarGroup = 0x0;
  }
private:
 
  OsgTools::Widgets::ProgressBarGroup * _pbarGroup;
};



///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ProgressBarGroup::ProgressBarGroup() : BaseClass(),
  _bars         (),
  _defaultProgressBarSize ( 1.9f, 0.03f ),
  _position     (  0.0f, 0.0, 0.0f ),
  _dirty    ( true ),
  _isRelativeToAbsolute ( true ),
  _root     ( new osg::Group ),
  _padding  ( 0.0f ),
  _numBars ( 0 )
{
  //_root->setUpdateCallback( new ProgressBarGroupCallback ( this ) ); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ProgressBarGroup::~ProgressBarGroup()
{
  _bars.clear();
  _root = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the position.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::position ( const Usul::Math::Vec3f& p )
{
  {
    Guard guard ( this->mutex() );
    _position = p;
  }
  this->_setDirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the position.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec3f  ProgressBarGroup::position () const
{
  Guard guard ( this->mutex() );
  return _position;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the vector or progress bars
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Widgets::ProgressBarGroup::Bars ProgressBarGroup::getBars()
{
  Guard guard ( this->mutex() );
  return _bars;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all progress bars that are finished.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::removeFinishedProgressBars ( )
{
  USUL_TRACE_SCOPE;
  typedef OsgTools::Widgets::ProgressBarGroup::Bars ProgressBars;

  // Make a copy.
  ProgressBars bars ( this->getBars() );

  // Progress bars that will need to be removed.
  ProgressBars doomed;

  for( ProgressBars::iterator iter = bars.begin(); iter != bars.end(); ++iter )
  {
    if( (*iter)->isFinished() && !(*iter)->isAnimating() && !(*iter)->isVisible() )
    {
      doomed.push_back ( *iter );
    }
  }

  for( ProgressBars::iterator iter = doomed.begin(); iter != doomed.end(); ++iter )
    this->_removeProgressBar ( *iter );

  // We need to be rebuilt.
  this->_setDirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the given progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::_removeProgressBar ( ProgressBar * bar )
{
  USUL_TRACE_SCOPE;
  
  if ( 0x0 != bar )
  {
    bar->clear();

    Guard guard ( this->mutex() );
    _bars.erase ( std::remove_if ( _bars.begin(), 
                                 _bars.end(), 
                                 ProgressBar::RefPtr::IsEqual ( bar ) ), 
                  _bars.end() );
    
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we dirty?
//
///////////////////////////////////////////////////////////////////////////////

bool ProgressBarGroup::_isDirty() const
{
  Guard guard ( this->mutex() );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the text.
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::_setDirty ( bool d )
{
  Guard guard ( this->mutex() );
  _dirty = d;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of progress bars in this group
//
///////////////////////////////////////////////////////////////////////////////

int ProgressBarGroup::getNumBars()
{
  Guard guard ( this->mutex() );
  return _bars.size();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Create and add a progress bar object to the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ProgressBarGroup::append (  )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  ProgressBar::RefPtr pbar ( new ProgressBar() );

  this->_addProgressBar ( pbar.get() );
  this->_setDirty ( true );
  return pbar->queryInterface ( Usul::Interfaces::IUnknown::IID );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append a bar.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ProgressBarGroup::append ( ProgressBar *bar )
{
  if ( 0x0 != bar )
  {
    Guard guard ( this->mutex() );
    _bars.push_back ( bar );
    return bar->queryInterface ( Usul::Interfaces::IUnknown::IID );
  }
  else
    return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * ProgressBarGroup::getScene()
{
  Guard guard ( this->mutex() );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * ProgressBarGroup::buildScene()
{
  /*if ( false == this->_isDirty() )
  {
    Guard guard ( this->mutex() );
    return _root.get();
  }
  else
  {*/
    Guard guard ( this->mutex() );

    // Remove what we have.
    _root->removeChildren ( 0, _root->getNumChildren() ); 

    osg::ref_ptr< osg::MatrixTransform > matrix ( new osg::MatrixTransform() );

    matrix->setMatrix ( osg::Matrix::translate( _position[0], _position[1], _position[2] ) );
    if ( _isRelativeToAbsolute )
      matrix->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );

    // Get a copy of the bars.
    Bars bars ( _bars );

    osg::Vec3 currentPosition ( _padding, _padding, 0.0 );

    for ( Bars::const_iterator i = bars.begin(); i != bars.end(); ++i )
    {
      osg::ref_ptr< osg::MatrixTransform > m ( new osg::MatrixTransform() );
      m->setMatrix ( osg::Matrix::translate ( currentPosition ) );

      ProgressBar::RefPtr _bar ( *i );
      m->addChild ( _bar->buildScene() );
      
      matrix->addChild ( m.get() );

      currentPosition.y() += _bar->size()[1];
    }

    _root->addChild ( matrix.get() );
    
    this->_setDirty ( false );

    return _root.get();
  //}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create and add a progress bar object to the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::_addProgressBar ( ProgressBar * pbar )
{
  pbar->setLowerLeft ( Usul::Math::Vec2f ( 0.0, 0.0 ) );
  pbar->setBarLengthAndHeight ( _defaultProgressBarSize );

  _bars.push_back ( pbar );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the default size of a progress bar;
//
///////////////////////////////////////////////////////////////////////////////

void ProgressBarGroup::defaultProgressBarSize ( const Usul::Math::Vec2f& size )
{
  Guard guard ( this );
  _defaultProgressBarSize = size;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the default size of a progress bar;
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec2f ProgressBarGroup::defaultProgressBarSize () const
{
  Guard guard ( this );
  return _defaultProgressBarSize;
}
