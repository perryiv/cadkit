
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Classes for attribute wrappers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_ATTRIBUTE_CLASS_H_
#define _MODFLOW_MODEL_ATTRIBUTE_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Object.h"
#include "Usul/Functions/GUID.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Trace/Trace.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Base class for attribute wrappers.
//
///////////////////////////////////////////////////////////////////////////////

class BaseAttribute : public Usul::Base::Object,
                      public Usul::Interfaces::ILayer
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::ILayer ILayer;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BaseAttribute );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Usul::Interfaces::ILayer
  virtual std::string     guid() const;
  virtual std::string     name() const;

protected:

  // Construction.
  BaseAttribute ( const std::string &name, const std::string &guid );

  // Use reference counting.
  virtual ~BaseAttribute();

private:

  // Do not copy.
  BaseAttribute ( const BaseAttribute & );
  BaseAttribute &operator = ( const BaseAttribute & );

  std::string _guid;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Class for attribute wrapping.
//
///////////////////////////////////////////////////////////////////////////////

template < class Getter, class Setter > class Attribute : public BaseAttribute
{
public:

  // Useful typedefs.
  typedef BaseAttribute BaseClass;
  typedef Attribute < Getter, Setter > ThisType;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::ILayer ILayer;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ThisType );

  // Construction.
  Attribute ( const std::string &name, Getter g, Setter s ) : 
    BaseClass ( name, Usul::Functions::GUID::generate() ),
    _get ( g ),
    _set ( s )
  {
    USUL_TRACE_SCOPE;
  }

  // Set the layer visibility.
  virtual void showLayer ( bool state )
  {
    USUL_TRACE_SCOPE;
    Guard guard ( this->mutex() );
    _set ( state );
  }

  // Get the layer visibiity.
  virtual bool showLayer() const
  {
    USUL_TRACE_SCOPE;
    Guard guard ( this->mutex() );
    return _get();
  }

protected:

  // Use reference counting.
  virtual ~Attribute()
  {
  }

private:

  // Do not copy.
  Attribute ( const Attribute & );
  Attribute &operator = ( const Attribute & );

  mutable Getter _get;
  Setter _set;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to make an attribute wrapper.
//
///////////////////////////////////////////////////////////////////////////////

template < class Getter, class Setter > 
BaseAttribute *makeAttribute ( const std::string &name, Getter g, Setter s )
{
  return new Attribute < Getter, Setter > ( name, g, s );
}


#endif // _MODFLOW_MODEL_ATTRIBUTE_CLASS_H_
