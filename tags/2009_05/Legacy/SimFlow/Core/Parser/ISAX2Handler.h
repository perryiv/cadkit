
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// Handle for SAX2. 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSTK_OSTK_XML_ISAX2_HANDLER_H__
#define __OSTK_OSTK_XML_ISAX2_HANDLER_H__

#if _WIN32
	#include <windows.h>
#endif 

#include <ostream>
#include <vector>
#include <string>

#include "xercesc/sax2/Attributes.hpp"
#include "xercesc/sax2/DefaultHandler.hpp"
#include "xercesc/sax2/ContentHandler.hpp"
#include "xercesc/sax2/DeclHandler.hpp"
#include "xercesc/sax2/LexicalHandler.hpp"

#include "SimFlow/Core/Export.h"

using namespace std;
XERCES_CPP_NAMESPACE_USE

namespace SimFlow
{
	namespace XMLParser
	{
		class SIM_FLOW_EXPORT ISAX2Handler : public DefaultHandler
		{
			public:				
				///////////////////////////////////////////////////////////////////////////
				//
				// Constructor. 
				//
				///////////////////////////////////////////////////////////////////////////
				ISAX2Handler();

				///////////////////////////////////////////////////////////////////////////
				//
				// Destructor.
				//
				///////////////////////////////////////////////////////////////////////////
				virtual ~ISAX2Handler();
				///////////////////////////////////////////////////////////////////////////
				//
				// Return number of elements in a description file.
				//
				///////////////////////////////////////////////////////////////////////////
				unsigned int getElementCount() const
				{
					return fElementCount;
				}

				///////////////////////////////////////////////////////////////////////////
				//
				// Return number of attributes (properties) in a description file.
				//
				///////////////////////////////////////////////////////////////////////////
				unsigned int getAttrCount()	const
				{
					return fAttrCount;
				}

				///////////////////////////////////////////////////////////////////////////
				//
				// Return number of characters in a description file.
				//
				///////////////////////////////////////////////////////////////////////////
				unsigned int getCharacterCount() const
				{
					return fCharacterCount;
				}

				///////////////////////////////////////////////////////////////////////////
				//
				// Return true/false for whether or not any error occured reading the 
				// description file. 
				//
				///////////////////////////////////////////////////////////////////////////
				bool getSawErrors()	const
				{
					return fSawErrors;
				}

				///////////////////////////////////////////////////////////////////////////
				//
				// Return number of space char in a description file.
				//
				///////////////////////////////////////////////////////////////////////////
				unsigned int getSpaceCount() const
				{
					return fSpaceCount;
				}

				///////////////////////////////////////////////////////////////////////////
				//
				//  Handlers for the SAX ContentHandler interface
				//
				///////////////////////////////////////////////////////////////////////////
				virtual void startElement(  const XMLCh* const uri, 
								                    const XMLCh* const localname, 
								                    const XMLCh* const qname, 
								                    const Attributes& attrs )
				{
				}

				virtual void endElement(  const XMLCh* const uri, 
								                  const XMLCh* const localname, 
								                  const XMLCh* const qname )
				{
				}

				virtual void characters(   const XMLCh* const chars, 
								   const unsigned int length )
				{
				}

				void ignorableWhitespace( const XMLCh* const chars, const unsigned int length );

				virtual void resetDocument();


				///////////////////////////////////////////////////////////////////////////
				//
				//  Handlers for the SAX ErrorHandler interface
				//
				///////////////////////////////////////////////////////////////////////////

				void warning( const SAXParseException& exc );

				void error( const SAXParseException& exc );

				void fatalError( const SAXParseException& exc );

				void resetErrors();

			protected:
				///////////////////////////////////////////////////////////////////////////
				//
				//  Private data members
				//
				//  fAttrCount
				//  fCharacterCount
				//  fElementCount
				//  fSpaceCount
				//      These are just counters that are run upwards based on the input
				//      from the document handlers.
				//
				//  fSawErrors
				//      This is set by the error handlers, and is queryable later to
				//      see if any errors occured.
				//
				///////////////////////////////////////////////////////////////////////////
				unsigned int				fAttrCount;
				unsigned int				fCharacterCount;
				unsigned int				fElementCount;
				unsigned int				fSpaceCount;
				bool						    fSawErrors;		
		};
	}
}

#endif // __OSTK_OSTK_XML_SAX2_HANDLER_H__
