
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The camera path class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PATH_ANIMATION_CAMERA_PATH_H_
#define _PATH_ANIMATION_CAMERA_PATH_H_

#include "PathAnimation/CompileGuard.h"

#include "Serialize/XML/Macros.h"

#include "Usul/Documents/Document.h"
#include "Usul/Math/Vector3.h"

#include <vector>


class CameraPath : public Usul::Documents::Document
{
public:

  // Smart pointers.
  USUL_DECLARE_QUERY_POINTERS ( CameraPath );

  // Typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Vector3 < Vec3d > Triplet;
  typedef std::vector < Triplet > Values;

  // Constructor.
  CameraPath();

  // Append the path.
  void                          append  ( const Usul::Math::Vec3d &eye, const Usul::Math::Vec3d &center, const Usul::Math::Vec3d &up );

  /// Get the camera info.
  void                          camera ( Usul::Math::Vec3d &eye, Usul::Math::Vec3d &center, Usul::Math::Vec3d &up, unsigned int num ) const;

  // Can we close the path?
  bool                          canClose() const;

  // Can we play the animation?
  bool                          canPlay() const;

  // Return true if this document can do it.
  virtual bool                  canExport ( const std::string &ext ) const;
  virtual bool                  canInsert ( const std::string &ext ) const;
  virtual bool                  canOpen   ( const std::string &ext ) const;
  virtual bool                  canSave   ( const std::string &ext ) const;

  // Clear the document.
  virtual void                  clear ( IUnknown *caller = 0x0 );

  // Close the path (if possible).
  void                          closePath();

  // Get the filters that correspond to what this document can do.
  virtual Filters               filtersExport() const;
  virtual Filters               filtersInsert() const;
  virtual Filters               filtersOpen()   const;
  virtual Filters               filtersSave()   const;

  // Insert the new camera between the closest camera and it's closest neighbor.
  void                          insertBetweenNearest  ( const Usul::Math::Vec3d &eye, const Usul::Math::Vec3d &center, const Usul::Math::Vec3d &up );

  // Prepend the path.
  void                          prepend ( const Usul::Math::Vec3d &eye, const Usul::Math::Vec3d &center, const Usul::Math::Vec3d &up );

  // Read.
  virtual void                  read ( const std::string &file, IUnknown *caller = 0x0, IUnknown *progress = 0x0 );

  // Remove the camera closest to the given eye position.
  void                          removeNearest ( const Usul::Math::Vec3d &eye, const Usul::Math::Vec3d &center, const Usul::Math::Vec3d &up );

  // Replace the nearest camera.
  void                          replaceNearest ( const Usul::Math::Vec3d &eye, const Usul::Math::Vec3d &center, const Usul::Math::Vec3d &up );

  // Get number of cameras.
  unsigned int                  size() const;

  // Get the values.
  void                          values ( Values &, bool reverseOrder ) const;

  // Write.
  virtual void                  write ( const std::string &file, IUnknown *caller = 0x0, IUnknown *progress = 0x0  ) const;

  // Predicate functor for testing equal values.
  struct EqualValue
  {
    bool operator () ( const Triplet &a, const Triplet &b ) const
    {
      return ( a[0].equal ( b[0] ) && 
               a[1].equal ( b[1] ) && 
               a[2].equal ( b[2] ) );
    }
  };

protected:

  // Use reference counting.
  virtual ~CameraPath();

  unsigned int                  _closest ( const Usul::Math::Vec3d &eye ) const;

private:

  Values _values;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( CameraPath );
};


#endif // _PATH_ANIMATION_CAMERA_PATH_H_
