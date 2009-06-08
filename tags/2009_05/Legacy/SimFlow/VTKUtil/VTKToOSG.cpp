
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#include "VTKToOSG.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkPointData.h"
#include "vtkCellData.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Vec3"
#include "osg/LineWidth"

#include "osgUtil/SmoothingVisitor"

#include <iostream>

using namespace SimFlow::VTKUtil;

osg::Geode*  VTKToOSG::vtkActorToOSG( vtkActor *actor, int verbose ) 
{
	osg::ref_ptr< osg::Geode > geode = new osg::Geode();

	// Make this actor current. 
	actor->GetMapper()->Update();

	// Currently vtkActorToOSG assumes polyData. 
	// @Todo: Extend this to vtkDataSetMapper.
	if ( strcmp ( actor->GetMapper()->GetInput()->GetClassName(), "vtkPolyData" ) ) 
	{
		std::cerr << " ERROR! Actor must use a vtkPolyDataMapper " << std::endl;
		std::cerr << " If you are using a vtkDataSetMapper, use vtkGeometryFilter instead " << std::endl;
		return NULL;
	}

	// Get poly data. 
	vtkPolyData *polyData = ( vtkPolyData *) actor->GetMapper()->GetInput();

	// Get primitive arrays
	osg::ref_ptr< osg::Geometry > points, lines, polys, strips;

	// create new Geometry for the Geode
	if( polyData->GetVerts() )
	{
		points = processPrimitive( actor, polyData->GetVerts(),  osg::PrimitiveSet::POINTS,          verbose );
	}
	if( polyData->GetLines() )
	{
		lines  = processPrimitive( actor, polyData->GetLines(),  osg::PrimitiveSet::LINE_STRIP,      verbose );
	}
	if( polyData->GetPolys() )
	{
		polys  = processPrimitive( actor, polyData->GetPolys(),  osg::PrimitiveSet::POLYGON,         verbose );
	}
	if( polyData->GetStrips() )
	{
		strips = processPrimitive( actor, polyData->GetStrips(), osg::PrimitiveSet::TRIANGLE_STRIP,  verbose );
	}

	// remove old gsets and delete them	
	while( geode->getNumDrawables() ) geode->removeDrawable( ( unsigned int ) 0 );

	if( points.valid() ) geode->addDrawable( points.get() );
	if( lines.valid() )  geode->addDrawable( lines.get() );
	if( polys.valid() )  geode->addDrawable( polys.get() );
	if( strips.valid() ) geode->addDrawable( strips.get() );

	return geode.release();
}

