
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach & Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all image implementations.
//
///////////////////////////////////////////////////////////////////////////////

#include "Images/Core/BaseImage.h"

using namespace Images;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseImage::BaseImage() : BaseClass(),
  _rows    ( 0 ),
  _columns ( 0 ),
  _layers  ( 1 ),
  _alpha   ( false )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseImage::BaseImage ( const BaseImage &b ) : BaseClass ( b ),
  _rows    ( b._rows    ),
  _columns ( b._columns ),
  _layers  ( b._layers  ),
  _alpha   ( b._alpha   )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseImage::~BaseImage()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size info.
//
///////////////////////////////////////////////////////////////////////////////

void BaseImage::_set ( unsigned int rows, unsigned int columns, unsigned int layers )
{
  _rows    = rows;
  _columns = columns;
  _layers  = layers;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag that says if last channel should be interpreted as alpha.
//
///////////////////////////////////////////////////////////////////////////////

void BaseImage::alpha ( bool a )
{
  _alpha = a;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of values.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int BaseImage::size() const
{
  return ( this->rows() * this->columns() * this->layers() * this->channels() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of rows.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int BaseImage::rows() const
{
  return _rows;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of columns.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int BaseImage::columns() const
{
  return _columns;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of layers.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int BaseImage::layers() const
{
  return _layers;
}