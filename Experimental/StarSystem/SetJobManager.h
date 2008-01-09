
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

#ifndef _STAR_SYSTEM_SET_JOB_MANAGER_VISITOR_CLASS_H_
#define _STAR_SYSTEM_SET_JOB_MANAGER_VISITOR_CLASS_H_

#include "StarSystem/Visitor.h"

namespace Usul { namespace Jobs { class Manager; } }


namespace StarSystem {


class STAR_SYSTEM_EXPORT SetJobManager : public Visitor
{
public:

  // Useful typedefs.
  typedef Visitor BaseClass;

  // Reference pointers.
  USUL_DECLARE_REF_POINTERS ( SetJobManager );

  // Constructor
  SetJobManager ( Usul::Jobs::Manager * );

protected:

  // Visit the node.
  virtual void visit ( Body & );
  virtual void visit ( System & );

  // Use reference counting.
  virtual ~SetJobManager();

private:

  // No copying or assignment.
  SetJobManager ( const SetJobManager & );
  SetJobManager &operator = ( const SetJobManager & );

  Usul::Jobs::Manager *_manager;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_SET_JOB_MANAGER_VISITOR_CLASS_H_
