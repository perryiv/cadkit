
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Model for showtime.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SHOW_TIME_MODEL_MODEL_H_
#define _SHOW_TIME_MODEL_MODEL_H_

#include "ShowtimeModel/CompileGuard.h"

#include "Usul/Base/Object.h"
#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IUnknown.h"

#include "osg/Group"
#include "osg/ref_ptr"

#include <map>
#include <string>

namespace XmlTree { class Node; }


namespace Showtime {


class Model : public Usul::Base::Object
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef osg::ref_ptr<osg::Group> GroupPtr;
  typedef Usul::Documents::Document Document;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::map<std::string,std::string> Attributes;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Model );

  // Construction.
  Model ( const std::string &file, const XmlTree::Node *node );

  // Hide the model.
  void                        hide();

  // Load the file now if needed.
  void                        load ( Usul::Interfaces::IUnknown *progress = 0x0 );

  // Get the scene root.
  GroupPtr                    root();

  // Show the model.
  void                        show();

  // Unload the model.
  void                        unload();

protected:

  // Use reference counting.
  virtual ~Model();

private:

  // Do not copy.
  Model ( const Model & );
  Model &operator = ( const Model & );

  void                        _destroy();

private:

  std::string _file;
  GroupPtr _root;
  GroupPtr _model;
  Document::RefPtr _document;
  Attributes _attributes;
};


} // namespace Showtime


#endif // _SHOW_TIME_MODEL_MODEL_H_
