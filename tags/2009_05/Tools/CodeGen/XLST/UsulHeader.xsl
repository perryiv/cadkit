<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- Usul Header StyleSheet -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text"/>
		<xsl:strip-space elements="*"/>
	<xsl:template match="/">
        <xsl:text>
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, </xsl:text><xsl:value-of select="PluginTemplate/User/@name" /><xsl:text>
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  </xsl:text><xsl:value-of select="PluginTemplate/UsulHeader/@classDescription" /><xsl:text>
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_</xsl:text> <xsl:value-of select="PluginTemplate/UsulHeader/@compileGuard" /><xsl:text>
#define __USUL_INTERFACES_</xsl:text> <xsl:value-of select="PluginTemplate/UsulHeader/@compileGuard" /><xsl:text>

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct </xsl:text><xsl:value-of select="PluginTemplate/UsulHeader/@className" /><xsl:text> : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( </xsl:text><xsl:value-of select="PluginTemplate/UsulHeader/@className" /><xsl:text> );

  /// Id for this interface.
  enum { IID = </xsl:text><xsl:value-of select="PluginTemplate/UsulHeader/@IID" /><xsl:text>u };

}; // struct </xsl:text> <xsl:value-of select="PluginTemplate/UsulHeader/@className" /><xsl:text>


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_</xsl:text> <xsl:value-of select="PluginTemplate/UsulHeader/@compileGuard" /><xsl:text> */</xsl:text>   		
		<xsl:text>&#xa;</xsl:text>     
	</xsl:template>
</xsl:stylesheet>

