#ifndef _PF_DU_H_
#define _PF_DU_H_

#include <fstream>

#include "Performer/pf/pfNode.h"

inline int pfdStoreFile ( pfNode *root, const char *filename )
{
  if ( 0x0 == root )
    return 0;

  if ( 0x0 == filename )
    return 0;

  std::ofstream out ( filename );

  if ( false == out.is_open() )
    return 0;

  root->write ( out );
  return 1;
}

#endif
