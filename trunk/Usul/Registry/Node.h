
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
#include "Usul/Registry/Convert.h"
#include "Usul/Trace/Trace.h"

#include <string>
#include <map>

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

  // Get the string value.
  std::string                     get ( const std::string &defaultValue ) const;
  template < class T > T          get ( const T &defaultValue ) const;

  // Operator to return the child with the name. Creates child nodes as needed.
  Node &                          operator [] ( const std::string &name );

  // Set the value.
  template < class T > Node &     operator = ( const T &t );

  // Set the string value.
  void                            set ( const std::string & );
  template < class T > void       set ( const T &t );

protected:

  // Use reference counting.
  virtual ~Node();

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
  this->set ( Usul::Registry::Convert<T>::to ( t ) );
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
  Usul::Registry::Convert<T>::from ( _value, v );
  return v;
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
