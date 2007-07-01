
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/DocumentProxy.h"

#include "Usul/Errors/Assert.h"

using namespace CadKit::Helios::Core;

///////////////////////////////////////////////////////////////////////////////
//
//  Default Constructor.
//  This is for Q_DECLARE_METATYPE.  Do not use.
//
///////////////////////////////////////////////////////////////////////////////

DocumentProxy::DocumentProxy () : 
  _document ( 0x0 )
{
  USUL_ASSERT ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DocumentProxy::DocumentProxy ( Document * doc ) : 
  _document ( doc )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DocumentProxy::DocumentProxy ( const DocumentProxy& rhs ) :
  _document ( rhs._document )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

DocumentProxy& DocumentProxy::operator = ( const DocumentProxy &rhs )
{
  this->_document = rhs._document;
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DocumentProxy::~DocumentProxy()
{
  _document = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Documents::Document* DocumentProxy::document()
{
  return _document;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

void DocumentProxy::document ( Document* document )
{
  _document = document;
}
