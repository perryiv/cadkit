
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CALCPERVERTEXNORMALSCOMPONENT_H__
#define __CALCPERVERTEXNORMALSCOMPONENT_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"

#include "Usul/Interfaces/ICalculatePerVertexNormals.h"

class CalcPerVertexNormalsComponent : public Usul::Base::Referenced,
                                      public Usul::Interfaces::IPlugin,
                                      public Usul::Interfaces::ICalculatePerVertexNormals
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( CalcPerVertexNormalsComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  CalcPerVertexNormalsComponent();

protected:    
  /// Destructor
  ~CalcPerVertexNormalsComponent();

  /// Usul::Interfaces::IPlugin implementation
  virtual std::string getPluginName() const;
    
  /** Insert your Plugin specific code here **/

  virtual void    calculatePerVertexNormals ( osg::Array *vertices, osg::DrawElementsUInt *elements, osg::Array *normalsT, osg::Array *normalsV );
  virtual void    calculatePerVertexNormals ( OsgTools::Triangles::TriangleSet *triangleSet );
};

#endif /* __CALCPERVERTEXNORMALSCOMPONENT_H__  */		
	
