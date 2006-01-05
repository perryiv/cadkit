<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- Export StyleSheet -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text"/>
		<xsl:strip-space elements="*"/>
	<xsl:template match="/">
        <xsl:text>LIBRARY	</xsl:text> <xsl:value-of select="PluginTemplate/@name" /><xsl:text>
EXPORTS
                usul_is_debug_build      @1 PRIVATE
                usul_get_class_factory   @2 PRIVATE

</xsl:text>
</xsl:template>
</xsl:stylesheet>
