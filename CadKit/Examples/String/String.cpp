#ifdef _WIN32
#pragma warning(disable:4786) // Truncated debug names.
#endif

#include "Standard/SlString.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlStringIO.h"
#include <iostream>
#include <vector>

using namespace CadKit;


void testSplit ( char *splitMe )
{
  std::cout << "Original = " << splitMe << std::endl;
  char delim = ';';
  SlAString text ( splitMe );
  std::vector<SlAString> parts;
  CadKit::split ( text, delim, parts );
  size_t size = parts.size();
  for ( size_t i = 0; i < size; ++i ) 
    std::cout << "Component[" << i << "] = " << parts[i] << std::endl;
}


void testSplit ( wchar_t *splitMe )
{
  std::wcout << L"Original = " << splitMe << std::endl;
  wchar_t delim = L';';
  SlWString text ( splitMe );
  std::vector<SlWString> parts;
  CadKit::split ( text, delim, parts );
  size_t size = parts.size();
  for ( size_t i = 0; i < size; ++i ) 
    std::wcout << L"Component[" << i << L"] = " << parts[i] << std::endl;
}


void testSplit()
{
  SlAString s1 ( "The Quick Brown Fox Jumped Over the Lazy Dog" );
  std::cout << s1 << std::endl;
  CadKit::toUpper ( s1 );
  std::cout << s1 << std::endl;
  CadKit::toLower ( s1 );
  std::cout << s1 << std::endl;

  SlWString s2 ( L"The Quick Brown Fox Jumped Over the Lazy Dog" );
  std::wcout << s2 << std::endl;
  CadKit::toUpper ( s2 );
  std::wcout << s2 << std::endl;
  CadKit::toLower ( s2 );
  std::wcout << s2 << std::endl;

  SlAString s3 (  CadKit::getString ( "int = %d, double = %f, string = %s", 2, 2.0,  "hi" ) );
  std::cout << s3 << std::endl;

  SlWString s4 ( CadKit::getString ( L"int = %d, double = %f, string = %s", 2, 2.0, L"hi" ) );
  std::wcout << s4 << std::endl;

  SlAString s5 (  "To be or not to be, that is the question" );
  SlWString s6 ( L"To be or not to be, that is the question" );

  std::cout  << CadKit::convert ( s6 ) << std::endl;
  std::wcout << CadKit::convert ( s5 ) << std::endl;

  testSplit ( "The;Quick;Brown;Fox.Jumped.Over;the;Lazy;Dog" );
  testSplit ( "Perry Miller" );
  testSplit ( "Perry" );
  testSplit ( ";Perry;" );
  testSplit ( ";;Perry;;" );
  testSplit ( ";;;Perry;;;" );

  testSplit ( L"The;Quick;Brown;Fox.Jumped.Over;the;Lazy;Dog" );
  testSplit ( L"Perry Miller" );
  testSplit ( L"Perry" );
  testSplit ( L";Perry;" );
  testSplit ( L";;Perry;;" );
  testSplit ( L";;;Perry;;;" );
}


int main ( int argc, char **argv )
{
  testSplit();

  // Wait for user to press the 'any' key.
  //std::cin.get();

  return 0;
}
