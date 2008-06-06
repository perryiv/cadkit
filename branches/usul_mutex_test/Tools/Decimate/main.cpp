
#include "vtkSTLReader.h"
#include "vtkSTLWriter.h"
#include "vtkDecimatePro.h"
#include "vtkSmartPointer.h"

#include <cstdlib>

int main ( int argc, char ** argv )
{
  if( argc < 4 )
    return -1;

  
  vtkSmartPointer < vtkSTLReader > reader ( vtkSTLReader::New() );
  reader->SetFileName( argv[1] );

  vtkSmartPointer < vtkDecimatePro > decimate ( vtkDecimatePro::New() );
  decimate->SetInput( reader->GetOutput() );
  decimate->PreserveTopologyOn();
  decimate->SetTargetReduction ( ::atof( argv[3] ) );

  vtkSmartPointer < vtkSTLWriter > writer ( vtkSTLWriter::New() );
  writer->SetInput( decimate->GetOutput() );
  writer->SetFileTypeToBinary();
  writer->SetFileName( argv[2] ); 
  writer->Write();

  return 0;
}

