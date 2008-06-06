
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#include "MovieHandler.h"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//
// Set the node that will be traversed to gather all the image streams object 
// content in it. 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::MovieHandler::set( osg::Node* node )
{  
  mImageStreamList.clear();

  Cinema::Core::FindImageStreamsVisitor fisv( mImageStreamList );
  node->accept( fisv );
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the current action type( play, pause, restart etc..).
//
///////////////////////////////////////////////////////////////////////////////
Cinema::Core::MovieHandler::Action Cinema::Core::MovieHandler::action() const 
{
  return mAction;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the current action type( play, pause, restart etc..).
// @Note: 
// Application has to explicitly call update(). 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::MovieHandler::action( Cinema::Core::MovieHandler::Action action )
{
  mAction = action;
}

///////////////////////////////////////////////////////////////////////////////
//
// Get the current mode. 
//
///////////////////////////////////////////////////////////////////////////////
Cinema::Core::MovieHandler::Mode Cinema::Core::MovieHandler::mode() const 
{
  return mMode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the current mode. 
// @Note: 
// Application has to explicitly call update(). 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::MovieHandler::mode( Cinema::Core::MovieHandler::Mode mode )
{
  mMode = mode;
}

///////////////////////////////////////////////////////////////////////////////
//
// Update actions and mode. 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::MovieHandler::update()
{
  switch( mAction )
  {
    case PLAY:
    {
      for( ImageStreamList::iterator itr= mImageStreamList.begin(); itr!= mImageStreamList.end(); ++itr )
      {
          std::cout << "Play" << std::endl;
          if( (*itr)->getStatus() != osg::ImageStream::INVALID )
          {
            (*itr)->play();
          }
      }
      break;
    }
    case Core::MovieHandler::PAUSE:
    {
      for( ImageStreamList::iterator itr= mImageStreamList.begin(); itr!= mImageStreamList.end(); ++itr )
      {
          std::cout << "Pause" << std::endl;
          if( (*itr)->getStatus() != osg::ImageStream::INVALID )
          {
            (*itr)->pause();
          }
      }
      break;
    }
    case Core::MovieHandler::STOP:
    {
      // Yet to be implemented.
      break;
    }
    case Core::MovieHandler::RESTART:
    {
      for( ImageStreamList::iterator itr= mImageStreamList.begin(); itr!= mImageStreamList.end(); ++itr )
      {
          std::cout << "Restart" << std::endl;
          if( (*itr)->getStatus() != osg::ImageStream::INVALID )
          {
            (*itr)->rewind();
            (*itr)->play();
          }
      }
      break;
    }
    case Core::MovieHandler::REWIND:
    {
      // Yet to be implemented.
      break;
    }
    case Core::MovieHandler::FORWARD: 
    {
      // Yet to be implemented.
      break;
    }
    default:
    {
      return;      
    }
  };
}

///////////////////////////////////////////////////////////////////////////////
//
// Perform clean up. 
//
///////////////////////////////////////////////////////////////////////////////
void Cinema::Core::MovieHandler::cleanUp()
{
  for( size_t i=0; i < mImageStreamList.size(); ++i )
  {
    mImageStreamList[i]->quit();
  }

  mImageStreamList.clear();
}

bool Cinema::Core::MovieHandler::isPlaying()
{
  // Return the status for last stream only.   
  if( mImageStreamList.size() > 0 )
  {
    if( mImageStreamList[ mImageStreamList.size() - 1 ]->getStatus() == osg::ImageStream::PLAYING )
    {
      return true;
    }  
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}

bool Cinema::Core::MovieHandler::isPaused()
{
  // Return the status for last stream only.   
  if( mImageStreamList.size() > 0 )
  {
    if( mImageStreamList[ mImageStreamList.size() - 1 ]->getStatus() == osg::ImageStream::PAUSED )
    {
      return true;
    }  
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}
