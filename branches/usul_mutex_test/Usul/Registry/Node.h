
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Node class for the registry.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_REGISTRY_NODE_CLASS_H_
#define _USUL_REGISTRY_NODE_CLASS_H_

#include "Usul/Base/Object.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Trace/Trace.h"

#include <map>
#include <string>
#include <vector>

namespace Usul { namespace Registry { class Visitor; } }


namespace Usul {
namespace Registry {


class USUL_EXPORT Node : public Usul::Base::Object
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;
  typedef std::vector<std::string> Path;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Node );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Node );

  // Have to declare after the smart-pointer definition.
  typedef std::map < std::string, Node::RefPtr > Kids;
  typedef Kids::iterator Iterator;
  typedef Kids::const_iterator ConstIterator;

  // Constructor.
  Node();

  // Accept the visitor.
  void                            accept ( Visitor * );

  // Iterators.
  Iterator                        begin();
  ConstIterator                   begin() const;

  // Clear the node.
  void                            clear();

  // Iterators.
  Iterator                        end();
  ConstIterator                   end() const;

  // Find node with given path.  No new nodes are created.
  Node::RefPtr                    find ( const std::string & ) const;

  // Get the value.
  std::string                     get ( const std::string &defaultValue, bool setValueIfEmpty );
  std::string                     get ( const std::string &defaultValue ) const;
  template < class T > T          get ( const T &defaultValue, bool setValueIfEmpty );
  template < class T > T          get ( const T &defaultValue ) const;

  // Operator to return the child with the name or path. Creates child nodes as needed.
  Node &                          operator [] ( const std::string &name );
  Node &                          operator [] ( const Path &path );

  // Set the value.
  template < class T > Node &     operator = ( const T &t );

  // Set the string value.
  void                            set ( const std::string & );
  template < class T > void       set ( const T &t );

protected:

  // Use reference counting.
  virtual ~Node();

  Node::RefPtr                    _getNode ( const std::string & );

private:

  // No copying or assignment.
  Node &operator = ( const Node & );
  Node ( const Node & );

  std::string _value;
  Kids _kids;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value. Extend formatting with overloads to "to".
//
///////////////////////////////////////////////////////////////////////////////

template < class T > void Node::set ( const T &t )
{
  USUL_TRACE_SCOPE;
  this->set ( Usul::Convert::Type<T,std::string>::convert ( t ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value. Extend formatting with overloads to "from".
//
///////////////////////////////////////////////////////////////////////////////

template < class T > T Node::get ( const T &defaultValue ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( true == _value.empty() )
    return defaultValue;

  T v = defaultValue;
  Usul::Convert::Type<std::string,T>::convert ( _value, v );
  return v;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value. Extend formatting with overloads to "from".
//
///////////////////////////////////////////////////////////////////////////////

template < class T > T Node::get ( const T &defaultValue, bool setValueIfEmpty )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( ( true == _value.empty() ) && ( true == setValueIfEmpty ) )
  {
    this->set ( defaultValue );
  }

  return this->get<T> ( defaultValue );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > Node &Node::operator = ( const T &t )
{
  USUL_TRACE_SCOPE;
  this->set ( t );
  return *this;
}


} // namespace Registry
} // namespace Usul


#endif // _USUL_REGISTRY_VISITOR_CLASS_H_
