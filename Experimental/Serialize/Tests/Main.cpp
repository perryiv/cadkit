
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test program for serialization.
//
///////////////////////////////////////////////////////////////////////////////

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"
#include "Serialize/XML/RegisterCreator.h"
#include "Serialize/XML/DataMember.h"
#include "Serialize/XML/DataMemberMap.h"
#include "Serialize/XML/Macros.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Errors/Assert.h"
#include "Usul/File/Contents.h"
#include "Usul/File/Path.h"
#include "Usul/File/Remove.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/Mutex.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <map>


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize mutex factory.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Usul::Threads::newSingleThreadedMutexStub );


///////////////////////////////////////////////////////////////////////////////
//
//  Constants.
//
///////////////////////////////////////////////////////////////////////////////

const std::string FILE_NAME_1 ( "out_1.xml" );
const std::string FILE_NAME_2 ( "out_2.xml" );


///////////////////////////////////////////////////////////////////////////////
//
//  Class to be serialized.
//
///////////////////////////////////////////////////////////////////////////////

class ClassA : public Usul::Base::Referenced
{
public:

  USUL_DECLARE_REF_POINTERS ( ClassA );
  typedef Usul::Base::Referenced BaseClass;

  ClassA() : BaseClass(),
    SERIALIZE_XML_INITIALIZER_LIST,
    _name ( "Default name for ClassA" )
  {
    this->_addMember ( "_name", _name );
  }

protected:

  virtual ~ClassA()
  {
  }

  std::string _name;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( ClassA );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Class to be serialized.
//
///////////////////////////////////////////////////////////////////////////////

class ClassB : public ClassA
{
public:

  USUL_DECLARE_REF_POINTERS ( ClassB );
  typedef ClassA BaseClass;

  ClassB() : BaseClass(),
    _id1 ( ::rand() ),
    _id2 ( ::rand() ),
    _id3 ( ::rand() ),
    _id4 ( ::rand() )
  {
    _name = "Default name for ClassB";
    SERIALIZE_XML_ADD_MEMBER ( _id1 );
    SERIALIZE_XML_ADD_MEMBER ( _id2 );
    SERIALIZE_XML_ADD_MEMBER ( _id3 );
    SERIALIZE_XML_ADD_MEMBER ( _id4 );
  }

protected:

  virtual ~ClassB()
  {
  }

private:

  unsigned int _id1;
  double _id2;
  double _id3;
  int _id4;

  SERIALIZE_XML_DEFINE_MEMBERS ( ClassB );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Register some types.
//
///////////////////////////////////////////////////////////////////////////////

SERIALIZE_XML_REGISTER_CREATOR ( ClassA );
SERIALIZE_XML_REGISTER_CREATOR ( ClassB );


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _run()
{
  typedef std::vector < ClassA::RefPtr > Objects;
  Objects objects;

  // Create objects using factory.
  {
    const std::string names[] = { "ClassA", "ClassB", "ClassC" };
    const unsigned int num ( sizeof ( names ) / sizeof ( std::string ) );

    objects.reserve ( num );

    for ( unsigned int i = 0; i < num; ++i )
    {
      ClassA::RefPtr object ( dynamic_cast < ClassA * > ( Serialize::XML::Factory::instance().create ( names[i] ) ) );
      std::cout << names[i] << ( ( object.valid() ) ? " exists" : " does not exist" ) << std::endl;
      objects.push_back ( object );
    }
  }

  // Write objects to file.
  {
    Serialize::XML::serialize ( FILE_NAME_1, "objects", objects.begin(), objects.end() );
  }

  // Read objects from first file and write to a second file.
  {
    objects.clear();
    Serialize::XML::deserialize ( FILE_NAME_1, objects );
    Serialize::XML::serialize ( FILE_NAME_2, "objects", objects.begin(), objects.end() );
  }

  // Compare contents of the two files.
  {
    std::string file1, file2;
    Usul::File::contents ( FILE_NAME_1, false, file1 );
    Usul::File::contents ( FILE_NAME_2, false, file2 );
    USUL_ASSERT ( file1.compare ( file2 ) == 0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.
//
///////////////////////////////////////////////////////////////////////////////

void _clean()
{
  Serialize::XML::Factory::instance ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  ::srand ( static_cast < unsigned int > ( ::time ( 0x0 ) ) );
  Usul::Functions::safeCall ( _run,   "3566023837" );
  Usul::Functions::safeCall ( _clean, "3022189564" );
  std::cin.get();
  return 0;
}
