
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the molecule list
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_GET_MOLECULES_H_
#define _USUL_INTERFACE_GET_MOLECULES_H_

#include "Usul/Interfaces/IUnknown.h"

#include <vector>

namespace osg { template < class T > class ref_ptr; }
namespace osgPlugins { namespace pdb { class Molecule; } }

namespace Usul {
namespace Interfaces {


struct IGetMolecules : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGetMolecules );

  /// Id for this interface.
  enum { IID = 1986733210u };

  typedef osg::ref_ptr< osgPlugins::pdb::Molecule > MoleculePtr;
  typedef std::vector< MoleculePtr > Molecules;

  virtual Molecules& getMolecules() = 0;
}; // class IGetMolecules


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_MOLECULES_DOCUMENT_H_
