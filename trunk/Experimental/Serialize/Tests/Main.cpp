
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
#include "Serialize/XML/SimpleDataMember.h"
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
#include <vector>
#include <list>


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

  void name ( const std::string &n )
  {
    _name = n;
  }

  const std::string &name() const
  {
    return _name;
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
//  Class to be serialized.
//
///////////////////////////////////////////////////////////////////////////////

class ClassC : public ClassA
{
public:

  USUL_DECLARE_REF_POINTERS ( ClassC );
  typedef ClassA BaseClass;

  ClassC() : BaseClass(),
    _b ( new ClassB )
  {
    _name = "Default name for ClassC";
    SERIALIZE_XML_ADD_MEMBER ( _b );
  }

protected:

  virtual ~ClassC()
  {
  }

private:

  ClassB::RefPtr _b;

  SERIALIZE_XML_DEFINE_MEMBERS ( ClassC );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Class to be serialized.
//
///////////////////////////////////////////////////////////////////////////////

class ClassD : public ClassB
{
public:

  USUL_DECLARE_REF_POINTERS ( ClassD );
  typedef ClassB BaseClass;

  ClassD() : BaseClass(),
    _c ( new ClassC ),
    _id1D ( ::rand() ),
    _id2D ( ::rand() )
  {
    _name = "Default name for ClassD";
    SERIALIZE_XML_ADD_MEMBER ( _c );
    SERIALIZE_XML_ADD_MEMBER ( _id1D );
    SERIALIZE_XML_ADD_MEMBER ( _id2D );
  }

protected:

  virtual ~ClassD()
  {
  }

private:

  ClassC::RefPtr _c;
  double _id1D;
  double _id2D;

  SERIALIZE_XML_DEFINE_MEMBERS ( ClassD );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Class to be serialized.
//
///////////////////////////////////////////////////////////////////////////////

class ClassE : public ClassD
{
public:

  USUL_DECLARE_REF_POINTERS ( ClassE );
  typedef ClassD BaseClass;
  typedef std::map<std::string,ClassC::RefPtr> MapC;
  typedef std::map<std::string,ClassA::RefPtr> MapA;

  ClassE() : BaseClass(),
    _map1(),
    _map2()
  {
    _name = "Default name for ClassE";

    SERIALIZE_XML_ADD_MEMBER ( _map1 );
    _map1["Key1"] = new ClassC;
    _map1["Key2"] = new ClassC;
    _map1["Key3"] = new ClassC;
    _map1["Key4"] = new ClassC;

    SERIALIZE_XML_ADD_MEMBER ( _map2 );
    _map2["KeyA"] = new ClassB;
    _map2["KeyB"] = new ClassC;
    _map2["KeyC"] = new ClassD;
  }

protected:

  virtual ~ClassE()
  {
  }

private:

  MapC _map1;
  MapA _map2;

  SERIALIZE_XML_DEFINE_MEMBERS ( ClassE );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Class to be serialized.
//
///////////////////////////////////////////////////////////////////////////////

class ClassF : public ClassA
{
public:

  USUL_DECLARE_REF_POINTERS ( ClassF );
  typedef ClassA BaseClass;
  typedef std::vector<ClassB::RefPtr> VectorB;
  typedef std::list<ClassE::RefPtr> ListE;

  ClassF() : BaseClass(),
    _vecB(),
    _listE()
  {
    _name = "Default name for ClassF";

    SERIALIZE_XML_ADD_MEMBER ( _vecB );
    SERIALIZE_XML_ADD_MEMBER ( _listE );

    for ( unsigned int i = 0; i < 10; ++i )
    {
      _vecB.push_back ( new ClassB );
      _vecB.push_back ( new ClassD );
      _listE.push_back ( new ClassE );
    }
  }

protected:

  virtual ~ClassF()
  {
  }

private:

  VectorB _vecB;
  ListE _listE;

  SERIALIZE_XML_DEFINE_MEMBERS ( ClassF );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Register some types.
//
///////////////////////////////////////////////////////////////////////////////

SERIALIZE_XML_REGISTER_CREATOR ( ClassA );
SERIALIZE_XML_REGISTER_CREATOR ( ClassB );
SERIALIZE_XML_REGISTER_CREATOR ( ClassC );
SERIALIZE_XML_REGISTER_CREATOR ( ClassD );
SERIALIZE_XML_REGISTER_CREATOR ( ClassE );
SERIALIZE_XML_REGISTER_CREATOR ( ClassF );


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
  for ( unsigned j = 0; j < 10; ++j )
  {
    const std::string names[] = { "ClassA", "ClassB", "ClassC", "ClassD", "ClassE", "ClassF", "ClassG" };
    const unsigned int num ( sizeof ( names ) / sizeof ( std::string ) );

    objects.reserve ( num );

    for ( unsigned int i = 0; i < num; ++i )
    {
      ClassA::RefPtr object ( dynamic_cast < ClassA * > ( Serialize::XML::Factory::instance().create ( names[i] ) ) );
      if ( true == object.valid() )
      {
        std::cout << names[i] << " exists" << std::endl;
        objects.push_back ( object );
        std::ostringstream out; out << "Object " << j * num + i;
        object->name ( out.str() );
      }
      else
      {
        std::cout << names[i] << " does not exist" << std::endl;
      }
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
  //std::cin.get();
  return 0;
}
