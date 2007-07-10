
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#include "SimFlow/Core/Parser/IParser.h"

using namespace SimFlow::XMLParser;

//using namespace ostk::ostkXML;

void IParser::init()
{
	// Initialize the XML4C2 system
    try
    {     
	   mParser = XMLReaderFactory::createXMLReader();
	}
    catch (const XMLException& toCatch)
    {
		std::cerr << "[readXML] Exception raised: " << toCatch.getMessage() << std::endl;     
    }
}

void IParser::setHandler( DefaultHandler* handler )
{
	mHandler = handler;

	setContentHandler( mHandler );
	setErrorHandler  ( mHandler );
}

void IParser::setContentHandler( DefaultHandler* handler )
{
    mParser->setContentHandler( handler );
}

void IParser::setErrorHandler( DefaultHandler* handler )
{
    mParser->setErrorHandler( handler );
}
