
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NcDefine.h: Definitions, numerical and otherwise.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_NURBS_CORE_LIBRARY_POUND_DEFINES_H_
#define _CADKIT_NURBS_CORE_LIBRARY_POUND_DEFINES_H_


///////////////////////////////////////////////////////////////////////////////
//
//  Numerical limits.
//
///////////////////////////////////////////////////////////////////////////////

#define CADKIT_NURBS_MINIMUM_NUMBER_OF_INDEPENDENT_VARIABLES              1
#define CADKIT_NURBS_MINIMUM_NUMBER_OF_DEPENDENT_VARIABLES                1
#define CADKIT_NURBS_MINIMUM_DIMENSION                                    1
#define CADKIT_NURBS_MINIMUM_TOTAL_NUMBER_OF_KNOTS                        4
#define CADKIT_NURBS_MINIMUM_TOTAL_NUMBER_OF_CONTROL_POINTS               2
#define CADKIT_NURBS_MINIMUM_NUMBER_OF_KNOTS                              4
#define CADKIT_NURBS_MINIMUM_NUMBER_OF_CONTROL_POINTS                     2
#define CADKIT_NURBS_MINIMUM_ORDER                                        2
#define CADKIT_NURBS_MINIMUM_DEGREE                                       1


///////////////////////////////////////////////////////////////////////////////
//
//  Parametric fitting.
//
///////////////////////////////////////////////////////////////////////////////

#define CADKIT_NURBS_CHORDAL_FIT                                          1.0
#define CADKIT_NURBS_CENTRIPETAL_FIT                                      0.5


#endif // _CADKIT_NURBS_CORE_LIBRARY_POUND_DEFINES_H_
