
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgText.h: Text node.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_TEXT_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_TEXT_H_

#include "SgShape.h"

#include <string>


namespace CadKit
{
class SG_API SgText : public SgShape
{
public:

  SgText();
  SgText ( const SgText &text );
  SgText ( const std::string &text );

  const std::string &     getText() const { return _text; }
  void                    setText ( const std::string &text ) { _text = text; }

protected:

  virtual ~SgText();

  std::string _text;

  SG_DECLARE_DYNAMIC_NODE ( SgText, 1047871636 );
};
};

#endif
