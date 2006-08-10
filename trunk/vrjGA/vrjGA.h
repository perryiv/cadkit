
///////////////////////////////////////////////////////////////////////////////
//
//  Insert copyright here.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what GSG_CORE_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRJGA_DLL_IMPORT_EXPORT_H_
#define _VRJGA_DLL_IMPORT_EXPORT_H_


#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Turn off this warning, it doesn't apply.
# pragma warning ( disable : 4251 ) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# ifdef _COMPILING_VRJ_GA
#   define VRJ_GA_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define VRJ_GA_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define VRJ_GA_EXPORT
#endif


#endif // _VRJGA_DLL_IMPORT_EXPORT_H_
