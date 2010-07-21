
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
//
//  For use with CFDViz for spatial partitioning of Computational Fluid
//  Dynamics visualization.  Will generalize at a later date when 
//  proof of concept is working.
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __EXPERIMENTAL_VPI_SCENARIO_H__
#define __EXPERIMENTAL_VPI_SCENARIO_H__

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Documents/Document.h"

#include "boost/shared_ptr.hpp"

#include <vector>

class Scenario : public Usul::Base::Object
{
public:
  
  typedef Usul::Base::Object																									BaseClass;
  typedef Usul::Interfaces::IUnknown																					Unknown;
	typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI										IVPI;
	typedef IVPI::VPIScenarioEntry																							VPIScenarioEntry;
	typedef IVPI::VPIScenario																										VPIScenario;
	typedef IVPI::VPIScenarios																									VPIScenarios;
	typedef std::vector< std::string >																					StringVec;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Scenario );

  USUL_DECLARE_REF_POINTERS ( Scenario );

  Scenario ();
  virtual ~Scenario();

	void							read( const std::string& filename );
	void							write( const std::string& filename );

	IVPI::VPIScenario	scenario();
	void							scenario( IVPI::VPIScenario );

	

protected:

  StringVec					_getOptions( const std::string& optStr );
  
private:

  // No copying or assignment.
  Scenario ( const Scenario & );
  Scenario &operator = ( const Scenario & );
  
	IVPI::VPIScenario			_scenario;
};
#endif // __EXPERIMENTAL_VPI_SCENARIO_H__

