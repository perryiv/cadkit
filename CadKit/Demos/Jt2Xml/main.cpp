
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A Jupiter to XML translator.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning(disable:4251) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# pragma warning(disable:4786) // Truncated debug names.
#endif

#include "Translators/Jupiter2Xml/TrJt2Xml.h"
#include "Database/XML/DbXmlWrite.h"
#include "Standard/SlPathname.h"
#include <string>
#include <iostream>

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Translate the file.
//
///////////////////////////////////////////////////////////////////////////////

bool _translate ( const char *filename, TrJt2Xml &jt2xml )
{
  SL_ASSERT ( filename );
  std::cout << "Translating: " << filename << std::endl;

  // Declare an XML group.
  DbXmlGroup::Ptr root = new DbXmlGroup ( "jt2xml" );

  // Translate.
  if ( false == jt2xml.translate ( filename, *root ) )
  {
    std::cout << jt2xml.getLastError() << std::endl;
    std::cout << "Failed to translate: " << filename << std::endl;
    return false;
  }

  // The output filename should have the same path as the input filename, 
  // except with a different extension.
  std::string name ( filename );
  SL_ASSERT ( name.size() - 3 == name.find ( ".jt" ) );
  name.resize ( name.size() - 2 );
  name += "xml";

  std::cout << "Writing:     " << name << std::endl;

  // Declare an XML writer.
  DbXmlWrite::Ptr out = new DbXmlWrite;

//#ifdef _DEBUG
//  // Write only the names (no values) to the file.
//  out->setMode ( DbXmlWrite::WRITE_NAME_ONLY );
//#endif

  // Write the XML tree to file.
  if ( false == out->write ( *root, name.c_str() ) )
  {
    std::cout << "Failed to write: " << name << std::endl;
    return false;
  }

  std::cout << "Done with:   " << filename << std::endl;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // There should be at least one argument.
  if ( argc < 2 )
  {
    std::cout << "Usage: " << CadKit::justFilename ( std::string ( argv[0] ) );
    std::cout << "<filename1.jt> [filename2.jt] ..." << std::endl;
    return 0;
  }

  // Declare an instance of the Jupiter to XML translator.
  TrJt2Xml jt2xml;

  // Initialize.
  if ( false == jt2xml.init() )
  {
    std::cout << "Failed to initialize translator" << std::endl;
    return 0;
  }

  // Loop through all the input files.
  for ( int i = 1; i < argc; ++i )
  {
    // Translate the jupiter database.
    ::_translate ( argv[i], jt2xml );
  }

  // It worked.
  return 1;
}
