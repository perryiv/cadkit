#ifndef _PF_GROUP_H_
#define _PF_GROUP_H_

#include "Performer/pf/pfNode.h"

#include "Standard/SlRefPtr.h"
#include "Standard/SlAssert.h"

#include <vector>

class pfGroup : public pfNode
{
public:

  pfGroup() : pfNode(){}

  int                           addChild ( pfNode *node ) { _kids.push_back ( node ); return _kids.size(); }

  virtual const char *          className() const { return "pfGroup"; }

  pfNode *                      getChild ( int i ) const { return _kids[i].getValue(); }
  int                           getNumChildren() const { return (int) _kids.size(); }
  
  virtual void                  write ( std::ostream &out ) const;

protected:

  typedef std::vector<CadKit::SlRefPtr<pfNode> > Kids;
  Kids _kids;

  virtual ~pfGroup(){}
  virtual pfNode *_clone() { return new pfGroup; }
};


inline void pfGroup::write ( std::ostream &out ) const
{
  out << _indent << this->className() << "::_numChildren = " << _kids.size() << std::endl;
  pfNode::write ( out );

  _indent += "  ";

  for ( Kids::const_iterator i = _kids.begin(); i != _kids.end(); ++i )
    (*i)->write ( out );

  SL_ASSERT ( _indent.size() >= 2 );
  _indent.resize ( _indent.size() - 2 );
}

#endif
