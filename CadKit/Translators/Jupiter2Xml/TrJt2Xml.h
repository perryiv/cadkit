
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
//  TrJt2Xml: The Jupiter to XML translator.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_JUPITER_TO_XML_TRANSLATER_H_
#define _CADKIT_JUPITER_TO_XML_TRANSLATER_H_

#include "TrJt2XmlApi.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Database/Jupiter/DbJtTraverser.h"
# include "Standard/SlVec4.h"
# include <list>
# include <string>
# include <vector>
#endif


namespace CadKit
{
class DbXmlGroup;

class TR_JT_2_XML_API TrJt2Xml
{
public:

  TrJt2Xml();
  ~TrJt2Xml();

  // Get the last error.
  const std::string &     getLastError() const { return _error; }

  // Initialize. Call this before doing anything else.
  bool                    init();

  // Set the verbose flag. If true it will print progress notifications 
  // to stdout. Default is false.
  void                    setVerbose ( bool verbose );

  // Translate the Jupiter database to XML.
  bool                    translate ( const char *filename, DbXmlGroup &root );

protected:

  bool _verbose;
  DbJtTraverser::Ptr _jtTraverser;
  std::string _error;
  typedef std::list<DbXmlGroup *> XmlGroupStack;
  XmlGroupStack _groupStack;

  bool                    _addArray      ( const unsigned int &valid, const unsigned int &which, const std::vector<float> &array, const char *arrayName, DbXmlGroup &set );
  bool                    _addColor      ( const unsigned int &valid, const unsigned int &which, const SlVec4f &color, const char *colorName, DbXmlGroup &material );
  bool                    _addInstance   ( DbJtTraverser::EntityHandle entity );
  bool                    _addLOD        ( DbJtTraverser::EntityHandle entity, const unsigned int &whichLOD, DbXmlGroup &part );
  bool                    _addLODs       ( DbJtTraverser::EntityHandle entity, DbXmlGroup &part );
  bool                    _addName       ( DbJtTraverser::EntityHandle entity, DbXmlGroup &group );
  bool                    _addMaterial   ( DbJtTraverser::EntityHandle entity, DbXmlGroup &group );
  bool                    _addMaterial   ( const SlMaterialf &material, DbXmlGroup &group );
  bool                    _addPart       ( DbJtTraverser::EntityHandle entity );
  bool                    _addSet        ( DbJtTraverser::EntityHandle entity, const unsigned int &whichLOD, const unsigned int &whichShape, const unsigned int &whichSet, const std::string &name, DbXmlGroup &shape );
  bool                    _addShape      ( DbJtTraverser::EntityHandle entity, const unsigned int &whichLOD, const unsigned int &whichShape, DbXmlGroup &lod );
  bool                    _addTransform  ( DbJtTraverser::EntityHandle entity, DbXmlGroup &group );
  bool                    _assemblyStart ( DbJtTraverser::EntityHandle entity );

  DbXmlGroup *            _createGroup ( const char *groupName, DbJtTraverser::EntityHandle entity );

  bool                    _endCurrentGroup();

  void                    _printGroup ( DbXmlGroup &group );
  bool                    _processEntity ( DbJtTraverser::EntityHandle entity );

  void                    _setArrayString ( const std::vector<float> &array, SlAString &s );

  static bool             _traverseCallback ( const DbJtTraverser::Message &message, const DbJtTraverser &traverser, const void *clientData );
  bool                    _traverseNotify   ( const DbJtTraverser::Message &message );
};

}; // namespace CadKit

#endif // _CADKIT_JUPITER_TO_XML_TRANSLATER_H_
