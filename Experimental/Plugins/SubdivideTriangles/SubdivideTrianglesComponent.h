
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

#ifndef __SUBDIVIDETRIANGLESCOMPONENT_H__
#define __SUBDIVIDETRIANGLESCOMPONENT_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ISubdivideTriangles.h"

class SubdivideTrianglesComponent : public Usul::Base::Referenced,
                                    public Usul::Interfaces::IPlugin,
                                    public Usul::Interfaces::ISubdivideTriangles
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SubdivideTrianglesComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  SubdivideTrianglesComponent();

protected:    
  ///Destructor
  ~SubdivideTrianglesComponent();

  ///Usul/Interfaces/IPlugin.h implementation
  virtual std::string getPluginName() const;
    
  /// Usul::Interfaces::ISubdivideTriangles

  virtual void subdivideTriangles ( osg::Array *vertices, osg::DrawElementsUInt *indices,
                                    osg::Array *normalsT, osg::Array *normalsV, unsigned int numIterations );

  virtual void subdivideTriangles ( OsgTools::Triangles::TriangleSet *triangleSet, unsigned int numSubdivisions );
};

#endif /* __SUBDIVIDETRIANGLESCOMPONENT_H__  */		
	
