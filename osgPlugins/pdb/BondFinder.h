///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_PDB_PLUGIN_BOND_FINDER_H__
#define __OSG_PDB_PLUGIN_BOND_FINDER_H__

#include <vector>
#include <map>
#include <algorithm>
#include <list>

#include "Atom.h"
#include "Bond.h"

class BondFinder
{
public:
  typedef std::pair< Atom::ID, Atom > AtomPair;
  typedef std::map<Atom::ID, Atom >  Map;
  typedef std::vector< Atom > Atoms;
  typedef std::list< Bond > Bonds;

  struct SortAtom : public std::binary_function< Atom, Atom, bool>
  {
    bool operator() (const Atom &a, const Atom &b)
    {
      return (a.length2() < b.length2());
    }
  };

  BondFinder() {}
  
  void findBonds(const Atoms& atoms, Bonds &bonds)
  {
    float maxRadius = 3.0f;  // TODO get max radius from periodic table
    Atoms sorted = atoms;
    //sort atoms by distance^2 from the origin
    std::sort(sorted.begin(), sorted.end(), SortAtom());

    for(Atoms::iterator i = sorted.begin(); i != sorted.end(); ++i)
    {
      const Atom &atom = *i;
      float radius = atom.getRadius();
      float delta = distanceFunc(radius, maxRadius);
      float distance = atom.length2();
      //TODO fix MaxDistance so don't have to check atoms beyond it
      float maxDistance = distance + (delta + radius + maxRadius) * (delta + radius + maxRadius);
      Atoms::iterator walking = i;
      //start walking forwards
      while(1)//walking->length2() < maxDistance)
      {
        if(walking != i)
        {
          float check = distanceFunc(walking->getRadius(), radius) + radius + walking->getRadius();
          check *= check;
          float d = (walking->getVec3() - atom.getVec3()).length2();
          if(d <= check)
            bonds.push_back( Bond(atom, *walking, bonds.size() + 1));
        }
        ++walking;
        if(walking >= sorted.end())
          break;
      }
    }
  }

  Atoms mapToVector(const Map& m )
  {
    Atoms atoms;
    atoms.reserve(m.size());
    for(Map::const_iterator i = m.begin(); i != m.end(); ++i)
    { 
      atoms.push_back(i->second);
    }
    return atoms;
  }

private:
  float distanceFunc(float r1, float r2)
  {
    return 0.6 * r1 * r2;
  }
};

#endif
