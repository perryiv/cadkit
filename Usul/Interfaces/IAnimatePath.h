
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IANIMATEPATH_H__
#define __USUL_INTERFACES_IANIMATEPATH_H__

#include "Usul/Interfaces/IUnknown.h"

#include <vector>


namespace Usul {
namespace Interfaces {


struct IAnimatePath : public Usul::Interfaces::IUnknown
{
  /// Typedefs.
  typedef std::vector < double > PackedMatrix;
  typedef std::vector < PackedMatrix > PackedMatrices;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAnimatePath );

  /// Id for this interface.
  enum { IID = 4103645503u };

  // Set an animation path from a vector of matrices.
  virtual void                animatePath ( const PackedMatrices &, Usul::Interfaces::IUnknown::RefPtr notify ) = 0;
  virtual void                animatePath ( const PackedMatrices &, unsigned int stepsPerSpan, Usul::Interfaces::IUnknown::RefPtr notify ) = 0;

  // Is there a path being played?
  virtual bool                isPlaying() const = 0;

  // Stop playing.
  virtual void                stopPlaying() = 0;
};


}
}

#endif // __USUL_INTERFACES_IANIMATEPATH_H__
