
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Visitor class for setting the job manager.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_CORE_SET_JOB_MANAGER_VISITOR_CLASS_H_
#define _MINERVA_CORE_SET_JOB_MANAGER_VISITOR_CLASS_H_

#include "Minerva/Core/Visitor.h"

namespace Usul { namespace Jobs { class Manager; } }


namespace Minerva {
namespace Core {
namespace Visitors {


class MINERVA_EXPORT SetJobManager : public Minerva::Core::Visitor
{
public:

  // Useful typedefs.
  typedef Minerva::Core::Visitor BaseClass;

  // Reference pointers.
  USUL_DECLARE_REF_POINTERS ( SetJobManager );

  // Constructor
  SetJobManager ( Usul::Jobs::Manager * );

protected:

  // Visit the node.
  virtual void visit ( Minerva::Core::TileEngine::Body & );
  virtual void visit ( Minerva::Core::TileEngine::System & );

  // Use reference counting.
  virtual ~SetJobManager();

private:

  // No copying or assignment.
  SetJobManager ( const SetJobManager & );
  SetJobManager &operator = ( const SetJobManager & );

  Usul::Jobs::Manager *_manager;
};


} // namespace Visitors
} // namespace Core
} // namespace Minerva


#endif // _MINERVA_CORE_SET_JOB_MANAGER_VISITOR_CLASS_H_
