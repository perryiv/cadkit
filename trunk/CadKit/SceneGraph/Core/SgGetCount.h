
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgGetCount: The counting visitor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_GET_COUNT_VISITOR_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_GET_COUNT_VISITOR_H_

#include "SgVisitor.h"


namespace CadKit
{
class SG_API SgGetCount : public SgVisitor
{
public:

  SgGetCount();
  virtual ~SgGetCount();

  // Call this to count the items.
  bool              count ( SgNode &scene );

  // Get the count for the specified type. Call count() first.
  IndexGetCount     getCount ( const SlType *type ) const;

protected:

  // Hiding the std::map because of VC++ warning 4786. 
  // The pragma isn't helping.
  void *_countMap;

  void              _incrementCount ( const SlType *type );

  // The visit functions.
  SG_DECLARE_ALL_VISITOR_FUNCTIONS;

  SG_DECLARE_DYNAMIC_VISITOR ( SgGetCount, 0x00001014 );
};

}; // namespace CadKit


/////////////////////////////////////////////////////////////////////////////
//
//  Macro for defining the nested visitor functions.
//
/////////////////////////////////////////////////////////////////////////////

#define SG_VISIT_FUNCTION_GET_COUNT(class_name,base_class) \
bool SgGetCount::visit ( class_name &node ) \
{ \
  SL_ASSERT ( this ); \
  this->_incrementCount ( class_name::getClassType() ); \
  return node.base_class::accept ( *this ); \
}


#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_GET_COUNT_VISITOR_H_
