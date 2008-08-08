
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Component class for VTK algorithms.
//
///////////////////////////////////////////////////////////////////////////////

#include "VtkAlgorithmsComponent.h"
#include "DecimateTriangles.h"
#include "SmoothTriangles.h"
#include "SubdivideTriangles.h"

#include "Usul/Components/Factory.h"


USUL_DECLARE_COMPONENT_FACTORY ( VtkAlgorithmsComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( VtkAlgorithmsComponent, VtkAlgorithmsComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

VtkAlgorithmsComponent::VtkAlgorithmsComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

VtkAlgorithmsComponent::~VtkAlgorithmsComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *VtkAlgorithmsComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
	case Usul::Interfaces::IDecimateTriangles::IID:
    return static_cast < Usul::Interfaces::IDecimateTriangles* > ( this );
  case Usul::Interfaces::ISmoothTriangles::IID:
    return static_cast < Usul::Interfaces::ISmoothTriangles* > ( this );
	case Usul::Interfaces::ISubdivideTriangles::IID:
    return static_cast < Usul::Interfaces::ISubdivideTriangles* > ( this );
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Usul/Interfaces/IPlugin.h implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string VtkAlgorithmsComponent::getPluginName() const 
{
  return "Vtk Algorithms";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Smooth the triangle set.
//
///////////////////////////////////////////////////////////////////////////////

void VtkAlgorithmsComponent::smoothTriangles ( OsgTools::Triangles::TriangleSet *triangleSet, unsigned int numIterations )
{
	SmoothTriangles st;
	st ( triangleSet, numIterations );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Smooth the vertices.
//
///////////////////////////////////////////////////////////////////////////////

void VtkAlgorithmsComponent::smoothTriangles ( osg::Array *v, osg::DrawElementsUInt *indices,
                                               osg::Array *nT, osg::Array *nV, unsigned int numIterations )
{
	SmoothTriangles st;
	st ( v, indices, nT, nV, numIterations );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subdivide the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void VtkAlgorithmsComponent::subdivideTriangles ( osg::Array *vertices, osg::DrawElementsUInt *indices,
                                                  osg::Array *normalsT, osg::Array *normalsV, unsigned int numSubdivisions )
{
	SubdivideTriangles st;
	st ( vertices, indices, normalsT, normalsV, numSubdivisions );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subdivide the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void VtkAlgorithmsComponent::subdivideTriangles ( OsgTools::Triangles::TriangleSet *triangleSet, unsigned int numSubdivisions )
{
	SubdivideTriangles st;
	st ( triangleSet, numSubdivisions );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decimate the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void VtkAlgorithmsComponent::decimateTriangles ( osg::Array *inVerts, osg::DrawElementsUInt *inIndices,
                                                 osg::Array *outNormalsT, osg::Array *outNormalsV, float reduction )
{
	DecimateTriangles dt;
	dt ( inVerts, inIndices, outNormalsT, outNormalsV, reduction );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decimate the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void VtkAlgorithmsComponent::decimateTriangles ( OsgTools::Triangles::TriangleSet* triangleSet, float reduction )
{
	DecimateTriangles dt;
	dt ( triangleSet, reduction );
}
