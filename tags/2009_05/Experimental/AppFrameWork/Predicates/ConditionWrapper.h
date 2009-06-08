
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate class that wraps a condition.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_PREDICATE_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_PREDICATE_CONDITION_CLASS_H_

#include "AppFrameWork/Core/Export.h"

#include <functional>

namespace AFW { namespace Core { class Object; } }
namespace AFW { namespace Conditions { class Condition; } }


namespace AFW {
namespace Predicates {


class APP_FRAME_WORK_EXPORT ConditionWrapper : public std::unary_function < const AFW::Core::Object *, bool >
{
public:

  typedef std::unary_function < const AFW::Core::Object *, bool > BaseClass;

  ConditionWrapper ( AFW::Conditions::Condition * );
  ConditionWrapper ( const ConditionWrapper & );
  ~ConditionWrapper();

  ConditionWrapper &operator() ( const ConditionWrapper & );

  bool operator () ( AFW::Core::Object * ) const;

  template < class SmartPointerType > bool operator () ( SmartPointerType &p ) { return ConditionWrapper::operator () ( p.get() ); }

protected:

  void            _set ( AFW::Conditions::Condition * );

private:

  AFW::Conditions::Condition *_c;
};


}; // namespace Predicates
}; // namespace AFW


#endif // _APP_FRAME_WORK_PREDICATE_CONDITION_CLASS_H_
