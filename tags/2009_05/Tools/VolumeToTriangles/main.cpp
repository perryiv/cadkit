
#include "vtkDiscreteMarchingCubes.h"
#include "vtkTriangleFilter.h"
#include "vtkBMPReader.h"
#include "vtkTIFFReader.h"
#include "vtkSTLWriter.h"
#include "vtkSmartPointer.h"
#include "vtkImageData.h"

#include <string>

const unsigned int xSize ( 1024 );
const unsigned int ySize ( 884 );
const unsigned int zSize ( 25 );

const unsigned int start ( 1 );

const std::string filepattern ( "D:\\adam\\models\\Yoonessi\\Claycomposites_April06_2006\\Segmentedimagesclaycomposite_April6th2006\\%03d.tif" );
const std::string outfile ( "D:\\adam\\models\\Yoonessi\\Claycomposites_April06_2006\\Segmentedimagesclaycomposite_April6th2006\\model1.stl" );

int main ( int argc, char **argv )
{
  vtkSmartPointer < vtkTIFFReader > reader ( vtkTIFFReader::New() );

  reader->SetFilePattern ( filepattern.c_str() );

  reader->SetDataExtent ( 0, xSize, 0, ySize, 0, zSize );

  reader->SetDataScalarTypeToUnsignedShort();

  reader->SetFileNameSliceOffset( start );

  reader->Update();

  vtkSmartPointer < vtkDiscreteMarchingCubes > surface ( vtkDiscreteMarchingCubes::New() );
  surface->SetInput( reader->GetOutput() );
  surface->SetValue ( 0, 255.0 );
	surface->ComputeScalarsOff();
	surface->ComputeNormalsOn();
	surface->ComputeGradientsOff();

  vtkSmartPointer < vtkTriangleFilter > triangles ( vtkTriangleFilter::New() );
  triangles->SetInput(surface->GetOutput() );

  vtkSmartPointer < vtkSTLWriter > writer ( vtkSTLWriter::New() );
  writer->SetInput( triangles->GetOutput() );
  writer->SetFileTypeToBinary();
  writer->SetFileName( outfile.c_str() ); 
  writer->Write();

  return 0;
}



#if 0
  /// Hack for larry drummy data.
  vtkSmartPointer < vtkTIFFReader > reader ( vtkTIFFReader::New() );

  //reader->SetFilePrefix( "G:\\models\\DrummyTomoStack\\Stack_Close_FillHoles");
  //reader->SetFileName ( "" );
  reader->SetFilePattern ( "G:\\models\\DrummyTomoStack\\Stack_Close_FillHoles\\%2dm.tif" );

  unsigned int xSize ( 834 );
  unsigned int ySize ( 729 );
  unsigned int zSize ( 46 );

  reader->SetDataExtent ( 0, xSize, 0, ySize, 0, zSize );

  reader->SetDataScalarTypeToUnsignedShort();

  reader->SetFileNameSliceOffset( 50 );

  reader->Update();
  
  vtkImageData *temp ( reader->GetOutput() );

  xSize += 2;
  ySize += 2;
  zSize += 2;

  vtkImageData *image ( vtkImageData::New() );
  image->SetSpacing ( 1.0, 1.0, 1.0 );
  image->SetOrigin ( 0.0, 0.0, 0.0 );
  image->SetDimensions ( xSize, ySize, zSize );
  image->SetNumberOfScalarComponents(1);
  image->SetScalarTypeToUnsignedChar();
  image->AllocateScalars();

  // Set the data to all zeros.
  unsigned char *data = static_cast < unsigned char* > ( image->GetScalarPointer( ) );
  ::memset ( data, 0,  xSize * ySize * zSize );

  // Create the final image.
  for ( unsigned int x = 0; x < xSize - 2; ++x )
  {
    for ( unsigned int y = 0; y < ySize - 2; ++y )
    {
      for ( unsigned int z = 0; z < zSize - 2; ++z )
      {
        unsigned char *tempData = static_cast < unsigned char* > ( temp->GetScalarPointer( x, y, z ) );
        unsigned char *data = static_cast < unsigned char* > ( image->GetScalarPointer( x + 1, y + 1  , z + 1 ) );
        
        *data = 255 - *tempData;
      }
    }
  }



  Mesher model;
  model.setUseDecimation ( false );
  model.setDecimation ( 0.75 );
  model.setUsePolygonSmoothing ( false );
  model.setNumberIterations( 20 );
  model.createModel( image );

  vtkSmartPointer < vtkSTLWriter > stlwriter ( vtkSTLWriter::New() );
  stlwriter->SetInput( model.getData() );
  stlwriter->SetFileTypeToBinary();
  stlwriter->SetFileName( "G:\\models\\DrummyTomoStack\\Stack_Close_FillHoles\\model5.stl" ); 
  stlwriter->Write();

  system ( "pause" );

#endif