
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PLAY_MOVIE_H_
#define _PLAY_MOVIE_H_

#include "Minerva/Plugins/PlayMovie/CompileGuard.h"

#include "osg/ref_ptr"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"

#include "Cinema/Core/Movie.h"

namespace osg{ class Node; }

class PlayMovieComponent : public Usul::Base::Referenced,
                           public Usul::Interfaces::IPlugin
{
public:

  /// Typedefs.  
  typedef Usul::Base::Referenced                                   BaseClass;
  typedef Usul::Interfaces::IUnknown                               Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PlayMovieComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor
  PlayMovieComponent();
  
  virtual bool    execute ( Unknown* caller, bool left, bool middle, bool right, bool motion, float x, float y, float z );

  virtual bool    temporary();

  virtual void    play();

  virtual void    pause();

  virtual void    restart();

  virtual bool    isPlaying();

  virtual bool    isPaused();

protected:
  
  /// Play a movie given position, width, height vector and filename. 
  osg::Node* playMovie( const osg::Vec3f&, const osg::Vec3f&, const osg::Vec3f& , const std::string& );  

  /// Return name of plugin.
  virtual std::string           getPluginName() const;

  // Do not copy.
  PlayMovieComponent             ( const PlayMovieComponent & );
  PlayMovieComponent &operator = ( const PlayMovieComponent & );

  // As of now thiw will parse the geometry point and convert it into osg::Vec3. 
  osg::Vec3f convertStringToPosition( const std::string& ); 

  /// Use reference counting.
  virtual ~PlayMovieComponent();

protected:
  osg::ref_ptr< Cinema::Core::Movie >   _movie;
};


#endif // _PLAY_MOVIE_H_
