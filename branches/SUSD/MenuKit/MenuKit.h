// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MENUKIT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MENUKIT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MENUKIT_EXPORTS
#define MENUKIT_API __declspec(dllexport)
#else
#define MENUKIT_API __declspec(dllimport)
#endif

// This class is exported from the MenuKit.dll
class MENUKIT_API CMenuKit {
public:
	CMenuKit(void);
	// TODO: add your methods here.
};

extern MENUKIT_API int nMenuKit;

MENUKIT_API int fnMenuKit(void);
