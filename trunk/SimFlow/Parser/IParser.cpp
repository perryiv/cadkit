#include "Parser/IParser.h"

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
