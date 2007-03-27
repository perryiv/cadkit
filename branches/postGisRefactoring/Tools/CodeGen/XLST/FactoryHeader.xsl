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
//  </xsl:text> <xsl:value-of select="PluginTemplate/FactoryHeader/@classDescription" /><xsl:text>
//
///////////////////////////////////////////////////////////////////////////////

#ifndef </xsl:text> <xsl:value-of select="PluginTemplate/FactoryHeader/@compileGuard" /><xsl:text>
#define </xsl:text> <xsl:value-of select="PluginTemplate/FactoryHeader/@compileGuard" /><xsl:text>

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IClassFactory.h"


class </xsl:text> <xsl:value-of select="PluginTemplate/FactoryHeader/@className" /><xsl:text> : public Usul::Base::Referenced,
                                public Usul::Interfaces::IClassFactory
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( </xsl:text> <xsl:value-of select="PluginTemplate/FactoryHeader/@className" /><xsl:text> );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  </xsl:text> <xsl:value-of select="PluginTemplate/FactoryHeader/@className" /><xsl:text>();

protected:

  /// Use reference counting.
  virtual ~</xsl:text> <xsl:value-of select="PluginTemplate/FactoryHeader/@className" /><xsl:text>();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IClassFactory
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Create a component of the given interface type (defined by iid).
  /// Return an unknown pointer to this new component.
  virtual Unknown *       createInstance ( unsigned long iid );
};


#endif /* </xsl:text> <xsl:value-of select="PluginTemplate/FactoryHeader/@compileGuard" /><xsl:text>  */		
	</xsl:text>
	<xsl:text>&#xa;</xsl:text>     
	</xsl:template>
</xsl:stylesheet>
