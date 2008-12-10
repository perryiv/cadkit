
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

namespace osg { class Node; class Image; }

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
  
  /// Estimate the needed size for the item.
  virtual Size            estimateSizeForWidth ( unsigned int w ) const;
  
  /// Get/Set the text.
  void                    filename ( const std::string& filename );
  const std::string &     filename() const;

  /// Build the scene.
  osg::Node*              buildScene ( unsigned int depth = 0 );
  
protected:
  
  virtual ~Image();
  
  static Size             _fitImageToSize ( const Size& size, const osg::Image& image );
  
private:
  std::string _filename;
};

}
}


#endif //__OSG_TOOLS_LEGEND_IMAGE_H__
