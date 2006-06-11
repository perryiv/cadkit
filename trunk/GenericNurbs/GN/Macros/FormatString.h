
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  VC++ 2005 has depreciated sprintf
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_MACRO_FORMAT_STRING_H_
#define _GENERIC_NURBS_LIBRARY_MACRO_FORMAT_STRING_H_


#if ( _MSC_VER >= 1400 )
#define GN_FORMAT_STRING_1(the_buffer,buffer_size,format_string,exp_1)\
  ::sprintf_s ( static_cast < char * > ( the_buffer ), buffer_size, format_string, exp_1 )
#define GN_FORMAT_STRING_2(the_buffer,buffer_size,format_string,exp_1,exp_2)\
  ::sprintf_s ( static_cast < char * > ( the_buffer ), buffer_size, format_string, exp_1, exp_2 )
#else
#define GN_FORMAT_STRING_1(buffer,buffer_size,format_string,exp1)\
  ::sprintf ( buffer, format, exp )
#define GN_FORMAT_STRING_2(buffer,buffer_size,format_string,exp1,exp2)\
  ::sprintf ( buffer, format, exp1, exp2 )
#endif


#endif // _GENERIC_NURBS_LIBRARY_MACRO_FORMAT_STRING_H_
