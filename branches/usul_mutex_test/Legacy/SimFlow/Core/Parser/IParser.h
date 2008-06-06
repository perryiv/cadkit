
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSTK_OSTK_XML_IPARSER_H__
#define __OSTK_OSTK_XML_IPARSER_H__

#include <string.h>
#include <fstream>
#include <iostream>

#include "xercesc/parsers/SAXParser.hpp"
#include "xercesc/sax2/XMLReaderFactory.hpp"
#include "xercesc/sax2/SAX2XMLReader.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/OutOfMemoryException.hpp"
#include "xercesc/sax2/Attributes.hpp"
#include "xercesc/sax2/DefaultHandler.hpp"
#include "xercesc/sax2/Attributes.hpp"
#include "xercesc/sax/SAXParseException.hpp"
#include "xercesc/sax/SAXException.hpp"

#include "SimFlow/Core/Export.h"

XERCES_CPP_NAMESPACE_USE

namespace SimFlow
{ 
	namespace XMLParser
	{
		class SIM_FLOW_EXPORT IParser
		{
			public:

				virtual void init()											                    = 0;				

				virtual void setHandler( DefaultHandler* handler )			    = 0;

				virtual void setContentHandler( DefaultHandler* handler )   = 0;

				virtual void setErrorHandler( DefaultHandler* handler )		  = 0;

				virtual bool readXML( std::string fileName )			          = 0;

				virtual bool readXML( const char* fileName )				        = 0;
		};
	}
}


#endif // __OSTK_OSTK_XML_IPARSER_H__

