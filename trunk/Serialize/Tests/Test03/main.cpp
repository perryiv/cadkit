

///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test program for serializing osg nodes. 
//
///////////////////////////////////////////////////////////////////////////////

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
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/Mutex.h"

#include "XmlTree/XercesLife.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"
#include "Serialize/XML/RegisterCreator.h"
#include "Serialize/XML/SimpleDataMember.h"
#include "Serialize/XML/DataMemberMap.h"
#include "Serialize/XML/Macros.h"
#include "Serialize/XML/SmartPointerMember.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Errors/Assert.h"
#include "Usul/File/Contents.h"
#include "Usul/File/Path.h"
#include "Usul/File/Remove.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Pointers/SmartPointer.h"
#include "Usul/Threads/Mutex.h"

#include "osg/Node"
#include "osg/Group"
#include "osg/Referenced"

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
//  Constants.
//
///////////////////////////////////////////////////////////////////////////////

const std::string FILE_NAME_IN ( "input.xml" );
const std::string FILE_NAME_OUT ( "output.xml" );

///////////////////////////////////////////////////////////////////////////////
//
//  Set mutex factory.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Usul::Threads::newSingleThreadedMutexStub );


///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper around osg::Node.
//
///////////////////////////////////////////////////////////////////////////////

namespace Test
{
  class AppNode: public osg::Node, public Usul::Base::Referenced
  {
    public:

      USUL_DECLARE_REF_POINTERS( AppNode );

      AppNode() : osg::Node(),
        SERIALIZE_XML_INITIALIZER_LIST, 
        _id( "" )
      {
        this->_addMember( "_id", _id );  
      }

      void id( const std::string& id )
      {
        _id = id;
      }

      const std::string& id() const
      {
        return _id;
      }

      // Overriding this funtion to resolve the conflict between Usul::Referenced::ref and osg::ref. 
      virtual void ref() 
      {
        Usul::Base::Referenced::ref();
      }

      // Overriding this funtion to resolve the conflict between Usul::Referenced::ref and osg::ref. 
      virtual void unref() 
      {
        Usul::Base::Referenced::unref();
      }

    protected:

      virtual ~AppNode()
      {
      }

      std::string       _id;      

      SERIALIZE_XML_DEFINE_MAP;
      SERIALIZE_XML_DEFINE_MEMBERS ( AppNode );
  };

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Wrapper around osg::Group.
  //
  ///////////////////////////////////////////////////////////////////////////////
  class AppGroup : public AppNode, public osg::Group
  {
    public: 

      USUL_DECLARE_REF_POINTERS( AppGroup );

      typedef AppNode::RefPtr               NodePtr;      
      typedef std::vector< NodePtr >        NodePtrs;

      AppGroup() : AppNode(), osg::Group()
      { 
        SERIALIZE_XML_ADD_MEMBER( _nodes );       
      }     

    protected:       

      virtual ~AppGroup()
      {
      }
      
     NodePtrs                             _nodes;     
     
     SERIALIZE_XML_DEFINE_MEMBERS ( AppGroup );     
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register all types.
//
///////////////////////////////////////////////////////////////////////////////

void _registerTypes()
{
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator< Test::AppNode > ( "AppNode" ) );
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator< Test::AppGroup >( "AppGroup" ) );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _test3()
{
  typedef std::vector< Test::AppNode::RefPtr > Objects;
  Objects objects;

  // Read an XML string.
  std::string xml;
  Usul::File::contents( FILE_NAME_IN, false, xml );
  std::istringstream in ( xml );
    
  Serialize::XML::deserialize( in, objects );
  Serialize::XML::serialize ( FILE_NAME_OUT, "objects", objects.begin(), objects.end() );
}

void _run()
{
  // Register types.
  _registerTypes();

  // Test.
  _test3();
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
