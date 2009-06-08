
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////
#include "ISAX2Handler.h"

#include <iostream>

#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>

using namespace SimFlow::XMLParser;

ISAX2Handler::ISAX2Handler() : DefaultHandler(),
	fAttrCount(0), 
    fCharacterCount(0), 
    fElementCount(0), 
    fSpaceCount(0), 
    fSawErrors(false)
{	
}

ISAX2Handler::~ISAX2Handler()
{
}


void ISAX2Handler::ignorableWhitespace( const   XMLCh* const /* chars */, 
									    const unsigned int length )
{
	fSpaceCount += length;
}

void ISAX2Handler::resetDocument()
{
    fAttrCount		= 0;
    fCharacterCount = 0;
    fElementCount	= 0;
    fSpaceCount		= 0;
}

void ISAX2Handler::error( const SAXParseException& e )
{
    fSawErrors = true;   
}

void ISAX2Handler::fatalError( const SAXParseException& e )
{
    fSawErrors = true;    
}

void ISAX2Handler::warning( const SAXParseException& e )
{    
}

void ISAX2Handler::resetErrors()
{
    fSawErrors = false;
}


