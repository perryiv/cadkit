
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __APP_PARSER__
#define __APP_PARSER__

#include <string.h>
#include <fstream>
#include <iostream>

#include "SimFlow/Core/Export.h"

#include "SimFlow/Core/Parser/IParser.h"

namespace SimFlow
{
	namespace App
	{
		class SIM_FLOW_EXPORT Parser : public SimFlow::XMLParser::IParser
		{
		public:
			virtual void	init();

			void			      setHandler( DefaultHandler* handler );

			void			      setContentHandler( DefaultHandler* handler );

			void			      setErrorHandler( DefaultHandler* handler );

			virtual bool	  readXML( const char* fileName );

			virtual bool	  readXML( std::string fileName );			

		protected:

			SAX2XMLReader*	mParser;
		
			DefaultHandler*	mHandler;
		};
	}
}


#endif // __APP_PARSER__

