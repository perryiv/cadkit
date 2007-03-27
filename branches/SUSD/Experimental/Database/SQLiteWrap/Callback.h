
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base callback for SQLite database.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SQL_LITE_WRAP_CALLBACK_H_
#define _SQL_LITE_WRAP_CALLBACK_H_

#include "SQLiteWrap/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include <string>


namespace SQLiteWrap {

class Database;

class SQL_LITE_WRAP_EXPORT Callback : public Usul::Base::Referenced
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Callback );

  // Constructor
  Callback();

  // Get the mutex.
  Mutex &                 mutex() const;

  // Called right after a query.
  virtual void            finish ( const Database &db );

  // Called by SQLlite during a select query.
  virtual void            operator () ( const Database &db, const std::string &name, const std::string &value );

  // Called right before a query.
  virtual void            start ( const Database &db );

protected:

  // Destructor
  virtual ~Callback();

  void                    _clean();

private:

  // Can not copy or assign.
  Callback ( const Callback & );
  Callback &operator = ( const Callback & );

  // Data members.
  mutable Mutex *_mutex;
};


} // namespace SQLiteWrap


#endif // _SQL_LITE_WRAP_CALLBACK_H_
