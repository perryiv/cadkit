
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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
#endif

class pfDCS;
class pfGroup;
class pfNode;
class pfObject;
class pfGeoSet;
class pfGeoState;


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

  // Translate the Jupiter database to Performer.
  bool                    translate ( const char *filename, pfGroup &root );

protected:

  // Class to contain a group and all associated with it.
  class Group
  {
  public:
    Group() : _group ( NULL ) { _material.setValid ( 0 ); }
    Group ( pfGroup *group, const SlMaterialf &material ) : _group ( group ), _material ( material ){}
    Group ( const Group &group ) : _group ( group._group ), _material ( group._material ){}
    Group &operator = ( const Group &group ) { _group = group._group; _material = group._material; return *this; }
    pfGroup *getGroup() { return _group; }
    SlMaterialf &getMaterial() { return _material; }
    void setGroup ( pfGroup *group ) { _group = group; }
    void setMaterial ( const SlMaterialf &material ) { _material = material; }
  protected:
    pfGroup *_group;
    SlMaterialf _material;
  };

  typedef std::list<Group> GroupStack;
  DbJtTraverser::Ptr _jtTraverser;
  std::string _error;
  GroupStack _groupStack;

  //bool                    _addInstance   ( DbJtTraverser::EntityHandle entity );
  bool                    _addLOD        ( DbJtTraverser::EntityHandle entity, const unsigned int &whichLOD, Group &part );
  bool                    _addLODs       ( DbJtTraverser::EntityHandle entity, Group &part );
  bool                    _addName       ( DbJtTraverser::EntityHandle entity, pfNode &node );
  bool                    _addMaterial   ( DbJtTraverser::EntityHandle entity, Group &group );
  bool                    _addMaterial   ( const Group &part, pfGeoState &state );
  bool                    _addPart       ( DbJtTraverser::EntityHandle entity );
  bool                    _addSet        ( DbJtTraverser::EntityHandle entity, const unsigned int &whichLOD, const unsigned int &whichShape, const unsigned int &whichSet, pfGeoSet &gset );
  bool                    _addShape      ( DbJtTraverser::EntityHandle entity, const unsigned int &whichLOD, const unsigned int &whichShape, const Group &part, pfGroup &group );
  bool                    _addTransform  ( DbJtTraverser::EntityHandle entity, pfDCS &group );
  bool                    _assemblyStart ( DbJtTraverser::EntityHandle entity );

  bool                    _createGroup ( DbJtTraverser::EntityHandle entity, TrJt2Pf::Group &group );

  bool                    _endCurrentGroup();

  bool                    _processEntity ( DbJtTraverser::EntityHandle entity );

  void                    _setArrayString ( const std::vector<float> &array, SlAString &s );

  static bool             _traverseCallback ( const DbJtTraverser::Message &message, const DbJtTraverser &traverser, const void *clientData );
  bool                    _traverseNotify   ( const DbJtTraverser::Message &message );
};


// So class SlRefPtr works with class pfNodes. 
void _incrementPointerReferenceCount ( pfObject *p );
void _decrementPointerReferenceCount ( pfObject *p );


}; // namespace CadKit

#endif // _CADKIT_JUPITER_TO_PERFORMER_TRANSLATER_H_
