
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Jeff Conner and Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Widgets/ThreadSafeProgressBarGroup.h"

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

class ThreadSafeProgressBarGroupCallback : public osg::NodeCallback
{
  public:
  typedef osg::NodeCallback   BaseClass;

  ThreadSafeProgressBarGroupCallback ( ThreadSafeProgressBarGroup * pbarGroup ) : 
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
  virtual ~ThreadSafeProgressBarGroupCallback()
  {
    _pbarGroup = 0x0;
  }
private:
 
  OsgTools::Widgets::ThreadSafeProgressBarGroup * _pbarGroup;
};



///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ThreadSafeProgressBarGroup::ThreadSafeProgressBarGroup() : BaseClass(),
  _bars     (),
  _size     ( 0.0f, 0.0f ),
  _groupBarSize ( 1.0f, 0.03f ),
  _pos      (  0.0f, 0.0, -3.0f ),
  _dirty    ( true ),
  _isRelativeToAbsolute ( true ),
  _root     ( new osg::Group ),
  _padding  ( 0.0f ),
  _numBars ( 0 )
{
  _root->setUpdateCallback( new ThreadSafeProgressBarGroupCallback ( this ) ); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ThreadSafeProgressBarGroup::~ThreadSafeProgressBarGroup()
{
  _bars.clear();
  _root = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vector or progress bars
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Widgets::ThreadSafeProgressBarGroup::Bars ThreadSafeProgressBarGroup::getBars()
{
  return _bars;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Removes progress bar object at position pos
//
///////////////////////////////////////////////////////////////////////////////

void ThreadSafeProgressBarGroup::remove ( unsigned int pos )
{
  USUL_TRACE_SCOPE;

  ThreadSafeProgressBar::RefPtr bar ( _bars.at ( pos ) );
  if ( _bars.size() > 0 )
  {
    if ( _bars.size() == 1 )
    {
      _size.set ( 0, 0 );
    }
    else
    {
      for ( unsigned int x = _bars.size() - 1; x > pos; --x)
      {
        _bars.at( x )->setLowerLeft (  Usul::Math::Vec2f ( _padding,
                                       _bars.at( x - 1 )->getLowerLeft()[1] ) );
      }
      _size[1] -= bar->getHeight()  + _padding;
      
    }
    
    bar->clear();
    _bars.erase( _bars.begin() + pos );
    
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all progress bars that are finished.
//
///////////////////////////////////////////////////////////////////////////////

void ThreadSafeProgressBarGroup::removeFinishedProgressBars ( )
{
  typedef OsgTools::Widgets::ThreadSafeProgressBarGroup::Bars ProgressBars;

  // Make a copy.
  ProgressBars bars ( this->getBars() );

  // Progress bars that will need to be removed.
  ProgressBars doomed;

  for( ProgressBars::iterator iter = bars.begin(); iter != bars.end(); ++iter )
  {
    if(  (*iter)->isFinished() && !(*iter)->isAnimating() && !(*iter)->isVisible() )
    {
      doomed.push_back ( *iter );
    }
  }

  for( ProgressBars::iterator iter = doomed.begin(); iter != doomed.end(); ++iter )
    this->_removeProgressBar ( *iter );

  this->_updatePositions ();

  // We need to be rebuilt.
  this->_setDirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the given progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void ThreadSafeProgressBarGroup::_removeProgressBar ( ThreadSafeProgressBar * bar )
{
  USUL_TRACE_SCOPE;
  
  if ( 0x0 != bar )
  {
    bar->clear();

    Guard guard ( this->mutex() );
    _bars.erase ( std::remove_if ( _bars.begin(), 
                                 _bars.end(), 
                                 ThreadSafeProgressBar::RefPtr::IsEqual ( bar ) ), 
                  _bars.end() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we dirty?
//
///////////////////////////////////////////////////////////////////////////////

bool ThreadSafeProgressBarGroup::_isDirty() const
{
  Guard guard ( this->mutex() );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the text.
//
///////////////////////////////////////////////////////////////////////////////

void ThreadSafeProgressBarGroup::_setDirty ( bool d )
{
  Guard guard ( this->mutex() );
  _dirty = d;
}

void ThreadSafeProgressBarGroup::setBarValue ( int i, double v )
{
  Guard guard ( this->mutex() );
  _bars.at(i)->value(v);
  this->_setDirty( true );
}
double ThreadSafeProgressBarGroup::getBarValue( int i )
{
  Guard guard ( this->mutex() );
  return _bars.at(i)->value();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of progress bars in this group
//
///////////////////////////////////////////////////////////////////////////////

int ThreadSafeProgressBarGroup::getNumBars()
{
  return _bars.size();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Create and add a progress bar object to the progress bar group
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ThreadSafeProgressBarGroup::append (  )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  ThreadSafeProgressBar::RefPtr pbar = new ThreadSafeProgressBar();

  pbar->setBarLengthAndHeight ( _groupBarSize );
  this->_addProgressBar ( pbar.get() );
  this->_setDirty ( true );
  return pbar->queryInterface ( Usul::Interfaces::IUnknown::IID );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append a bar.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ThreadSafeProgressBarGroup::append ( ThreadSafeProgressBar *bar )
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

osg::Node * ThreadSafeProgressBarGroup::getScene()
{
  Guard guard ( this->mutex() );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * ThreadSafeProgressBarGroup::buildScene()
{
  /*if ( false == this->_isDirty() )
  {
    Guard guard ( this->mutex() );
    return _root.get();
  }
  else
  {*/
    Guard guard ( this->mutex() );
    _root->removeChildren ( 0, _root->getNumChildren() ); 

    osg::ref_ptr< osg::MatrixTransform > matrix ( new osg::MatrixTransform() );

    matrix->setMatrix ( osg::Matrix::translate( _pos[0], _pos[1], _pos[2] ) );
    if ( _isRelativeToAbsolute )
    matrix->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );

    // Get a copy of the bars.
    Bars bars;
    {
      Guard guard ( this->mutex() );
      bars = _bars;
     
    }

    osg::Vec3 currentPosition ( _padding, _padding, 0.0 );

    for ( Bars::const_iterator i = bars.begin(); i != bars.end(); ++i )
    {
      osg::ref_ptr< osg::MatrixTransform > m ( new osg::MatrixTransform() );
      m->setMatrix ( osg::Matrix::translate ( currentPosition ) );

      ThreadSafeProgressBar::RefPtr _bar ( *i );
      m->addChild ( _bar->buildScene() );
      
      matrix->addChild ( m.get() );

      //currentPosition.x() += _bar->size()[0];
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

void ThreadSafeProgressBarGroup::_addProgressBar ( ThreadSafeProgressBar * pbar )
{
 
  unsigned int numBars = _bars.size();
  pbar->setLowerLeft ( Usul::Math::Vec2f ( 0.0, 0.0 ) );
  //if( numBars == 0 )
  //  pbar->setLowerLeft ( Usul::Math::Vec2f ( _padding, _padding ) );
  //else
  //{
  //  float prevBarLL = _bars.at( numBars - 1 )->getLowerLeft()[1];
  //  float prevBarH = _bars.at( numBars - 1 )->getHeight();
  //  //pbar->setLowerLeft ( Usul::Math::Vec2f ( _padding, prevBarLL + prevBarH + _padding ) );
  //  pbar->setLowerLeft ( 

  //}
  
  if ( _size[0] < pbar->getLength() + _padding )
    _size[0] = pbar->getLength() + _padding * 2;

  if( _size[1] == 0 )
    _size[1] +=  pbar->getHeight() + _padding * 2;
  else
    _size[1] +=  pbar->getHeight() + _padding;

  _bars.push_back ( pbar );

#ifdef _MSC_VER
# ifdef _DEBUG
  
 /* std::ostringstream os;
  for ( unsigned int i = 0; i < _numBars; ++i )
  {
    os << "bar #" << i << " "
       << " LL: " << _bars.at(i)->getLowerLeft()[0] << " " << _bars.at(i)->getLowerLeft()[1]
       << " HL: " << _bars.at(i)->getHeight() << " " << _bars.at(i)->getLength()
       << std::endl;
    ::OutputDebugStringA ( os.str().c_str() ); 
  }*/
# endif
#endif
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the positions of all the progress bars.
//
///////////////////////////////////////////////////////////////////////////////

void ThreadSafeProgressBarGroup::_updatePositions ()
{
}
