#ifndef _PF_NODE_H_
#define _PF_NODE_H_

#include "Performer/pr/pfObject.h"

#include <string>

class pfSphere;


class pfNode : public pfObject
{
public:

  virtual const char *          className() const { return "pfNode"; }

  pfNode *                      clone ( int mode ) { return this->_clone(); }

  int                           getBound ( pfSphere *bsph ) { return 1; }
  const char *                  getName() const { return _name.c_str(); }

  void                          setName ( const char *name ) { _name = name; }

  virtual void                  write ( std::ostream &out ) const;

protected:

  std::string _name;

  pfNode() : pfObject(){}
  virtual ~pfNode(){}

  virtual pfNode *  _clone() = 0;
};


inline void pfNode::write ( std::ostream &out ) const
{
  out << _indent << this->className() << "::_name = " << this->getName() << std::endl;
}


#endif
