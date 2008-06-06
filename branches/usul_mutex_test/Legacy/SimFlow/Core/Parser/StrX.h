
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __OSTK_OSTK_XML_STRX_H__
#define __OSTK_OSTK_XML_STRX_H__

#include "SimFlow/Core/Export.h"

XERCES_CPP_NAMESPACE_USE

namespace SimFlow
{
	namespace XMLParser
	{
		class SIM_FLOW_EXPORT StrX
		{
			public :
				///////////////////////////////////////////////////////////////////////
				//
				//  Constructor
				//
				///////////////////////////////////////////////////////////////////////
				StrX(const XMLCh* const toTranscode)
				{
					// Call the private transcoding method
					fLocalForm = XMLString::transcode(toTranscode);
				}

				///////////////////////////////////////////////////////////////////////
				//
				// Destructor
				//
				///////////////////////////////////////////////////////////////////////
				~StrX()
				{
					XMLString::release(&fLocalForm);
				}

				const char* localForm() const
				{
					return fLocalForm;
				}

			private :
				///////////////////////////////////////////////////////////////////////
				//
				//  Private data members
				//
				//  fLocalForm
				//      This is the local code page form of the string.
				//
				///////////////////////////////////////////////////////////////////////
				char*   fLocalForm;
		};

		inline XERCES_STD_QUALIFIER ostream& operator<<(XERCES_STD_QUALIFIER ostream& target, const StrX& toDump)
		{
			target << toDump.localForm();
			return target;
		}
	}
}

#endif // __OSTK_OSTK_XML_STRX_H__

