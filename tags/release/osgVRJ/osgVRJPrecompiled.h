
///////////////////////////////////////////////////////////////////////////////
//
//  Insert copyright here.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The precompiled header file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_VRJ_INTERNAL_PRECOMPILED_HEADERS_H_A
#define _OSG_VRJ_INTERNAL_PRECOMPILED_HEADERS_H_A

#ifndef _COMPILING_OSG_VRJ
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif


#ifdef _MSC_VER                     // VC++
# pragma warning ( disable : 4290 ) // Exception specification ignored.
# pragma warning ( disable : 4786 ) // Truncated debug names.
# define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX                   // Do not define min and max as macros.
# include <windows.h>               // Base Windows header file.
# include <osgUtil/RenderStage>
# include <osgUtil/RenderBin>
# include <osg/FrameStamp>
# include <osg/LightModel>
# include <osg/MatrixTransform>
# include <osgUtil/UpdateVisitor>
# include <vrj/Kernel/Kernel.h>
#endif // _MSC_VER

#endif // _OSG_VRJ_INTERNAL_PRECOMPILED_HEADERS_H_A
