
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Group class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_SAL_GROUP_CLASS_H_
#define _OSG_SAL_GROUP_CLASS_H_

#include "SAL/OSG/Node.h"
#include "SAL/Interfaces/IGroup.h"

namespace osg { class Group; };


namespace SAL {
namespace OSG {


class Group : public SAL::OSG::Node,
              public SAL::Interfaces::IGroup
{
public:

  /// Typedefs.
  typedef SAL::OSG::Node BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Group );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  Group ( osg::Group * );

protected:

  typedef USUL_VALID_REF_POINTER ( osg::Group ) ValidOsgGroupPtr;

  /// No default construction.
  Group();

  /// Do not copy.
  Group ( const Group & );
  Group &operator = ( const Group & );

  /// Use reference counting.
  virtual ~Group();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  SAL::Interfaces::IGroup
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Append the node to list of children.
  virtual void            addChild ( INode * );

  /// Get the i'th child.
  virtual const INode *   childNode ( unsigned long i ) const;
  virtual INode *         childNode ( unsigned long i );

  /// Get the number of children.
  virtual unsigned long   numChildren() const;

  /// Remove all children.
  virtual void            removeAllChildren();

  /// Remove all occurances of the node from the list of children.
  virtual void            removeAllOccurances ( INode * );

  /// Remove the first occurance of the node from the list of children.
  virtual void            removeFirstOccurance ( INode * );

private:

  ValidOsgGroupPtr _group;
};


}; // namespace OSG
}; // namespace SAL


#endif // _OSG_SAL_GROUP_CLASS_H_
