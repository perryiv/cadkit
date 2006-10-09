
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ReadFile.h"

#include "osg/ref_ptr"
#include "osg/Node"
#include "osgDB/ReadFile"

using namespace CadKit::OSG::Glue;

namespace Detail
{
  std::string toNativeString( System::String^ source )
  {
    System::IntPtr ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( source );
    char* s = (char*)(void*) ptr;
    
    std::string string ( s );
    
    System::Runtime::InteropServices::Marshal::FreeHGlobal( ptr );

    return string;
  }
}

Node^ ReadFile::readNodeFile( System::String ^filename )
{
  Node ^node = gcnew Node();

  osg::ref_ptr < osg::Node > nativeNode = osgDB::readNodeFile( Detail::toNativeString ( filename ) );

  node->node ( nativeNode.get() );

  return node;
}
