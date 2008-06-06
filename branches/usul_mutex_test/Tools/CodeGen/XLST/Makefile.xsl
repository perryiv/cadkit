<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- CompileGuard StyleSheet -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text"/>
		<xsl:strip-space elements="*"/>
	<xsl:template match="/">
        <xsl:text>SHELL = /bin/sh

# Must be defined before Makedefs is included.
PKG_NAME = </xsl:text> <xsl:value-of select="PluginTemplate/@name" /><xsl:text>
COMPILE_GUARD = </xsl:text> <xsl:value-of select="PluginTemplate/@compileGuard" /><xsl:text>

# Include all variables, flags, etc...
include ../Makedefs

INCLUDES = \
	-I./ \
	-I../ \
	-I$(USUL_INC_DIR) 

LIBS = -lUsul

LIB_DIRS = -L$(USUL_LIB_DIR)

# Include the rules for make...
include ../Makerules
</xsl:text>
</xsl:template>
</xsl:stylesheet>
