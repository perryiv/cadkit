
#ifndef __VTKCONV_H__
#define __VTKCONV_H__

#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkCellArray.h"
#include "vtkPolyDataWriter.h"
#include "vtkPolyDataReader.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkPointData.h"
#include "vtkCellData.h"

#include "SimFlowUtil/File/FileOperations.h"

#include <vector>

namespace SimFlow
{
	namespace VTKConv
	{
		void convVTKAsciiToBinary( const char* fileName, const std::string& destDir );

		void convVTKAsciiToBinary( const std::string& sourceDir, const std::string& destDir  )
		{
			std::vector< std::string > fileNames;
			
			SimFlowUtil::File::findFile(  sourceDir,  "vtk", fileNames, true );

			std::vector< std::string >::iterator itr;

			for( itr = fileNames.begin(); itr != fileNames.end(); ++itr )
			{
				convVTKAsciiToBinary( (*itr).c_str(), destDir );
			}
		}

		void convVTKAsciiToBinary( const char* fileName, const std::string& destDir )
		{
			// @Todo: Check whether this file really requires the conversion or not. 
			std::cout << "Converting file" << fileName << " from ASCII to VTK BINARY format " << std::endl;
			
			// @Todo: Check for different data types here. 
			vtkPolyDataReader* reader = vtkPolyDataReader::New();
			reader->SetFileName( fileName );
			reader->Update();

			vtkPolyDataMapper* map = vtkPolyDataMapper::New();
			map->SetInput( reader->GetOutput() );

			vtkActor* actor = vtkActor::New();
			actor->SetMapper( map );			

			// Get poly data. 
			vtkPolyData *polyData = ( vtkPolyData *) actor->GetMapper()->GetInput();

			vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
			writer->SetInput( polyData );
			//writer->Update();			

			// Lets check whether or not we have the destDir already created. 
			std::string destPath;

			if( !SimFlowUtil::File::exists( destDir ) )
			{				
				SimFlowUtil::File::create( destDir );
			}
			
			writer->SetFileName( SimFlowUtil::File::combine( destDir, fileName ).c_str() );
			writer->SetFileTypeToBinary();
			writer->Write();
		}
	}
}

#endif // __VTKCONV_H__
