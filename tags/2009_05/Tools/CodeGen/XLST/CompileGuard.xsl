<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- CompileGuard StyleSheet -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text"/>
		<xsl:strip-space elements="*"/>
	<xsl:template match="/">
        <xsl:text>
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, </xsl:text> <xsl:value-of select="PluginTemplate/User/@name" /><xsl:text>
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  This header file should not be included in client code.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef </xsl:text> <xsl:value-of select="PluginTemplate/CompileGuard/@headerDefine" /><xsl:text>
#define </xsl:text> <xsl:value-of select="PluginTemplate/CompileGuard/@headerDefine" /><xsl:text>

#ifndef </xsl:text> <xsl:value-of select="PluginTemplate/CompileGuard/@compilePreprocess" /><xsl:text>
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#endif /* </xsl:text> <xsl:value-of select="PluginTemplate/CompileGuard/@headerDefine" /><xsl:text> */</xsl:text>   		
		<xsl:text>&#xa;</xsl:text>     
	</xsl:template>
</xsl:stylesheet>

