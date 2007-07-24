
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Database pager class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _STAR_SYSTEM_DATABASE_PAGER_CLASS_H_
#define _STAR_SYSTEM_DATABASE_PAGER_CLASS_H_

#include "StarSystem/Export.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

namespace osgDB { class DatabasePager; }


namespace StarSystem {


class STAR_SYSTEM_EXPORT Pager
{
public:

  // Useful typedefs.
  typedef osgDB::DatabasePager DataPager;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;

  // Destroy the single instance.
  static void               destroy();

  // Initialize the cull and update visitors of the caller.
  void                      initVisitors ( Usul::Interfaces::IUnknown *caller );

  // Get the single instance.
  static Pager &            instance();

  // Get the mutex.
  Mutex &                   mutex() const;

  // Get the database pager.
  DataPager *               pager();

  // Pre- and post-render notifications.
  void                      preRender  ( Usul::Interfaces::IUnknown *caller );
  void                      postRender ( Usul::Interfaces::IUnknown *caller );

protected:

  // Constructor
  Pager();

  // Destructor.
  ~Pager();

private:

  // No copying or assignment.
  Pager ( const Pager & );
  Pager &operator = ( const Pager & );

  void                      _destroy();

  static Pager *_instance;
  mutable Mutex *_mutex;
  DataPager *_pager;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_DATABASE_PAGER_CLASS_H_
