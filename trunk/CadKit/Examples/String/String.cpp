
#define CADKIT_DEFINE_SL_TEMPLATE_STRING_SPLIT_INTO_LIST_FUNCTION
#define CADKIT_DEFINE_SL_VECTOR_STD_OSTREAM_OPERATOR

#ifdef _WIN32
#pragma warning(disable:4786) // Truncated debug names.
#endif

#include "Standard/SlString.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlList.h"
#include "Standard/SlStringIO.h"
using namespace CadKit;

#include <iostream>
using namespace std;


void testSplit ( char *splitMe )
{
  cout << "Original = " << splitMe << endl;
  char delim = ';';
  SlAString text ( splitMe );
  SlList<SlAString> parts;
  CadKit::split ( text, delim, parts );
  size_t size = parts.size();
  for ( size_t i = 0; i < size; ++i ) 
    cout << "Component[" << i << "] = " << parts[i] << endl;
}


void testSplit ( wchar_t *splitMe )
{
  wcout << L"Original = " << splitMe << endl;
  wchar_t delim = L';';
  SlWString text ( splitMe );
  SlList<SlWString> parts;
  CadKit::split ( text, delim, parts );
  size_t size = parts.size();
  for ( size_t i = 0; i < size; ++i ) 
    wcout << L"Component[" << i << L"] = " << parts[i] << endl;
}


void testSplit()
{
  SlAString s1 ( "The Quick Brown Fox Jumped Over the Lazy Dog" );
  cout << s1 << endl;
  CadKit::toUpper ( s1 );
  cout << s1 << endl;
  CadKit::toLower ( s1 );
  cout << s1 << endl;

  SlWString s2 ( L"The Quick Brown Fox Jumped Over the Lazy Dog" );
  wcout << s2 << endl;
  CadKit::toUpper ( s2 );
  wcout << s2 << endl;
  CadKit::toLower ( s2 );
  wcout << s2 << endl;

  SlAString s3 (  CadKit::format ( "int = %d, double = %f, string = %s", 2, 2.0,  "hi" ) );
  cout << s3 << endl;

  SlWString s4 ( CadKit::format ( L"int = %d, double = %f, string = %s", 2, 2.0, L"hi" ) );
  wcout << s4 << endl;

  SlAString s5 (  "To be or not to be, that is the question" );
  SlWString s6 ( L"To be or not to be, that is the question" );

  cout  << CadKit::convert ( s6 ) << endl;
  wcout << CadKit::convert ( s5 ) << endl;

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
