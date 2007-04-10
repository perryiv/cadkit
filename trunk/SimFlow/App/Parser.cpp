
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#include "SimFlow/App/Parser.h"
#include "SimFlow/App/AppSAX2Handler.h"

void SimFlow::App::Parser::init()
{
	// Initialize the XML4C2 system
  try
  {   
    XMLPlatformUtils::Initialize();	   
    mParser = XMLReaderFactory::createXMLReader();
  }
  catch ( const XMLException& ex )
  {
	std::cerr << "[ERROR:] Exception raised: " << ex.getMessage() << std::endl;     
  }
}

void SimFlow::App::Parser::setHandler( DefaultHandler* handler )
{
	mHandler = handler;

	if( mHandler )
	{
		setContentHandler( mHandler );
		setErrorHandler  ( mHandler );
	}
}

void SimFlow::App::Parser::setContentHandler( DefaultHandler* handler )
{
    mParser->setContentHandler( handler );
}

void SimFlow::App::Parser::setErrorHandler( DefaultHandler* handler )
{
    mParser->setErrorHandler( handler );
}

bool SimFlow::App::Parser::readXML( const char* fileName )
{
	bool success = false;	
 
  // Print out the stats that we collected and time taken
	try
	{	
    mParser->parse( fileName );
		AppSAX2Handler* handler = dynamic_cast< AppSAX2Handler* >( mHandler );

		if (!handler->getSawErrors())
		{
			XERCES_STD_QUALIFIER cout 
				<< fileName << ": " 
				<< handler->getElementCount()		  << " elems, "
				<< handler->getAttrCount()			  << " attrs, "
				<< handler->getSpaceCount()			  << " spaces, "
				<< handler->getCharacterCount()		<< " chars)" << XERCES_STD_QUALIFIER endl;
			success = true;
		}
		else
		{
			success = false;
		}
	}
	catch( const XMLException& ex )
	{
		success = false;
    std::cerr << "[ERROR:] Exception raised: " << ex.getMessage() << std::endl;		
	}

	return success;
}

bool SimFlow::App::Parser::readXML( std::string fileName )
{
	return readXML( fileName.c_str() );
}

