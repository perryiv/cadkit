
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VTKUTIL_VTK_READER_H__
#define __VTKUTIL_VTK_READER_H__

#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkPolyDataReader.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkStructuredGridReader.h"
#include "vtkStructuredGrid.h"
#include "vtkStructuredGridGeometryFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkDelaunay3D.h"
#include "vtkLoopSubdivisionFilter.h"
#include "vtkTriangleFilter.h"
#include "vtkSmartPointer.h"

#include "SimFlow/VTKUtil/VTKToOSG.h"

namespace osg { class Node; }

namespace SimFlow
{
	namespace VTKUtil
	{
		class OSGVTK_EXPORT VTKReader
		{
		public:
			
			enum VTKDataType
			{        
				POLYGONALDATA   = 0, 		  
				STRUCTUREDGRID,		
				RECTILINEARGRID,		
				UNSTRUCTUREDGRID	
			};


			static osg::Node* readVTKFile( const std::string& fileName, unsigned int type=0 )
			{
				return readVTKFile( fileName.c_str(), type );
			}

			static osg::Node* readVTKFile( const char*  fileName, unsigned int type=0 )
			{
				switch( type )
				{
					case POLYGONALDATA : 
					{
            osg::ref_ptr< osg::Node > node = dynamic_cast< osg::Node* >( VTKToOSG::vtkActorToOSG( readPolygonalData( fileName ) ) );
						return node.release();	
						break;
					}
					case STRUCTUREDGRID:
					{
            osg::ref_ptr< osg::Node > node = dynamic_cast< osg::Node* >( VTKToOSG::vtkActorToOSG( readStructuredGrid( fileName ) ) );
						return node.release();	
						break;
					}
					default:
					{
						throw std::runtime_error( std::string( "[ERROR:ERRORNO] Type not handled by the reader: "  ) );
						break;
					}
				}
			}

			static vtkActor* readPolygonalData( const char* fileName )
			{
				// Read the poly data file.
        vtkSmartPointer< vtkPolyDataReader > reader( vtkPolyDataReader::New() );	
				reader->SetFileName( fileName );
				reader->Update();

        vtkSmartPointer< vtkPolyDataMapper > map( vtkPolyDataMapper::New() );
				map->SetInput( reader->GetOutput() );
				
				vtkActor *actor = vtkActor::New();
				actor->SetMapper(map);

				return actor;
			}

			static vtkActor* readStructuredGrid( const char* fileName )
			{
				vtkSmartPointer< vtkStructuredGridReader > sgReader( vtkStructuredGridReader::New() );	
				sgReader->SetFileName( fileName );
				sgReader->Update();
				
				vtkSmartPointer< vtkStructuredGrid > sGrid( sgReader->GetOutput() );									
        vtkSmartPointer< vtkStructuredGridGeometryFilter > sgGeomFil( vtkStructuredGridGeometryFilter::New() );				
				sgGeomFil->SetInput( sGrid );				
        sgGeomFil->SetExtent( sGrid->GetExtent()[0], sGrid->GetExtent()[1], sGrid->GetExtent()[2], sGrid->GetExtent()[3], 0, 0 );
        //sgGeomFil->SetExtent( 0, 100, 20, 20, 0, 10 );
				sgGeomFil->Update();

        /*
        vtkSmartPointer< vtkTriangleFilter > triangulator( vtkTriangleFilter::New() );
        triangulator->SetInput( sgGeomFil->GetOutput() );
        
        vtkSmartPointer< vtkLoopSubdivisionFilter > subdivide( vtkLoopSubdivisionFilter::New() );        
        subdivide->SetInput( triangulator->GetOutput() );
        subdivide->SetNumberOfSubdivisions( 4 );     			
        */				

        vtkSmartPointer< vtkPolyDataNormals > normals( vtkPolyDataNormals::New() );
        normals->SetInputConnection( sgGeomFil->GetOutputPort() );
        //normals->SetInputConnection( subdivide->GetOutputPort() );
        
        vtkSmartPointer< vtkPolyDataMapper > mapper( vtkPolyDataMapper::New() );
        mapper->SetInputConnection( normals->GetOutputPort() );
				
        vtkActor* actor = vtkActor::New();
				actor->SetMapper( mapper );
				return actor;
			}
		};
	}
}

#endif // __VTKUTIL_VTK_READER_H__

