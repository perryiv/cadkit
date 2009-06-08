
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
  unsigned int      getCount ( const SlType *type ) const;

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
