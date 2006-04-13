
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

#ifndef __DECIMATETRIANGLESCOMPONENT_H__
#define __DECIMATETRIANGLESCOMPONENT_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDecimateTriangles.h"


class DecimateTrianglesComponent : public Usul::Base::Referenced,
                                   public Usul::Interfaces::IDecimateTriangles,
                                   public Usul::Interfaces::IPlugin
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DecimateTrianglesComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  DecimateTrianglesComponent();

protected:    
  ///Destructor
  ~DecimateTrianglesComponent();

  ///Usul/Interfaces/IPlugin.h implementation
  virtual std::string getPluginName() const;
    
  /// Usul::Interfaces::IDecimateTriangles

  virtual void decimateTriangles ( osg::Array *inVerts, osg::DrawElementsUInt *inIndices,
                                   osg::Array *outNormalsT, osg::Array *outNormalsV, float reduction );

  virtual void decimateTriangles ( OsgTools::Triangles::TriangleSet* );
};

#endif /* __DECIMATETRIANGLESCOMPONENT_H__  */		
	
