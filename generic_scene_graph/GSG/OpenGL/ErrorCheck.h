
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Checks for a GL error in the constructor and destructor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_OPENGL_ERROR_CHECKING_CLASS_H_
#define _GENERIC_SCENE_GRAPH_OPENGL_ERROR_CHECKING_CLASS_H_


namespace GSG {
namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Function used to actually do the checking.
//
///////////////////////////////////////////////////////////////////////////////

void checkOpenGlErrorState();


///////////////////////////////////////////////////////////////////////////////
//
//  Generic definition.
//
///////////////////////////////////////////////////////////////////////////////

template < bool check_ > class ErrorCheck
{
  static void check(){}
};


///////////////////////////////////////////////////////////////////////////////
//
//  Specialization.
//
///////////////////////////////////////////////////////////////////////////////

template <> class ErrorCheck < true >
{
public:

  ErrorCheck()
  {
    this->check();
  }

  ~ErrorCheck()
  {
    this->check();
  }

  static void check()
  {
    GSG::Detail::checkOpenGlErrorState();
  }
};


}; // namespace Detail
}; // namespace GSG


///////////////////////////////////////////////////////////////////////////////
//
//  Configure OpenGL error checking here.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define GSG_GL_ERROR_CHECKER   GSG::Detail::ErrorCheck<true> _temp_instance_of_ErrorCheck
#define GSG_GL_ERROR_CHECK_NOW GSG::Detail::ErrorCheck<true>::check()
#else
#define GSG_GL_ERROR_CHECKER   GSG::Detail::ErrorCheck<false>
#define GSG_GL_ERROR_CHECK_NOW GSG::Detail::ErrorCheck<false>::check()
#endif


#endif // _GENERIC_SCENE_GRAPH_OPENGL_ERROR_CHECKING_CLASS_H_
