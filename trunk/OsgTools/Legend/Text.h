
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_LEGEND_TEXT_H__
#define __OSG_TOOLS_LEGEND_TEXT_H__

#include "OsgTools/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

namespace osg { class Node; }

namespace OsgTools {
namespace Legend {

class OSG_TOOLS_EXPORT Text : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Text );

  Text();

  /// Get/Set the text.
  void                    text( const std::string& text );
  const std::string &     text () const;

  /// Build the scene.
  osg::Node*              buildScene();

protected:
  virtual ~Text();

private:
  std::string _text;
};

}
}


#endif //__OSG_TOOLS_LEGEND_TEXT_H__