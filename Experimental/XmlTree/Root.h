
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  XML tree root class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_TREE_ROOT_H_
#define _XML_TREE_ROOT_H_

#include "XmlTree/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

namespace XmlTree { namespace Detail { class RootImpl; } }


namespace XmlTree {


class XML_TREE_EXPORT Root : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Root );

  // Construction
  Root();
  Root ( const std::string & );

  // Clear the tree.
  void                                clear();

  // Load contents of file.
  void                                load ( const std::string & );

protected:

  // Use reference counting.
  virtual ~Root();

private:

  // Do not copy.
  Root ( const Root & );
  Root &operator = ( const Root & );

  XmlTree::Detail::RootImpl *_root;
};


} // namespace XmlTree


#endif // _XML_TREE_ROOT_H_
