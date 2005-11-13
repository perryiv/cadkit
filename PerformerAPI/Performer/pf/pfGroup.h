#ifndef a1ce9aff2_aa84_4cad_b216_189200cb8a16
#define a1ce9aff2_aa84_4cad_b216_189200cb8a16

#include "Performer/pf/pfNode.h"
#include "Standard/SlRefPtr.h"


class pfGroup : public pfNode
{
public:
  pfGroup() : pfNode(){}
  int addChild ( pfNode *node ) { _kids.push_back ( node ); return _kids.size(); }
  pfNode* getChild ( int i ) const { return _kids[i].getValue(); }
  int getNumChildren() const { return _kids.size(); }
protected:
  std::vector<CadKit::SlRefPtr<pfNode> > _kids;
  virtual ~pfGroup(){}
};

#endif
