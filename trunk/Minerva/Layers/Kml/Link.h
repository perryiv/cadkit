
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_LINK_H__
#define __MINERVA_LAYERS_KML_LINK_H__

#include "Usul/Base/Object.h"

#include <string>

namespace Minerva {
namespace Layers {
namespace Kml {
      
      
class Link : public Usul::Base::Object
{
public:
  // Typedefs.
  typedef Usul::Base::Object BaseClass;

  // Declare pointers.
  USUL_DECLARE_REF_POINTERS ( Link );
  
  // Refresh mode.
  enum RefreshMode
  {
    ON_CHANGE,
    ON_INTERVAL,
    ON_EXPIRE
  };
  
  Link();
  
  /// Get/Set the href.
  void                href( const std::string& );
  const std::string&  href() const;
  
  /// Get/Set refresh interval.
  void                refreshInterval ( double );
  double              refreshInterval() const;
  
  /// Get/Set refresh mode.

protected:
  virtual ~Link();
  
private:
  std::string _href;
  double _refreshInterval;
  RefreshMode _refreshMode;
};
      
      
}
}
}


#endif // __MINERVA_LAYERS_KML_LINK_H__
