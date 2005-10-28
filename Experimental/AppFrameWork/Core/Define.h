
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Macro definitions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_DEFINITIONS_H_
#define _APP_FRAME_WORK_DEFINITIONS_H_



///////////////////////////////////////////////////////////////////////////////
//
//  Macro definitions.
//
///////////////////////////////////////////////////////////////////////////////

#define AFW_CATCH_BLOCK(id0,id1)\
catch ( const std::exception &e )\
{\
  std::cout << "Error " << id0 << ": Standard exception caught." << std::endl;\
  if ( e.what() )\
    std::cout << ". " << e.what() << std::endl;\
}\
catch ( ... )\
{\
  std::cout << "Error " << id1 << ": Unknown exception caught." << std::endl;\
}



#endif // _APP_FRAME_WORK_DEFINITIONS_H_
