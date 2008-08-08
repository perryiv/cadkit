
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Component class to smooth triangles
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VTL_ALGORITHMS_COMPONENT_H__
#define __VTL_ALGORITHMS_COMPONENT_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDecimateTriangles.h"
#include "Usul/Interfaces/ISmoothTriangles.h"
#include "Usul/Interfaces/ISubdivideTriangles.h"

class VtkAlgorithmsComponent : public Usul::Base::Referenced,
															 public Usul::Interfaces::IDecimateTriangles,
                               public Usul::Interfaces::ISmoothTriangles,
															 public Usul::Interfaces::ISubdivideTriangles,
                               public Usul::Interfaces::IPlugin
{
public:
  
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( VtkAlgorithmsComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Constructor
  VtkAlgorithmsComponent();

protected:    
  ///Destructor
  virtual ~VtkAlgorithmsComponent();

  ///Usul/Interfaces/IPlugin.h implementation
  virtual std::string getPluginName() const;
    
  /// Usul::Interfaces::ISmoothTriangles
  virtual void smoothTriangles ( osg::Array *vertices, osg::DrawElementsUInt *indices,
                                 osg::Array *normalsT, osg::Array *normalsV, unsigned int numIterations );

  virtual void smoothTriangles ( OsgTools::Triangles::TriangleSet*, unsigned int numIteration );

	/// Usul::Interfaces::ISubdivideTriangles
	virtual void subdivideTriangles ( osg::Array *vertices, osg::DrawElementsUInt *indices,
                                    osg::Array *normalsT, osg::Array *normalsV, unsigned int numSubdivisions );

  virtual void subdivideTriangles ( OsgTools::Triangles::TriangleSet *triangleSet, unsigned int numSubdivisions );

	/// Usul::Interfaces::IDecimateTriangles.
	virtual void decimateTriangles ( osg::Array *inVerts, osg::DrawElementsUInt *inIndices,
                                   osg::Array *outNormalsT, osg::Array *outNormalsV, float reduction );

  virtual void decimateTriangles ( OsgTools::Triangles::TriangleSet*, float reduction );
};

#endif /* __VTL_ALGORITHMS_COMPONENT_H__  */		
