
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
//  Callbacks that determine tile splitting behavior.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_SPLIT_CALLBACKS_H__
#define __STAR_SYSTEM_SPLIT_CALLBACKS_H__

#include "StarSystem/Export.h"

#include "Usul/Base/Object.h"

namespace StarSystem { class Tile; }


namespace StarSystem {
namespace Callbacks {


///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all split callbacks.
//
///////////////////////////////////////////////////////////////////////////////

class SplitCallback : public Usul::Base::Object
{
public:

  typedef Usul::Base::Object BaseClass;

  USUL_DECLARE_REF_POINTERS ( SplitCallback );

  // Constructor
  SplitCallback() : BaseClass()
  {
  }

  // Return true if the tile should split.
  virtual bool shouldSplit ( bool suggestion, Tile * ) = 0;

protected:

  // Use reference counting.
  virtual ~SplitCallback()
  {
  }

private:

  // No copying or assignment.
  SplitCallback ( const SplitCallback & );
  SplitCallback &operator = ( const SplitCallback & );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Simple pass-through split callback.
//
///////////////////////////////////////////////////////////////////////////////

class PassThrough : public SplitCallback
{
public:

  typedef SplitCallback BaseClass;

  USUL_DECLARE_REF_POINTERS ( PassThrough );

  // Constructor
  PassThrough() : BaseClass()
  {
  }

  // Return true if the tile should split.
  virtual bool shouldSplit ( bool suggestion, Tile * )
  {
    return suggestion;
  }

protected:

  // Use reference counting.
  virtual ~PassThrough()
  {
  }
};


} // namespace Callbacks
} // namespace StarSystem


#endif // __STAR_SYSTEM_SPLIT_CALLBACKS_H__
