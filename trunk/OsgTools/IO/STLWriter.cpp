
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/IO/STLWriter.h"

#include "OsgTools/IO/STLWrite.h"
#include "OsgTools/IO/STLPrintVisitor.h"
#include "OsgTools/Statistics.h"
#include "OsgTools/SetDataVariance.h"
#include "OsgTools/Visitor.h"

#include "Usul/File/Temp.h"

#include "osg/Node"

#include "osgUtil/Optimizer"

using namespace OsgTools::IO;


STLWriter::STLWriter( const std::string& filename ) :
_filename ( filename ),
_header()
{
}


void STLWriter::header( const std::string& header )
{
  _header = header;
}

void STLWriter::writeASCII( const osg::Node &node  )
{
  // Make a temporary file
  Usul::File::Temp file ( Usul::File::Temp::ASCII );

  file.stream() << "solid " << _header << std::endl;

  osg::ref_ptr< osg::Node > copy ( dynamic_cast < osg::Node* > ( this->_copyScene( node ) ) );

  typedef OsgTools::IO::AsciiWriter< osg::Vec3 > Writer;
  typedef OsgTools::IO::STLPrintVisitor< OsgTools::IO::WriteSTLFile< Writer > > PrintVisitor;
  osg::ref_ptr<osg::NodeVisitor> printVisitor ( new PrintVisitor ( file.stream() ) );
  copy->accept ( *printVisitor );

  file.stream() << "endsolid" << std::endl;

  //Rename temporary file to final filename
  file.rename( _filename );
}

void STLWriter::writeBinary( const osg::Node &node )
{
  // Make a temporary file
  Usul::File::Temp file ( Usul::File::Temp::BINARY );

  // Count number of facets.
  OsgTools::Statistics sceneStats;
  sceneStats.greedy ( true );
  sceneStats.accumulate ( &node );
  unsigned int numFacets ( sceneStats.count ( OsgTools::Statistics::TRIANGLES ) );

  // Create a header.
  std::string header ( _header );
  header.resize ( 80, ' ' );
  file.stream().write ( header.c_str(), header.length() );
  Usul::IO::Binary::WriteLittleEndian::write ( file.stream(), numFacets );

  osg::ref_ptr< osg::Node > copy ( dynamic_cast < osg::Node* > ( this->_copyScene( node ) ) );

  // Visitor to write out file
  typedef OsgTools::IO::BinaryWriter< osg::Vec3 > Writer;
  typedef OsgTools::IO::STLPrintVisitor< OsgTools::IO::WriteSTLFile< Writer > > PrintVisitor;
  osg::ref_ptr<osg::NodeVisitor> printVisitor ( new PrintVisitor ( file.stream() ) );
  copy->accept ( *printVisitor );

  //Rename temporary file to final filename
  file.rename( _filename );
}


osg::Node*  STLWriter::_copyScene ( const osg::Node& node )
{
  //Make a deep copy of the scene
  osg::ref_ptr <osg::Group> copy ( dynamic_cast < osg::Group* > ( node.clone( osg::CopyOp::DEEP_COPY_ALL ) ) );

  // Traverse the scene and change all transforms to static data-variance.
  typedef OsgTools::SetDataVariance SetDataVariance;
  typedef OsgTools::Visitor < osg::Transform, SetDataVariance > VarianceVisitor;
  SetDataVariance setter ( osg::Object::STATIC );
  VarianceVisitor::Ptr vv ( new VarianceVisitor ( setter ) );
  copy->accept ( *vv );

  // Flatten static transforms
  osgUtil::Optimizer optimizer;
  optimizer.optimize ( copy.get(), osgUtil::Optimizer::FLATTEN_STATIC_TRANSFORMS );

  return copy.release();
}
