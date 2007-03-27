
#define CADKIT_DEFINE_SL_TEMPLATE_STRING_SPLIT_INTO_LIST_FUNCTION
#define CADKIT_DEFINE_SL_VECTOR_STD_OSTREAM_OPERATOR

#ifdef _WIN32
#pragma warning(disable:4786) // Truncated debug names.
#endif

#include "Standard/SlPathname.h"
#include "Standard/SlString.h"
#include "Standard/SlStringIO.h"
using namespace CadKit;

#include <iostream>

// Works with either, take your pick (except VC++ using STLport).
#if 1
typedef std::basic_string<char>    StringA;
typedef std::basic_string<wchar_t> StringW;
#else
typedef SlString<char>             StringA;
typedef SlString<wchar_t>          StringW;
#endif

typedef SlPathname<StringA>      SlAPathname;
typedef SlPathname<StringW>      SlWPathname;


void test ( const char *filename )
{
  SlAPathname path1 ( filename );
  std::cout << " pathname: " << path1.getPathname() << std::endl;
  std::cout << " fullpath: " << path1.getFullpath() << std::endl;
  std::cout << "    drive: " << path1.getDrive() << std::endl;
  std::cout << "directory: " << path1.getDirectory() << std::endl;
  std::cout << " filename: " << path1.getFilename() << std::endl;
  std::cout << "extension: " << path1.getExtension() << std::endl;
  std::cout << std::endl;

  SlAPathname path2 ( "../../test" );
  std::cout << " pathname: " << path2.getPathname() << std::endl;
  std::cout << " fullpath: " << path2.getFullpath() << std::endl;
  std::cout << "    drive: " << path2.getDrive() << std::endl;
  std::cout << "directory: " << path2.getDirectory() << std::endl;
  std::cout << " filename: " << path2.getFilename() << std::endl;
  std::cout << "extension: " << path2.getExtension() << std::endl;
  std::cout << std::endl;

  std::cout << "path1.isSamePath ( path2 ) = " << path1.isSamePath ( path2 ) << std::endl;
  std::cout << "path2.isSamePath ( path1 ) = " << path2.isSamePath ( path1 ) << std::endl;
  path2.setPathname ( path1 );
  std::cout << "path1.isSamePath ( path2 ) = " << path1.isSamePath ( path2 ) << std::endl;
  std::cout << "path2.isSamePath ( path1 ) = " << path2.isSamePath ( path1 ) << std::endl;
  std::cout << std::endl;

  SlWPathname path3 ( L"./test3" );
  std::wcout << L" pathname: " << path3.getPathname() << std::endl;
  std::wcout << L" fullpath: " << path3.getFullpath() << std::endl;
  std::wcout << L"    drive: " << path3.getDrive() << std::endl;
  std::wcout << L"directory: " << path3.getDirectory() << std::endl;
  std::wcout << L" filename: " << path3.getFilename() << std::endl;
  std::wcout << L"extension: " << path3.getExtension() << std::endl;
  std::wcout << std::endl;

  SlWPathname path4 ( L"./test4" );
  std::wcout << L" pathname: " << path4.getPathname() << std::endl;
  std::wcout << L" fullpath: " << path4.getFullpath() << std::endl;
  std::wcout << L"    drive: " << path4.getDrive() << std::endl;
  std::wcout << L"directory: " << path4.getDirectory() << std::endl;
  std::wcout << L" filename: " << path4.getFilename() << std::endl;
  std::wcout << L"extension: " << path4.getExtension() << std::endl;
  std::wcout << std::endl;

  std::wcout << L"path3.isSamePath ( path4 ) = " << path3.isSamePath ( path4 ) << std::endl;
  std::wcout << L"path4.isSamePath ( path3 ) = " << path4.isSamePath ( path3 ) << std::endl;
  path4.setPathname ( path3 );
  std::wcout << L"path3.isSamePath ( path4 ) = " << path3.isSamePath ( path4 ) << std::endl;
  std::wcout << L"path4.isSamePath ( path3 ) = " << path4.isSamePath ( path3 ) << std::endl;
  std::cout << std::endl;
}


int main ( int argc, char **argv )
{
  test ( argv[0] );

  // Wait for user to press the 'any' key.
  std::cin.get();

  return 0;
}