osg::Geometry* VTKToOSG::processPrimitive( vtkActor *actor, vtkCellArray *primArray, int primType, int verbose ) 
{
	// Initialize the Geometry
	osg::ref_ptr< osg::Geometry > geom = new osg::Geometry;

	// Get poly data from the actor. 
	vtkPolyData *polyData = ( vtkPolyData *) actor->GetMapper()->GetInput();

	int numPrimitives = primArray->GetNumberOfCells();
	if (numPrimitives == 0) 
		return NULL;

	// Get number of indices in the vtk prim array. Each vtkCell has the length
	// (not counted), followed by the indices.
	int primArraySize = primArray->GetNumberOfConnectivityEntries();
	int numIndices    = primArraySize - numPrimitives;

	// allocate as many verts as there are indices in vtk prim array
	osg::ref_ptr< osg::Vec3Array > vertices = new osg::Vec3Array;

	// check to see if there are normals
	int normalPerVertex = 0;
	int normalPerCell = 0;

	#ifdef VTK4
		vtkDataArray* normals = polyData->GetPointData()->GetNormals();
	#else
		vtkNormals* normals = polyData->GetPointData()->GetNormals();
	#endif
	
	if ( actor->GetProperty()->GetInterpolation() == VTK_FLAT )
	{
		normals = NULL;
	}

	if (normals != NULL)
	{
		normalPerVertex = 1;
	}
	else
	{
		normals = polyData->GetCellData()->GetNormals();
		if ( normals != NULL )
		{
			normalPerCell = 1;
		}
	}

	osg::ref_ptr< osg::Vec3Array > norms = new osg::Vec3Array;

	// check to see if there is color information
	int colorPerVertex = 0;
	int colorPerCell = 0;

	#ifdef VTK4
	   vtkReal opacity = actor->GetProperty()->GetOpacity();
	   vtkUnsignedCharArray *colorArray = actor->GetMapper()->MapScalars(opacity);
	#else
	   vtkScalars *colorArray = actor->GetMapper()->GetColors();
	#endif

	if ( actor->GetMapper()->GetScalarVisibility() && colorArray != NULL )
	{
		int scalarMode = actor->GetMapper()->GetScalarMode();

		// There is no point data. 
		if ( scalarMode == VTK_SCALAR_MODE_USE_CELL_DATA ||
			 !polyData->GetPointData()->GetScalars() ) 
			colorPerCell   = 1;
		else
			colorPerVertex = 1;
	}

	osg::ref_ptr< osg::Vec4Array > colors = new osg::Vec4Array;

	// Check to see if there are texture coordinates
	#ifdef VTK4
		vtkDataArray* texCoords = polyData->GetPointData()->GetTCoords();
	#else
		vtkTCoords* texCoords = polyData->GetPointData()->GetTCoords();
	#endif

	osg::ref_ptr< osg::Vec2Array > tcoords = new osg::Vec2Array;

	// Copy data from vtk prim array to osg Geometry
	int prim = 0, vert = 0;
	int i, npts, totpts = 0, *pts, transparentFlag = 0;;

	// Go through cells (primitives)
	for ( primArray->InitTraversal(); primArray->GetNextCell( npts, pts ); prim++ )
	{ 
		geom->addPrimitiveSet( new osg::DrawArrays( primType, totpts, npts ) );
		totpts += npts;
		if (colorPerCell)
		{
			#ifdef VTK4
					   unsigned char *aColor = colorArray->GetPointer(4*prim);
			#else
					 unsigned char *aColor = colorArray->GetPointer(prim);
			#endif

			colors->push_back( osg::Vec4( aColor[0]/255.0f, aColor[1]/255.0f,
										  aColor[2]/255.0f, aColor[3]/255.0f) );
			if ( aColor[3]/255.0f < 1 )
			{
				transparentFlag = 1; 
			}
		}

		if ( normalPerCell )
		{
			vtkReal* aNormal = normals->GetTuple( prim );
			norms->push_back( osg::Vec3( aNormal[0], aNormal[1], aNormal[2] ) );
		}

		// Go through points in cell (verts)
		for (i=0; i < npts; i++)
		{
			vtkReal* aVertex = polyData->GetPoint( pts[i] );
			vertices->push_back( osg::Vec3( aVertex[0], aVertex[1], aVertex[2] ) );
			if ( normalPerVertex )
			{
				vtkReal* aNormal = normals->GetTuple( pts[i] );
				norms->push_back(osg::Vec3( aNormal[0], aNormal[1], aNormal[2] ) );
			}
			

			if ( colorPerVertex )
			{ 
				#ifdef VTK4
					unsigned char *aColor = colorArray->GetPointer(4*pts[i]);
				#else
					unsigned char *aColor = colorArray->GetPointer(pts[i]);
				#endif
				colors->push_back( osg::Vec4( aColor[0]/255.0f, aColor[1]/255.0f,
											  aColor[2]/255.0f, aColor[3]/255.0f ) );
				if ( aColor[3]/255.0f < 1 )
				{
					transparentFlag = 1; 
				}
			}

			if (texCoords != NULL)
			{
				vtkReal* aTCoord = texCoords->GetTuple( pts[i] );
				tcoords->push_back( osg::Vec2( aTCoord[0], aTCoord[1] ) );
			}
			vert++;
		}
	}

	// Add attribute arrays to gset
	geom->setVertexArray( vertices.get() );
    geom->setColorArray( colors.get() );

    if( normals ) 
	{
		geom->setNormalArray(norms.get());
	}
	else
	{
		// Okk lets create the normals.
		osgUtil::SmoothingVisitor sv;
		sv.smooth( *( geom.get() ) );
	}
	if ( normalPerVertex )
	{
        geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	}
	if ( normalPerCell )
	{
        geom->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );
	}
	if ( colorPerVertex )
	{
        geom->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
	}
	else if ( colorPerCell )
	{
        geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	}
	else
	{ 
		// Use overall color (get from Actor)
		vtkReal* actorColor = actor->GetProperty()->GetColor();
		vtkReal opacity     = actor->GetProperty()->GetOpacity();

		colors->push_back( osg::Vec4( actorColor[0], actorColor[1], actorColor[2], opacity ) );
        geom->setColorBinding( osg::Geometry::BIND_OVERALL );
	}
  
	if ( texCoords != NULL )
		geom->setTexCoordArray(0,tcoords.get());

	// Create a geostate for this geoset
	osg::ref_ptr< osg::StateSet > stateset = new osg::StateSet();

	// If not opaque
	if (actor->GetProperty()->GetOpacity() < 1.0 || transparentFlag )
	{
		// Draw last. 
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN); 	   
		stateset->setMode( GL_BLEND,osg::StateAttribute::ON );
		stateset->setMode( GL_CULL_FACE,osg::StateAttribute::OFF );
	}

	// Wireframe
	if ( actor->GetProperty()->GetRepresentation() == VTK_WIREFRAME ) 
	{
		osg::ref_ptr< osg::LineWidth > lineWidth = new osg::LineWidth();
		lineWidth->setWidth( actor->GetProperty()->GetLineWidth() );
		stateset->setAttributeAndModes( lineWidth.get(), osg::StateAttribute::ON );
	} 

	// Backface culling
	if ( !actor->GetProperty()->GetBackfaceCulling() )
	{
		stateset->setMode(GL_CULL_FACE,osg::StateAttribute::OFF);
	}
  
	// Lighting. 
	if ( normals != NULL )
	    stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
	else
	    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

	// If it is lines, turn off lighting. 
	if ( primType == osg::PrimitiveSet::LINE_STRIP )
	    stateset->setMode( GL_LIGHTING,osg::StateAttribute::OFF );

    geom->setStateSet(stateset.get());

	return geom.release();
}

