
#include <windows.h>
#include <stdio.h>

#include <iostream>
#include <string>


BOOL printOperatingSystemInfo()
{
   // Structure to hold the os info.
   OSVERSIONINFOEX info;

   // Depends on the version.
   BOOL isInfoEx;

   // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
   // If that fails, try using the OSVERSIONINFO structure.
   ::ZeroMemory ( &info, sizeof ( OSVERSIONINFOEX ) );
   info.dwOSVersionInfoSize = sizeof ( OSVERSIONINFOEX );

   // Get the system info.
   if( !( isInfoEx = ::GetVersionEx ( (OSVERSIONINFO *) &info) ) )
   {
      // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
      info.dwOSVersionInfoSize = sizeof ( OSVERSIONINFO );
      if ( !::GetVersionEx ( (OSVERSIONINFO *) &info) ) 
      {
         // Neither one worked.
         return FALSE;
      }
   }

   // Set the string name of the structure used.
   std::string structName ( ( isInfoEx ) ? "OSVERSIONINFOEX" : "OSVERSIONINFO" );

   // Print the info.
   std::cout << structName << ".dwOSVersionInfoSize = " << info.dwOSVersionInfoSize << std::endl;
   std::cout << structName << ".dwMajorVersion      = " << info.dwMajorVersion      << std::endl;
   std::cout << structName << ".dwMinorVersion      = " << info.dwMinorVersion      << std::endl;
   std::cout << structName << ".dwBuildNumber       = " << info.dwBuildNumber       << std::endl;
   std::cout << structName << ".szCSDVersion        = " << info.szCSDVersion        << std::endl;
   std::cout << structName << ".dwPlatformId        = ";

   // Write the string for the platform id.
   switch ( info.dwPlatformId )
   {
   case VER_PLATFORM_WIN32s:
     std::cout << "VER_PLATFORM_WIN32s"        << std::endl;
     break;
   case VER_PLATFORM_WIN32_WINDOWS:
     std::cout << "VER_PLATFORM_WIN32_WINDOWS" << std::endl;
     break;
   case VER_PLATFORM_WIN32_NT:
     std::cout << "VER_PLATFORM_WIN32_NT"      << std::endl;
     break;
   default:
     std::cout << info.dwPlatformId            << std::endl;
     break;
   }

   // If not OSVERSIONINFOEX...
   if ( FALSE == isInfoEx )
     return TRUE;

   // Print additional info.
    std::cout << structName << ".wServicePackMajor   = " << info.wServicePackMajor << std::endl;
    std::cout << structName << ".wServicePackMinor   = " << info.wServicePackMinor << std::endl;
    std::cout << structName << ".wSuiteMask          = " << info.wSuiteMask;

    std::cout << std::endl;
    switch ( info.wSuiteMask )
    {
    case VER_SERVER_NT:
      std::cout << structName << ".wSuiteMask          = VER_SERVER_NT"                      << std::endl;
      break;
    case VER_WORKSTATION_NT:
      std::cout << structName << ".wSuiteMask          = VER_WORKSTATION_NT"                 << std::endl;
      break;
    case VER_SUITE_SMALLBUSINESS:
      std::cout << structName << ".wSuiteMask          = VER_SUITE_SMALLBUSINESS"            << std::endl;
      break;
    case VER_SUITE_ENTERPRISE:
      std::cout << structName << ".wSuiteMask          = VER_SUITE_ENTERPRISE"               << std::endl;
      break;
    case VER_SUITE_BACKOFFICE:
      std::cout << structName << ".wSuiteMask          = VER_SUITE_BACKOFFICE"               << std::endl;
      break;
    case VER_SUITE_COMMUNICATIONS:
      std::cout << structName << ".wSuiteMask          = VER_SUITE_COMMUNICATIONS"           << std::endl;
      break;
    case VER_SUITE_TERMINAL:
      std::cout << structName << ".wSuiteMask          = VER_SUITE_TERMINA"                  << std::endl;
      break;
    case VER_SUITE_SMALLBUSINESS_RESTRICTED:
      std::cout << structName << ".wSuiteMask          = VER_SUITE_SMALLBUSINESS_RESTRICTED" << std::endl;
      break;
    case VER_SUITE_EMBEDDEDNT:
      std::cout << structName << ".wSuiteMask          = VER_SUITE_EMBEDDEDNT"               << std::endl;
      break;
    case VER_SUITE_DATACENTER:
      std::cout << structName << ".wSuiteMask          = VER_SUITE_DATACENTER"               << std::endl;
      break;
    case VER_SUITE_SINGLEUSERTS:
      std::cout << structName << ".wSuiteMask          = VER_SUITE_SINGLEUSERTS"             << std::endl;
      break;
    case VER_SUITE_PERSONAL:
      std::cout << structName << ".wSuiteMask          = VER_SUITE_PERSONA"                  << std::endl;
      break;
    case VER_SUITE_BLADE:
      std::cout << structName << ".wSuiteMask          = VER_SUITE_BLADE"                    << std::endl;
      break;
    default:
      std::cout << structName << ".wSuiteMask          = " << info.wSuiteMask                << std::endl;
      break;
    }

    std::cout << structName << ".wProductType        = ";

    // Write the string for the product id.
    switch ( info.wProductType )
    {
    case VER_NT_WORKSTATION:
      std::cout << "VER_NT_WORKSTATION"       << std::endl;
      break;
    case VER_NT_DOMAIN_CONTROLLER:
      std::cout << "VER_NT_DOMAIN_CONTROLLER" << std::endl;
      break;
    case VER_NT_SERVER:
      std::cout << "VER_NT_SERVER"            << std::endl;
      break;
    default:
      std::cout << info.wProductType          << std::endl;
      break;
    }

    std::cout << structName << ".wReserved           = " << info.wReserved << std::endl;

   // It worked.
   return TRUE; 
}


int main ( int argc, char **argv )
{
  if ( FALSE == printOperatingSystemInfo() )
  {
    std::cout << "Failed to get operating system information\nGetLastError() returned = " << ::GetLastError() << std::endl;
    return 1;
  }

  return 0;
}
