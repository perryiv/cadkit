
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for a group of conditions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_GROUP_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_GROUP_CONDITION_CLASS_H_

#include "AppFrameWork/Conditions/Condition.h"

#include <vector>


namespace AFW {
namespace Conditions {


class APP_FRAME_WORK_EXPORT Group : public Condition
{
public:

  // Typedefs.
  typedef Condition BaseClass;
  typedef std::vector < Condition::ValidRefPtr > Children;
  typedef Children::iterator Iterator;
  typedef Children::const_iterator ConstIterator;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Group );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( Group );

  // Constructor
  Group ( bool want = true );

  // Append a condition.
  void                  append ( AFW::Conditions::Condition * );

  // Iterators to the children.
  Iterator              begin()       { return _kids.begin(); }
  ConstIterator         begin() const { return _kids.begin(); }

  // Iterators to the children.
  Iterator              end()       { return _kids.end(); }
  ConstIterator         end() const { return _kids.end(); }

protected:

  // Use reference counting.
  virtual ~Group();

private:

  // No copying.
  Group ( const Group & );
  Group &operator = ( const Group & );

  Children _kids;
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_GROUP_CONDITION_CLASS_H_
