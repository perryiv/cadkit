

///////////////////////////////////////////////////////////////////////////////

//

//  Copyright (c) 2002, Perry L. Miller IV

//  All rights reserved.

//  BSD License: http://www.opensource.org/licenses/bsd-license.html

//

///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////

//

//  DbJtVisApiHeaders: Include this to get the VisAPI headers.

//

///////////////////////////////////////////////////////////////////////////////



#ifndef _CADKIT_DATABASE_JUPITER_VIS_API_HEADERS_H_

#define _CADKIT_DATABASE_JUPITER_VIS_API_HEADERS_H_

#ifdef _CADKIT_USE_JTOPEN

#include "JtTk/JtkEntityFactory.h"

#include "JtTk/JtkCADImporter.h"

#include "JtTk/JtkTraverser.h"

#include "JtTk/JtkAttrib.h"

#include "JtTk/JtkLineStripSet.h"

#include "JtTk/JtkPointSet.h"

#include "JtTk/JtkPolygonSet.h"

#include "JtTk/JtkTriStripSet.h"

// Map JT Open types to DMDTk types

// constant redefinitions
#define eaiALL_LODS JtkALL_LODS
#define eaiHIGH_LOD JtkHIGH_LOD
#define eaiTESS_ONLY JtkTESS_ONLY
#define eaiBREP_ONLY JtkBREP_ONLY
#define eaiTESS_AND_BREP JtkTESS_AND_BREP
#define eaiINSTANCE_ASSEMBLY JtkINSTANCE_ASSEMBLY
#define eaiEXPLODE_ASSEMBLY JtkEXPLODE_ASSEMBLY
#define eaiASSEMBLY JtkASSEMBLY
#define eaiPART JtkPART
#define eaiINSTANCE JtkINSTANCE
#define eai_OK Jtk_OK
#define eai_ERROR Jtk_ERROR
#define eaiNONE JtkNONE
#define eaiPOLYGONSET JtkPOLYGONSET
#define eaiLINESTRIPSET JtkLINESTRIPSET
#define eaiPOINTSET JtkPOINTSET
#define eaiTRISTRIPSET JtkTRISTRIPSET
#define eaiBOXSET JtkBOXSET
#define eaiCYLINDERSET JtkCYLINDERSET
#define eaiPYRAMIDSET JtkPYRAMIDSET
#define eaiSPHERESET JtkSPHERESET
#define eaiTRIPRISMSET JtkTRIPRISMSET

// class redefinitions
#define eaiEntity JtkEntity
#define eaiCADImporter JtkCADImporter
#define eaiEntityFactory JtkEntityFactory
#define eaiTraverser JtkTraverser
#define eaiMaterial JtkMaterial
#define eaiTexImage JtkTexImage
#define eaiAssembly JtkAssembly
#define eaiPointSet JtkPointSet
#define eaiPart JtkPart
#define eaiInstance JtkInstance
#define eaiShape JtkShape
#define eaiLineStripSet JtkLineStripSet
#define eaiPolygonSet JtkPolygonSet
#define eaiTriStripSet JtkTriStripSet
#define eaiHierarchy JtkHierarchy
#define eaiClientData JtkClientData
#define eaiTransform JtkTransform


#else // _CADKIT_USE_JTOPEN

#ifdef _WIN32



// This hack is because DMDataTk/eaiStandard.h (which is indirectly included 

// below) includes <iostream.h>. This causes VC++ error C2874 with STLport.

#define EAI_STANDARD_HXX // From DMDataTk/eaiStandard.h

#define EAI_TOOLKIT_API __declspec(dllimport)

enum { eai_ERROR = 0, eai_OK = 1 };

enum Units { UNKNOWN=0, MICROMETERS, MILLIMETERS, CENTIMETERS, DECIMETERS, 

             METERS, KILOMETERS, INCHES, FEET, YARDS, MILES, MILS };



// DMDTk 5.2 introduced <iostream.h> in DMDataTk/eaiEntityList.h.

#define EAI_ENTITY_LIST_HXX

class eaiEntityList;



#pragma warning ( disable: 4251 )



#endif // _WIN32



#include <stdlib.h> // For "NULL", used in eaiStandard.h, 

                    // indirectly included by eaiEntityFactory.h



#include "DMDataTk/eaiEntityFactory.h" // Doesn't compile in DbJtPrecompiled.h

#include "DMDataTk/eaiCADImporter.h"

#include "DMDataTk/eaiTraverser.h"

#include "DMDataTk/eaiAttrib.h"

#include "DMDataTk/eaiLineStripSet.h"

#include "DMDataTk/eaiPointSet.h"

#include "DMDataTk/eaiPolygonSet.h"

#include "DMDataTk/eaiTriStripSet.h"

#include "DMDataTk/eaiClientData.h"

#endif // _CADKIT_USE_JTOPEN

#endif // _CADKIT_DATABASE_JUPITER_VIS_API_HEADERS_H_

