
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "MatrixManipulator.h"

#include "osgGA/MatrixManipulator"

using namespace CadKit::OSG::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MatrixManipulator::MatrixManipulator() : _matrixManipulator ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MatrixManipulator::MatrixManipulator( System::IntPtr ptr ) : _matrixManipulator ( 0x0 )
{
  this->ptr( ptr );
}


MatrixManipulator::~MatrixManipulator()
{
  this->_unreference();
}

MatrixManipulator::!MatrixManipulator()
{
  this->_unreference();
}

void MatrixManipulator::_unreference()
{
  if( 0x0 != _matrixManipulator )
  {
    _matrixManipulator->unref();
    _matrixManipulator = 0x0;
  }
}


System::IntPtr MatrixManipulator::ptr()
{
  return System::IntPtr ( _matrixManipulator );
}

void MatrixManipulator::ptr ( System::IntPtr ptr )
{
  this->_unreference();

  _matrixManipulator = reinterpret_cast< osgGA::MatrixManipulator* > ( ptr.ToPointer() );
  _matrixManipulator->ref();
}

