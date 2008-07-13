
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_LEGEND_IMAGE_H__
#define __OSG_TOOLS_LEGEND_IMAGE_H__

#include "OsgTools/Export.h"
#include "OsgTools/Widgets/Item.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

namespace osg { class Node; }

namespace OsgTools {
namespace Widgets {

class OSG_TOOLS_EXPORT Image : public OsgTools::Widgets::Item
{
public:
  typedef OsgTools::Widgets::Item BaseClass;
  
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Image );
  
  /// Constructors.
  Image();
  Image( const std::string& filename );
  
  /// Get/Set the text.
  void                    filename ( const std::string& filename );
  const std::string &     filename() const;
  
  /// Build the scene.
  osg::Node*              buildScene();
  
protected:
  virtual ~Image();
  
private:
  std::string _filename;
};

}
}


#endif //__OSG_TOOLS_LEGEND_IMAGE_H__
