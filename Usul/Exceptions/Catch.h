
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Catch-block definitions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_EXCEPTIONS_CATCH_BLOCK_DEFINITIONS_H_
#define _USUL_EXCEPTIONS_CATCH_BLOCK_DEFINITIONS_H_

#include <sstream>
#include <stdexcept>


namespace Usul {
namespace Exceptions {


///////////////////////////////////////////////////////////////////////////////
//
//  Catch handler.
//
///////////////////////////////////////////////////////////////////////////////

template < class ErrorIdType > inline void catchHandler ( ErrorIdType id, const std::exception &e, std::ostream &out )
{
  out << "Error " << id << ": Standard exception caught." << std::flush;
  if ( e.what() )
    out << ". " << e.what() << std::flush;
  out << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Catch handler.
//
///////////////////////////////////////////////////////////////////////////////

template < class ErrorIdType > inline void catchHandler ( ErrorIdType id, const std::string &e, std::ostream &out )
{
  out << "Error " << id << ": Standard string exception caught." << std::flush;
  if ( false == e.empty() )
    out << ". " << e << std::flush;
  out << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Catch handler.
//
///////////////////////////////////////////////////////////////////////////////

template < class ErrorIdType > inline void catchHandler ( ErrorIdType id, const char *e, std::ostream &out )
{
  out << "Error " << id << ": String exception caught." << std::flush;
  if ( e && ( ::strlen ( e ) > 0 ) )
    out << ". " << e << std::flush;
  out << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Catch handler.
//
///////////////////////////////////////////////////////////////////////////////

template < class ErrorIdType > inline void catchHandler ( ErrorIdType id, unsigned int e, std::ostream &out )
{
  out << "Error " << id << ": Unsigned integer exception caught: " << e << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Catch handler.
//
///////////////////////////////////////////////////////////////////////////////

template < class ErrorIdType > inline void catchHandler ( ErrorIdType id, int e, std::ostream &out )
{
  out << "Error " << id << ": Integer exception caught: " << e << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Catch handler.
//
///////////////////////////////////////////////////////////////////////////////

template < class ErrorIdType > inline void catchHandler ( ErrorIdType id, std::ostream &out )
{
  out << "Error " << id << ": Unknown exception caught." << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Declare catch block for one type.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_CATCH_EXCEPTION_TYPE(error_id,exception_type,stream_object)\
catch ( exception_type e )\
{\
  Usul::Exceptions::catchHandler ( error_id, e, stream_object );\
}


///////////////////////////////////////////////////////////////////////////////
//
//  Declare catch block for unknown type.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_CATCH_EXCEPTION_UNKNOWN(error_id,stream_object)\
catch ( ... )\
{\
  Usul::Exceptions::catchHandler ( error_id, stream_object );\
}


///////////////////////////////////////////////////////////////////////////////
//
//  Declare catch block for all types.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_CATCH_ALL_EXCEPTIONS(error_id,stream_object)\
  USUL_CATCH_EXCEPTION_TYPE ( error_id, const std::exception &, stream_object )\
  USUL_CATCH_EXCEPTION_TYPE ( error_id, const std::string &,    stream_object )\
  USUL_CATCH_EXCEPTION_TYPE ( error_id, const char *,           stream_object )\
  USUL_CATCH_EXCEPTION_TYPE ( error_id, unsigned int,           stream_object )\
  USUL_CATCH_EXCEPTION_TYPE ( error_id, int,                    stream_object )


}; // namespace Exceptions
}; // namespace Usul


#endif // _USUL_EXCEPTIONS_CATCH_BLOCK_DEFINITIONS_H_
