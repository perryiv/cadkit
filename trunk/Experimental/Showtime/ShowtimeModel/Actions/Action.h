
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Action for showtime.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SHOW_TIME_MODEL_ACTION_H_
#define _SHOW_TIME_MODEL_ACTION_H_

#include "ShowtimeModel/CompileGuard.h"

#include "Usul/Base/Object.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Interfaces/IUnknown.h"

#include <map>
#include <string>

namespace XmlTree { class Node; }
class ShowtimeDocument;


namespace Showtime {
namespace Actions {


class Action : public Usul::Base::Object
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Math::Vec2ui StepRange;
  typedef std::map<std::string,std::string> Attributes;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Action );

  // Get the attribute.
  std::string                 attribute ( const std::string &name ) const;

  // Execute the action.
  virtual void                execute ( unsigned int step );

  // Initialize.
  virtual void                init ( ShowtimeDocument *, const XmlTree::Node * );

  // Is the step in range?
  bool                        isInRange ( unsigned int step ) const;

  // Get the model.
  std::string                 model() const;

  // Get the step-range.
  StepRange                   stepRange() const;

protected:

  // Construction.
  Action();

  // Use reference counting.
  virtual ~Action();

  ShowtimeDocument *          _getDocument();

private:

  // Do not copy.
  Action ( const Action & );
  Action &operator = ( const Action & );

  void                        _destroy();

private:

  Attributes _attributes;
  std::string _model;
  StepRange _stepRange;
  ShowtimeDocument *_document;
};


} // namespace Action
} // namespace Showtime


#endif // _SHOW_TIME_MODEL_ACTION_H_
