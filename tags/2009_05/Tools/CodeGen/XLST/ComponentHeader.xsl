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

#ifndef </xsl:text> <xsl:value-of select="PluginTemplate/ComponentHeader/@compileGuard" /><xsl:text>
#define </xsl:text> <xsl:value-of select="PluginTemplate/ComponentHeader/@compileGuard" />
<xsl:text>&#xa;</xsl:text><xsl:text>
#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"</xsl:text>

<xsl:for-each select="PluginTemplate/ComponentHeader/LibInclude">
    <xsl:text>#include "</xsl:text><xsl:value-of select="@file" /><xsl:text>"&#xa;</xsl:text>
</xsl:for-each><xsl:text>&#xa;</xsl:text>
<xsl:for-each select="PluginTemplate/ComponentHeader/SystemInclude">
    <xsl:text>#include &lt;</xsl:text><xsl:value-of select="@file" /><xsl:text>&gt;&#xa;</xsl:text>
</xsl:for-each>
<xsl:text>
&#xa;
</xsl:text>
<xsl:for-each select="PluginTemplate/ComponentHeader/UsulInterface">
<xsl:text>#include "Usul/Interfaces/</xsl:text><xsl:value-of select="@name" /><xsl:text>.h"&#xa;</xsl:text>
</xsl:for-each><xsl:text>
&#xa;
class </xsl:text> <xsl:value-of select="PluginTemplate/ComponentHeader/@className" /><xsl:text> : public Usul::Base::Referenced,&#x0A;</xsl:text>
<xsl:for-each select="PluginTemplate/ComponentHeader/UsulInterface">
                    <xsl:text>public Usul::Interfaces::</xsl:text> <xsl:value-of select="@name" /><xsl:text>,&#x0A;</xsl:text>
</xsl:for-each>
                    <xsl:text>public Usul::Interfaces::IPlugin
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( </xsl:text> <xsl:value-of select="PluginTemplate/ComponentHeader/@className" /><xsl:text> );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  </xsl:text> <xsl:value-of select="PluginTemplate/ComponentHeader/@className" /><xsl:text>();

protected:    
  /// Destructor
  ~</xsl:text> <xsl:value-of select="PluginTemplate/ComponentHeader/@className" /><xsl:text>();

  /// Usul::Interfaces::IPlugin implementation
  virtual std::string getPluginName() const;
    
  /** Insert your Plugin specific code here **/
};

#endif /* </xsl:text> <xsl:value-of select="PluginTemplate/ComponentHeader/@compileGuard" /><xsl:text>  */		
	</xsl:text>
	<xsl:text>&#xa;</xsl:text>     
	</xsl:template>
</xsl:stylesheet>
