
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __APP_PARSER__
#define __APP_PARSER__

#include <string.h>
#include <fstream>
#include <iostream>

#include "SimFlow/Parser/IParser.h"

#include "Export.h" 

namespace SimFlow
{
	namespace App
	{
		class APP_EXPORT Parser : public SimFlow::XMLParser::IParser
		{
		public:

      Parser(){ ;}

      virtual ~Parser(){ XMLPlatformUtils::Terminate(); }

			virtual void	  init();

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

