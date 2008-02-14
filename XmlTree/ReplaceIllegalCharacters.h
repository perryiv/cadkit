
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __REPLACE_ILLEGAL_CHARACTERS_H__
#define __REPLACE_ILLEGAL_CHARACTERS_H__

#include "XmlTree/Export.h"

#include <string>

namespace XmlTree
{
  XML_TREE_EXPORT void replaceIllegalCharacters ( std::string& string );
  XML_TREE_EXPORT std::string translateToLegalCharacters ( const std::string& string );

  XML_TREE_EXPORT void restoreIllegalCharacters ( std::string& string );
}


#endif // __REPLACE_ILLEGAL_CHARACTERS_H__
