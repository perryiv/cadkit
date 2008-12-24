
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For jitter-based antialiasing.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_JITTER_H_
#define _OSG_TOOLS_JITTER_H_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

#include "Usul/Math/Vector2.h"

#include <vector>
#include <string>

namespace OsgTools {


class OSG_TOOLS_EXPORT Jitter
{
public:

  // Useful typedefs.
  typedef Usul::Math::Vec2d Point;
  typedef std::vector < Point > Points;
  typedef std::vector < Points > Methods;
  typedef std::pair < unsigned int, std::string > MethodPair;
  typedef std::vector < MethodPair > Available;

  // Get list of available methods.
  Available           available() const;

  // Is this method available?
  bool                available ( unsigned int ) const;

  // It's a singleton.
  static Jitter &     instance();

  // Get jittered perspective matrix for given number of passes.
  void                perspective ( unsigned int numPasses, 
                                    unsigned int currentPass, 
                                    const osg::Viewport &vp, 
                                    const osg::Matrixd &base, 
                                    osg::Matrixd &mat );

  // Set/get the scatter scale.
  double              scatterScale() const;
  void                scatterScale ( double );

private:

  Jitter();
  ~Jitter();
  Jitter ( const Jitter & );
  Jitter &operator = ( const Jitter & );

  static Jitter *_instance;
  Methods _methods;
  double _scatterScale;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_JITTER_H_
