#ifndef __XMLPARSER_STRX_H__
#define __XMLPARSER_STRX_H__

#include "Export.h"

XERCES_CPP_NAMESPACE_USE

namespace SimFlow
{
	namespace XMLParser
	{
		class PARSER_EXPORT StrX
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

#endif // __XMLPARSER_STRX_H__

