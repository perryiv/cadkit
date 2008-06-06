#ifndef _PF_GEODE_H_
#define _PF_GEODE_H_

#include "Performer/pf/pfNode.h"
#include "Performer/pr/pfGeoSet.h"

#include "Standard/SlRefPtr.h"

class pfGeoSet;


class pfGeode : public pfNode
{
public:

  pfGeode() : pfNode(){}

  virtual const char *          className() const { return "pfGeode"; }

  void                          addGSet ( pfGeoSet *gset ) { _gsets.push_back ( gset ); }

  int                           getNumGSets() const { return (int) _gsets.size(); }

  virtual void                  write ( std::ostream &out ) const;

protected:

  typedef std::vector<CadKit::SlRefPtr<pfGeoSet> > GeoSets;
  GeoSets _gsets;

  virtual ~pfGeode(){}

  virtual pfNode *_clone() { return new pfGeode; }
};


inline void pfGeode::write ( std::ostream &out ) const
{
  out << _indent << this->className() << "::_numGeoSets = " << _gsets.size() << std::endl;
  pfNode::write ( out );

  _indent += "  ";

  for ( GeoSets::const_iterator i = _gsets.begin(); i != _gsets.end(); ++i )
    (*i)->write ( out );

  SL_ASSERT ( _indent.size() >= 2 );
  _indent.resize ( _indent.size() - 2 );
}

#endif
