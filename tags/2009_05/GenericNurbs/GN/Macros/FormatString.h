
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
#define snprintf sprintf_s
#endif


#endif // _GENERIC_NURBS_LIBRARY_MACRO_FORMAT_STRING_H_
