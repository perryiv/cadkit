
#define CADKIT_DEFINE_SL_TEMPLATE_STRING_SPLIT_INTO_LIST_FUNCTION
#define CADKIT_DEFINE_SL_VECTOR_STD_OSTREAM_OPERATOR

#include "Standard/SlConvertStrings.h"
#include "Standard/SlUnicode.h"
#include "Standard/SlList.h"
using namespace CadKit;

#include <iostream>
using namespace std;


void testSplit ( char *splitMe )
{
  cout << "Original = " << splitMe << endl;
  char adelim = ';';
  SlAString text ( splitMe );
  SlList<SlAString> alist;
  text.split ( adelim, alist );
  size_t size = alist.size();
  for ( size_t i = 0; i < size; ++i ) 
    cout << "Component[" << i << "] = " << alist[i] << endl;
}


void testSplit ( wchar_t *splitMe )
{
  wcout << L"Original = " << splitMe << endl;
  wchar_t adelim = L';';
  SlWString text ( splitMe );
  SlList<SlWString> wlist;
  text.split ( adelim, wlist );
  size_t size = wlist.size();
  for ( size_t i = 0; i < size; ++i ) 
    wcout << L"Component[" << i << L"] = " << wlist[i] << endl;
}


void testSplit()
{
  SlAString s1 ( "The Quick Brown Fox Jumped Over the Lazy Dog" );
  cout << s1 << endl;
  s1.toUpper ( true );
  cout << s1 << endl;
  s1.toLower ( true );
  cout << s1 << endl;

  SlWString s2 ( L"The Quick Brown Fox Jumped Over the Lazy Dog" );
  wcout << s2 << endl;
  s2.toUpper ( true );
  wcout << s2 << endl;
  s2.toLower ( true );
  wcout << s2 << endl;

  SlAString s3 (  "int = %d, double = %f, string = %s", 2, 2.0, "hi" );
  cout << s3 << endl;

  SlWString s4 ( L"int = %d, double = %f, string = %s", 2, 2.0, L"hi" );
  wcout << s4 << endl;

  SlAString s5 (  "To be or not to be, that is the question" );
  SlWString s6 ( L"To be or not to be, that is the question" );

  cout  << CadKit::unicodeToAnsi ( s6 ) << endl;
  wcout << CadKit::ansiToUnicode ( s5 ) << endl;

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
  std::cin.get();

  return 0;
}
