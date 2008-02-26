
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Registry database class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_REGISTRY_DATABASE_CLASS_H_
#define _USUL_REGISTRY_DATABASE_CLASS_H_

#include "Usul/Registry/Node.h"


namespace Usul {
namespace Registry {


class USUL_EXPORT Database
{
public:

  // Typedefs.
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef Node::Path Path;

    // Constructor and destructor.
  Database();
  ~Database();

  // Accept the visitor.
  void                            accept ( Visitor * );

  // Convert the given string to a legal tag.
  std::string                     convertToTag ( const std::string & ) const;

  // Clear the database.
  void                            clear();

  // Destroy the single instance.
  static void                     destroy();

  // Return the single instance.
  static Database &               instance();

  // Get the mutex.
  Mutex &                         mutex() const;

  // Operator to return the child with the name or path. Creates child nodes as needed.
  Node &                          operator [] ( const std::string &name );
  Node &                          operator [] ( const Path &path );

  // Get the root.
  Node *                          root();
  const Node *                    root() const;

  // Helper class to create and destroy the database.
  struct Life
  {
    Life()  { Usul::Registry::Database::instance(); }
    ~Life() { Usul::Registry::Database::destroy();  }
  };

private:

  // No copying or assignment.
  Database &operator = ( const Database & );
  Database ( const Database & );

  static Database *_instance;
  mutable Mutex *_mutex;
  Node::RefPtr _root;
};


} // namespace Registry
} // namespace Usul


#endif // _USUL_REGISTRY_DATABASE_CLASS_H_
