
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  jt2flt: A Jupiter to OpenFlight converter.
//
///////////////////////////////////////////////////////////////////////////////

#define CADKIT_DEFINE_SL_VECTOR_STD_OSTREAM_OPERATOR
#define CADKIT_DEFINE_SL_MATRIX_STD_OSTREAM_FUNCTIONS
#define CADKIT_DEFINE_SL_MATRIX_ISTREAM_FUNCTIONS
#include "Standard/SlMatrix4.h"
#include "Standard/SlConstants.h"
using namespace CadKit;

// For convenience.
typedef std::list<std::string> Filenames;


///////////////////////////////////////////////////////////////////////////////
//
//  Print the usage.
//
///////////////////////////////////////////////////////////////////////////////

void printUsage()
{
  std::cout << "Usage: jt2flt [-m mode] [-s option] [-b option] [-f file] <filename1.jt> <filename2.jt> ..." << std::endl;
  std::cout << "  -m OpenFlight output option: fltMESH (default), fltPOLYGON" << std::endl;
  std::cout << "  -s shape load option: eaiALL_LODS, eaiHIGH_LOD (default)" << std::endl;
  std::cout << "  -b brep load option: eaiTESS_ONLY (default), eaiBREP_ONLY, eaiTESS_AND_BREP" << std::endl;
  std::cout << "  -f filename with a list of .jt files" << std::endl;
  std::cout << "  -p print polygon file (fltPOLYGON must be set): (default off)" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the command line and set the corresponding properties.
//
///////////////////////////////////////////////////////////////////////////////

bool parseCommandLine ( const int &argc, const char **argv, IGmtUnknown::Ptr &unknownRead, IGmtUnknown::Ptr &unknownWrite, Filenames &filenames )
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate the file.
//
///////////////////////////////////////////////////////////////////////////////

int translate ( const Filenames &filenames, IGmtUnknown::Ptr &unknownRead, IGmtUnknown::Ptr &unknownWrite )
{
  // Declare an instance of the Jupiter parser.
  JupiterRead in;

  // Get its generic interface.
  IGmtUnknown::Ptr unknownRead = in.getIGmtUnknown();
  if ( unknownRead.isNull() )
  {
    std::cout << "Failed to obtain IGmtUnknown interface from JupiterRead instance." << std::endl;
    return 0;
  }

  // Get the model-reading interface.
  IGmtModelRead::Ptr readModel = static_cast<IGmtModelRead *> ( unknownRead->queryInterface ( IID_IGmtModelRead ) );
  if ( readModel.isNull() )
  {
    std::cout << "Failed to obtain IGmtModelRead interface from unknownRead." << std::endl;
    return 0;
  }

  // Declare an instance of the Open Flight writer.
  OpenFlightWrite out;

  // Get its generic interface.
  IGmtUnknown::Ptr unknownWrite = out.getIGmtUnknown();
  if ( unknownWrite.isNull() )
  {
    std::cout << "Failed to obtain IGmtUnknown interface from OpenFlightWrite instance." << std::endl;
    return 0;
  }

  // Get the model-writng interface.
  IGmtModelWrite::Ptr writeModel = static_cast<IGmtModelWrite *> ( unknownWrite->queryInterface ( IID_IGmtModelWrite ) );
  if ( writeModel.isNull() )
  {
    std::cout << "Failed to obtain IGmtModelWrite interface from unknownWrite." << std::endl;
    return 0;
  }

  // Parse the command line.
  if ( false == ::parseCommandLine ( argc, argv, readModel, writeModel ) )
  {
    std::cout << "Unrecoverable command-line parse error." << std::endl;
    return 0;
  }

  // Read the model. When the model is read the pointer to unknownWrite will 
  // be used to query for suitable interfaces.
  if ( false == readModel->readModel ( unknownWrite ) )
  {
    std::cout << "IGmtModelRead::readModel() failed for parseWriteArgs." << std::endl;
    return 0;
  }

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // There should be at least one argument.
  if ( argc < 2 )
  {
    ::printUsage();
    return 0;
  }

  // Declare an instance of the Jupiter parser.
  JupiterRead in;

  // Get its generic interface.
  IGmtUnknown::Ptr unknownRead = in.getIGmtUnknown();
  if ( unknownRead.isNull() )
  {
    std::cout << "Failed to obtain IGmtUnknown interface from JupiterRead instance." << std::endl;
    return 0;
  }

  // Declare an instance of the Open Flight writer.
  OpenFlightWrite out;

  // Get its generic interface.
  IGmtUnknown::Ptr unknownWrite = out.getIGmtUnknown();
  if ( unknownWrite.isNull() )
  {
    std::cout << "Failed to obtain IGmtUnknown interface from OpenFlightWrite instance." << std::endl;
    return 0;
  }

  // Parse the command line.
  Filenames filenames;
  if ( false == ::parseCommandLine ( argc, argv, unknownRead, unknownWrite, filenames ) )
  {
    std::cout << "Unrecoverable command-line parse error." << std::endl;
    return 0;
  }

  // Read the model. When the model is read the pointer to unknownWrite will 
  // be used to query for suitable interfaces.
  if ( false == ::translate ( unknownRead, unknownWrite ) )
  {
    std::cout << "Failed to translate file for parseWriteArgs." << std::endl;
    return 0;
  }

  return 1;
}


Decide if you will declare a new parser for each file to be converted, or just use the same one over and over.
