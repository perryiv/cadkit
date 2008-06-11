
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODELPRESENTATION_GENERIC_INDEX_TOGGLE_COMMAND_H__
#define __MODELPRESENTATION_GENERIC_INDEX_TOGGLE_COMMAND_H__

#include "Usul/Commands/Command.h"


template < class GetFunction, class SetFunction > class GenericIndexToggle : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;
  typedef GenericIndexToggle < GetFunction, SetFunction > ThisType;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( GenericIndexToggle );

  GenericIndexToggle ( Usul::Interfaces::IUnknown * caller, GetFunction getter, SetFunction setter, const std::string text, unsigned int index )  :
  BaseClass ( caller ),
  _getter( getter ),
  _setter( setter ),
  _text ( text ),
  _index ( index )
{
  this->text ( text );
}

protected:
  virtual ~GenericIndexToggle ()
  {
  }

  virtual void _execute ()
  {
    _setter( _index, !_getter( _index ) );
  }

  virtual bool updateCheck () const
  {
    return _getter ( _index );
  }

private:
  std::string             _text;
  unsigned int            _index;
  GetFunction             _getter;
  SetFunction             _setter;

};

template < class GetFunction, class SetFunction >
const std::type_info &GenericIndexToggle < GetFunction, SetFunction >::classTypeId() 
{
  return typeid ( ThisType );
}


template < class GetFunction, class SetFunction >
const std::type_info &GenericIndexToggle < GetFunction, SetFunction >::typeId() const 
{
  return ThisType::classTypeId();
}


template < class GetFunction, class SetFunction >
Usul::Commands::Command* GenericIndexToggle < GetFunction, SetFunction >::clone ( ) const 
{
  return new ThisType ( *this );
}

template < class GetFunction, class SetFunction > 
GenericIndexToggle< GetFunction, SetFunction >*
genericIndexToggle ( Usul::Interfaces::IUnknown * caller, GetFunction getter, SetFunction setter, const std::string text, unsigned int set )
{
  return new GenericIndexToggle < GetFunction, SetFunction > ( caller, getter, setter, text, set );
}

#endif // __MODELPRESENTATION_GENERIC_INDEX_TOGGLE_COMMAND_H__



// usage
//= new GenericIndexToggle ( caller, Usul::Adaptors::memberfunction ( this, &ModelPresnetation::_getFunctionName ), 
//                          Usul::Adaptors::memberfunction ( this, &ModelPresnetation::_setFunctionName ),
//                          text, set );
