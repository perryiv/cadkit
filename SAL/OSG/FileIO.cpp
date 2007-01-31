
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for reading and writing a node.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "FileIO.h"
#include "Transform.h"
#include "Factory.h"

#include "SAL/Common/Exceptions.h"
#include "SAL/Interfaces/INode.h"

#include "Usul/Exceptions/Thrower.h"

#include "osg/MatrixTransform"

#include "osgDB/ReadFile"
#include "osgDB/WriteFile"

using namespace SAL;
using namespace SAL::OSG;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( FileIO, FileIO::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FileIO::FileIO() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FileIO::~FileIO()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *FileIO::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IRead::IID:
    return static_cast<Usul::Interfaces::IRead*>(this);
  case Usul::Interfaces::IWrite::IID:
    return static_cast<Usul::Interfaces::IWrite*>(this);
  case Usul::Interfaces::IUnknown::IID:
    return static_cast<Usul::Interfaces::IUnknown*>(static_cast<Usul::Interfaces::IRead*>(this));
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the scene from file.
//
///////////////////////////////////////////////////////////////////////////////

FileIO::Unknown *FileIO::read ( const std::string &filename, Unknown * )
{
  typedef USUL_REF_POINTER ( osg::Node ) OsgNodePtr;

  // Read the model.
  OsgNodePtr osgNode ( osgDB::readNodeFile ( filename ) );
  if ( !osgNode.valid() )
    Usul::Exceptions::Thrower<SAL::Exceptions::FailedToRead> 
      ( "Error: 3086591695, failed to read file: ", filename );

  // Try to create the node.
  SAL::Interfaces::INode::QueryPtr node ( Factory::create ( osgNode ) );
  if ( node.valid() )
    return node.release();

  // If we get here then it didn't work.
  Usul::Exceptions::Thrower<SAL::Exceptions::UnknownNodeType> (
    "Error: 1557893944, failed to determine node type read from file.", 
    "\n\tFilename  = ", filename,
    "\n\tosg::Node = ", osgNode,
    "\n\tName      = ", osgNode->getName() ,
    "\n\tType      = ", osgNode->className() );

  // Compiler does not know that the above throws.
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the scene to file.
//
///////////////////////////////////////////////////////////////////////////////

void FileIO::write ( const std::string &filename, Unknown *data, Unknown * )
{
  // Get the node.
  SAL::Interfaces::IOSG::ValidQueryPtr osg ( data );
  ValidOsgRefPtr refPtr ( osg->osgReferenced() );
  ValidOsgNodePtr node ( dynamic_cast < osg::Node * > ( refPtr.get() ) );

  // Write it to file.
  if ( false == osgDB::writeNodeFile ( *node, filename ) )
  {
    // Throw the exception.
    Usul::Exceptions::Thrower<SAL::Exceptions::FailedToWrite> (
      "Error: 2224918676, failed to write node to file.", 
      "\n\tFilename  = ", filename,
      "\n\tUnknown   = ", data,
      "\n\tosg::Node = ", node.get(),
      "\n\tName      = ", node->getName() );
  }
}
