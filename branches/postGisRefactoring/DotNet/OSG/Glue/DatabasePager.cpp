
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "DatabasePager.h"

#include "osgDB/DatabasePager"

using namespace CadKit::OSG::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DatabasePager::DatabasePager() : _databasePager ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DatabasePager::DatabasePager( System::IntPtr ptr ) : _databasePager ( 0x0 )
{
  this->ptr( ptr );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DatabasePager::~DatabasePager()
{
  this->_unreference();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalizer.
//
///////////////////////////////////////////////////////////////////////////////

DatabasePager::!DatabasePager()
{
  this->_unreference();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference.
//
///////////////////////////////////////////////////////////////////////////////

void DatabasePager::_unreference()
{
  if( 0x0 != _databasePager )
  {
    _databasePager->unref();
    _databasePager = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the pointer.
//
///////////////////////////////////////////////////////////////////////////////

System::IntPtr DatabasePager::ptr()
{
  return System::IntPtr ( _databasePager );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the pointer.
//
///////////////////////////////////////////////////////////////////////////////

void DatabasePager::ptr ( System::IntPtr ptr )
{
  this->_unreference();

  _databasePager = reinterpret_cast< osgDB::DatabasePager* > ( ptr.ToPointer() );
  _databasePager->ref();
}

