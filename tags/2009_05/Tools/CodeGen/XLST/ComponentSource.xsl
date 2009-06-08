<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- **Factory.h StyleSheet -->
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
//  </xsl:text> <xsl:value-of select="PluginTemplate/ComponentHeader/@classDescription" /><xsl:text>
//
///////////////////////////////////////////////////////////////////////////////
&#xa;
#include "</xsl:text> <xsl:value-of select="PluginTemplate/ComponentHeader/@className" /><xsl:text>.h"&#xa;</xsl:text>
<xsl:text>
&#xa;
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( </xsl:text> <xsl:value-of select="PluginTemplate/ComponentHeader/@className" /><xsl:text> , </xsl:text> <xsl:value-of select="PluginTemplate/ComponentHeader/@className" /><xsl:text>::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

</xsl:text> <xsl:value-of select="PluginTemplate/ComponentHeader/@className" /><xsl:text>::</xsl:text> <xsl:value-of select="PluginTemplate/ComponentHeader/@className" /><xsl:text>() 
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

</xsl:text> <xsl:value-of select="PluginTemplate/ComponentHeader/@className" /><xsl:text>::~</xsl:text> <xsl:value-of select="PluginTemplate/ComponentHeader/@className" /><xsl:text>() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *</xsl:text><xsl:value-of select="PluginTemplate/ComponentHeader/@className" /><xsl:text>::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:&#x0A;</xsl:text>
  <xsl:for-each select="PluginTemplate/ComponentHeader/UsulInterface">
  <xsl:text>case Usul::Interfaces::</xsl:text> <xsl:value-of select="@name" /><xsl:text>::IID:
    return static_cast &lt; Usul::Interfaces::</xsl:text> <xsl:value-of select="@name" /><xsl:text>*&gt;(this);&#x0A;</xsl:text>
  </xsl:for-each>
  <xsl:text>case Usul::Interfaces::IPlugin::IID:
    return static_cast &lt; Usul::Interfaces::IPlugin* &gt; ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Usul/Interfaces/IPlugin.h implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string </xsl:text><xsl:value-of select="PluginTemplate/ComponentHeader/@className" /><xsl:text>::getPluginName() const 
{
  return "</xsl:text><xsl:value-of select="PluginTemplate/ComponentHeader/@className" /><xsl:text>";
}

	</xsl:text>
	<xsl:text>&#xa;</xsl:text>     
	</xsl:template>
</xsl:stylesheet>
