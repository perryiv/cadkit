#ifndef a163d9c44_f7bb_4590_9b2c_5ae871dbb532
#define a163d9c44_f7bb_4590_9b2c_5ae871dbb532

#include "Performer/pr/pfObject.h"

#include <string>

class pfSphere;

class pfNode : public pfObject
{
public:
  pfNode *clone ( int mode ) { return this->_clone(); }
  int getBound ( pfSphere *bsph ) { return 1; }
  const char *getName() const { return _name.c_str(); }
  void setName ( const char *name ) { _name = name; }
protected:
  std::string _name;
  pfNode() : pfObject(){}
  virtual ~pfNode(){}
  virtual pfNode *_clone() = 0;
};

#endif
