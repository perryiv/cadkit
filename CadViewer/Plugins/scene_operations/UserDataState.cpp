
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  User-data for state-sets.
//
///////////////////////////////////////////////////////////////////////////////

#include "UserDataState.h"

#include "CadViewer/ErrorChecker.h"

using namespace CV;
using namespace CV::Plugins;

CV_SCENE_OPERATIONS_IMPLEMENT_USER_DATA ( UserDataState, 1070870795 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

UserDataState::UserDataState() : UserDataBase(),
  _states()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

UserDataState::UserDataState ( const UserDataState &s ) : UserDataBase ( s ),
  _states ( s._states )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

UserDataState::~UserDataState()
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Push the state-set.
//
/////////////////////////////////////////////////////////////////////////////

void UserDataState::push ( osg::StateSet *s )
{
  // Null is ok.
  _states.push ( StateSetPtr ( s ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Pop the top-most state-set.
//
/////////////////////////////////////////////////////////////////////////////

void UserDataState::pop()
{
  ErrorChecker ( 1070862516, false == _states.empty() );
  _states.pop();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return te top-most state-set.
//
/////////////////////////////////////////////////////////////////////////////

osg::StateSet *UserDataState::top()
{
  ErrorChecker ( 1070862517, false == _states.empty() );
  return ( _states.top().valid() ) ? _states.top().get() : 0x0;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return te top-most state-set.
//
/////////////////////////////////////////////////////////////////////////////

const osg::StateSet *UserDataState::top() const
{
  ErrorChecker ( 1070862518, false == _states.empty() );
  return ( _states.top().valid() ) ? _states.top().get() : 0x0;
}
