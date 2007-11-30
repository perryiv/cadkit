
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Traps pure virtual function calls.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ERROR_PURE_VIRTUAL_CALL_HANDLER_H_
#define _USUL_ERROR_PURE_VIRTUAL_CALL_HANDLER_H_


namespace Usul {
namespace Errors {


template < class ActionType > struct PureVirtualCall
{
  typedef PureVirtualCall < ActionType > ThisType;

  PureVirtualCall()
  {
    ::_set_purecall_handler ( &ThisType::_pureCallHandler );
  }

private:

  static void _pureCallHandler()
  {
    ActionType action ( 1833320484, false );
  }
};


} // namespace Errors
} // namespace Usul


#endif // _USUL_ERROR_PURE_VIRTUAL_CALL_HANDLER_H_
