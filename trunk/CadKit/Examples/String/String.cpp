#ifdef _WIN32
#pragma warning(disable:4786) // Truncated debug names.
#endif

#include "Standard/SlString.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlStringCase.h"
#include "Standard/SlStringIO.h"
#include <iostream>
#include <vector>

using namespace CadKit;


void testSplit ( char *splitMe )
{
  std::cout << "B01: Original = " << splitMe << std::endl;
  char delim = ';';
  SlAString text ( splitMe );
  std::vector<SlAString> parts;
  CadKit::split ( text, delim, parts );
  size_t size = parts.size();
  for ( size_t i = 0; i < size; ++i ) 
    std::cout << "B02: Component[" << i << "] = " << parts[i] << std::endl;
}


void testSplit ( wchar_t *splitMe )
{
  std::wcout << L"C01: Original = " << splitMe << std::endl;
  wchar_t delim = L';';
  SlWString text ( splitMe );
  std::vector<SlWString> parts;
  CadKit::split ( text, delim, parts );
  size_t size = parts.size();
  for ( size_t i = 0; i < size; ++i ) 
    std::wcout << L"C02: Component[" << i << L"] = " << parts[i] << std::endl;
}


void testSplit()
{
  SlAString s1 ( "The Quick Brown Fox Jumped Over the Lazy Dog" );
  std::cout << "A01: " << s1 << std::endl;
  CadKit::toUpper ( s1 );
  std::cout << "A02: " << s1 << std::endl;
  CadKit::toLower ( s1 );
  std::cout << "A03: " << s1 << std::endl;

  SlWString s2 ( L"The Quick Brown Fox Jumped Over the Lazy Dog" );
  std::wcout << L"A04: " << s2 << std::endl;
  CadKit::toUpper ( s2 );
  std::wcout << L"A05: " << s2 << std::endl;
  CadKit::toLower ( s2 );
  std::wcout << L"A06: " << s2 << std::endl;

  SlAString s3 (  CadKit::getString ( "int = %d, double = %f, string = %s", 2, 2.0,  "hi" ) );
  std::cout << "A07: " << s3 << std::endl;

  SlWString s4 ( CadKit::getString ( L"int = %d, double = %f, string = %s", 2, 2.0, L"hi" ) );
  std::wcout << L"A08: " << s4 << std::endl;

  SlAString s5 (  "To be or not to be, that is the question" );
  SlWString s6 ( L"To be or not to be, that is the question" );

  std::cout  <<  "A09: " << CadKit::convert ( s6 ) << std::endl;
  std::wcout << L"A10: " << CadKit::convert ( s5 ) << std::endl;

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

  CadKit::toLower ( s1 );
  std::cout << "A11: " << s1 << std::endl;
  s1.replace ( "quick", "harry" );
  std::cout << "A12: " << s1 << std::endl;
  s1.replace ( "fox", "lion" );
  std::cout << "A13: " << s1 << std::endl;
  s1.replace ( "jumped", "ran" );
  std::cout << "A14: " << s1 << std::endl;
  s1.replace ( "cat", "bird" );
  std::cout << "A15: " << s1 << std::endl;
  s1.replace ( " ", "_" );
  std::cout << "A16: " << s1 << std::endl;

  CadKit::toLower ( s2 );
  std::wcout << L"A17: " << s2 << std::endl;
  s2.replace ( L"quick", L"harry" );
  std::wcout << L"A18: " << s2 << std::endl;
  s2.replace ( L"fox", L"lion" );
  std::wcout << L"A19: " << s2 << std::endl;
  s2.replace ( L"jumped", L"ran" );
  std::wcout << L"A20: " << s2 << std::endl;
  s2.replace ( L"cat", L"bird" );
  std::wcout << L"A21: " << s2 << std::endl;
  s2.replace ( L" ", L"-" );
  std::wcout << L"A21: " << s2 << std::endl;
}


int main ( int argc, char **argv )
{
  testSplit();

  // Wait for user to press the 'any' key.
  //std::cin.get();

  return 0;
}
