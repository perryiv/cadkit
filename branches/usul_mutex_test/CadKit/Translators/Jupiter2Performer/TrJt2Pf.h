
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  TrJt2Pf: The Jupiter to Performer translator.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_JUPITER_TO_PERFORMER_TRANSLATER_H_
#define _CADKIT_JUPITER_TO_PERFORMER_TRANSLATER_H_

#include "TrJt2PfApi.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Database/Jupiter/DbJtTraverser.h"
# include "Standard/SlMaterial.h"
# include <list>
# include <string>
# include <vector>
# include <map>
#endif

class pfDCS;
class pfGroup;
class pfNode;
class pfMemory;
class pfGeode;
class pfGeoSet;
class pfGeoState;
struct pfVec2;
struct pfVec3;
struct pfVec4;


namespace CadKit
{
class DbJtTraverser;

class TR_JT_2_PF_API TrJt2Pf
{
public:

  TrJt2Pf();
  ~TrJt2Pf();

  // Get the last error.
  const std::string &     getLastError() const { return _error; }

  // Initialize. Call this before doing anything else.
  bool                    init();

  // Set the verbose flag. If true it will print progress notifications 
  // to stdout. Default is false.
  void                    setVerbose ( bool verbose );

  // Translate the Jupiter database to Performer.
  bool                    translate ( const char *filename, pfGroup &root );

protected:

  // Class to contain a Performer group and all associated with it.
  class Group
  {
  public:
    Group() : _group ( NULL ) { _material.setValid ( 0 ); }
    Group ( pfGroup *group, const SlMaterialf &material ) : _group ( group ), _material ( material ){}
    Group ( const Group &group ) : _group ( group._group ), _material ( group._material ){}
    Group &operator = ( const Group &group ) { _group = group._group; _material = group._material; return *this; }
    pfGroup *getGroup() { return _group; }
    const SlMaterialf &getMaterial() const { return _material; }
    void setGroup ( pfGroup *group ) { _group = group; }
    void setMaterial ( const SlMaterialf &material ) { _material.setValue ( material ); }
  protected:
    pfGroup *_group;
    SlMaterialf _material;
  };

  typedef std::list<Group> Assemblies;
  typedef std::map<DbJtTraverser::EntityHandle, pfGroup *> GroupMap;
  bool _verbose;
  DbJtTraverser::Ptr _jtTraverser;
  mutable std::string _error;
  std::auto_ptr<GroupMap> _groupMap;
  Assemblies _assemblies;

  bool                    _addInstance   ( DbJtTraverser::EntityHandle entity );
  bool                    _addLOD        ( DbJtTraverser::EntityHandle entity, const unsigned int &whichLOD, Group &part );
  bool                    _addLODs       ( DbJtTraverser::EntityHandle entity, Group &part );
  bool                    _addName       ( DbJtTraverser::EntityHandle entity, pfNode &node );
  bool                    _addMaterial   ( DbJtTraverser::EntityHandle entity, Group &group );
  bool                    _addMaterial   ( DbJtTraverser::EntityHandle entity, const unsigned int &whichLOD, const unsigned int &whichShape, const Group &part, pfGeoState &state );
  bool                    _addPart       ( DbJtTraverser::EntityHandle entity );
  bool                    _addSet        ( DbJtTraverser::EntityHandle entity, const unsigned int &whichLOD, const unsigned int &whichShape, const unsigned int &whichSet, pfGeoSet &gset );
  bool                    _addShape      ( DbJtTraverser::EntityHandle entity, const unsigned int &whichLOD, const unsigned int &whichShape, const Group &part, pfGeode &geode );
  bool                    _addTransform  ( DbJtTraverser::EntityHandle entity, pfDCS &group );
  bool                    _assemblyStart ( DbJtTraverser::EntityHandle entity );

  bool                    _createGroup ( DbJtTraverser::EntityHandle entity, TrJt2Pf::Group &group );

  bool                    _endCurrentGroup();

  pfGroup *               _findGroup ( DbJtTraverser::EntityHandle entity ) const;

  int *                   _makeIntArray ( const std::vector<unsigned int> &vec ) const;
  pfVec2 *                _makeVec2Array ( const std::vector<SlVec2f> &vec ) const;
  pfVec3 *                _makeVec3Array ( const std::vector<SlVec3f> &vec ) const;
  pfVec4 *                _makeVec4Array ( const std::vector<SlVec3f> &vec ) const;

  bool                    _processEntity ( DbJtTraverser::EntityHandle entity );

  bool                    _setMaterial ( const SlMaterialf &material, pfGeoState &state ) const;

  static bool             _traverseCallback ( const DbJtTraverser::Message &message, const DbJtTraverser &traverser, const void *clientData );
  bool                    _traverseNotify   ( const DbJtTraverser::Message &message );
};


// So class SlRefPtr works with class pfMemory. 
TR_JT_2_PF_API void _incrementPointerReferenceCount ( pfMemory *p );
TR_JT_2_PF_API void _decrementPointerReferenceCount ( pfMemory *p );


}; // namespace CadKit

#endif // _CADKIT_JUPITER_TO_PERFORMER_TRANSLATER_H_
