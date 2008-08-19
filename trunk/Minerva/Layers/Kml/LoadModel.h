
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to create an osg::Node from a filename.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_LOAD_MODEL_H__
#define __MINERVA_LAYERS_KML_LOAD_MODEL_H__

namespace osg { class Node; }
namespace Minerva { namespace Core { namespace Data { class ModelCache; } } }

#include <string>

namespace Minerva {
namespace Layers {
namespace Kml {

class LoadModel
{
public:
  typedef Minerva::Core::Data::ModelCache            ModelCache;
  
  LoadModel();

  osg::Node* operator() ( const std::string& filename, ModelCache *cache );

  void       toMeters ( double amount );
  double     toMeters() const;

private:

  void _preProcessCollada ( const std::string& filename );

  double _toMeters;
};

}
}
}

#endif // __MINERVA_LAYERS_KML_LOAD_MODEL_H__
