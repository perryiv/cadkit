
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/DocumentProxy.h"

#include "Usul/Pointers/Functions.h"
#include "Usul/Errors/Assert.h"

using namespace CadKit::Helios::Core;

///////////////////////////////////////////////////////////////////////////////
//
//  Default Constructor.
//  This is for Q_DECLARE_METATYPE.  Do not use.
//
///////////////////////////////////////////////////////////////////////////////

DocumentProxy::DocumentProxy() : 
  _document ( 0x0 )
{
  USUL_ASSERT ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DocumentProxy::DocumentProxy ( Document::RefPtr doc ) : 
  _document ( doc.get() )
{
  Usul::Pointers::reference ( _document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DocumentProxy::DocumentProxy ( const DocumentProxy& rhs ) :
  _document ( rhs._document )
{
  Usul::Pointers::reference ( _document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

DocumentProxy& DocumentProxy::operator = ( const DocumentProxy &rhs )
{
  this->setDocument ( rhs.getDocument() );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DocumentProxy::~DocumentProxy()
{
  Usul::Pointers::unreference ( _document );
  _document = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Documents::Document::RefPtr DocumentProxy::getDocument() const
{
  return Usul::Documents::Document::RefPtr ( _document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

void DocumentProxy::setDocument ( Document::RefPtr document )
{
  Usul::Pointers::unreference ( _document );
  _document = document.get();
  Usul::Pointers::reference ( _document );
}
