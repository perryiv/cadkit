
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ANIMATE_MOVIE_H__
#define __ANIMATE_MOVIE_H__

#include "Animate/Frame.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "GN/Config/UsulConfig.h"
#include "GN/Splines/Curve.h"
#include "GN/Evaluate/Point.h"
#include "GN/Write/XML.h"

#include "Usul/Documents/Document.h"

#include <list>
#include <iosfwd>

namespace osg { class Node; }

namespace Animate {


class Movie : public Usul::Documents::Document
{
public:

  /// Typedefs.
  typedef Usul::Documents::Document    BaseClass;
  typedef Animate::Frame               Frame;
  typedef std::vector < Frame >        Frames;
  typedef Frames::iterator             iterator;
  typedef Frames::const_iterator       const_iterator;
  typedef Frames::size_type            size_type;
  typedef std::vector < Frame >        Sequence;
  typedef GN::Config::UsulConfig < double, double, unsigned int > DoubleConfig;
  typedef GN::Splines::Curve < DoubleConfig >  DoubleCurve;
  typedef DoubleCurve::IndependentSequence     IndependentSequence;
  typedef DoubleCurve::DependentContainer      DependentContainer;
  typedef DoubleCurve::DependentSequence       DependentSequence;
  typedef DoubleCurve::IndependentType         Parameter;
  typedef Usul::Interfaces::IUnknown           IUnknown;
  typedef std::string                          Filename;
  typedef std::vector< Filename >              Filenames;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Movie );

  enum Mode
  {
    IDLE,
    PLAYING_FORWARDS,
    PLAYING_REVERSE,
    STOPPED,
    PAUSED
  };



  Movie (  );

  //Get/Set the current frame
  void                        currentFrame( const Frame& );
  const Frame&                currentFrame() const;
  Frame&                      currentFrame();

  void                        setCurrentFrame ( unsigned int i );
  unsigned int                getCurrentFrame () const { return _currentFrame; }

  unsigned int                getPercentageDone () const;

  //Function for changing the current frame
  void                        firstFrame();
  void                        previousFrame();
  void                        nextFrame();
  void                        lastFrame();

  //Get frame at given index
  const Frame&                frame ( unsigned int ) const;
  Frame&                      frame ( unsigned int );

  //Return the number of frames
  unsigned int                getNumberFrames () const { return _frames.size(); }

  //Add frames
  void                        prependFrame ( const Frame & );
  void                        appendFrame  ( const Frame & );

  //Remove frames
  void                        removeFrame ( unsigned int );

  //Play/Stop movie
  void                        playForward( Usul::Interfaces::IUnknown* caller );
  void                        playReverse( Usul::Interfaces::IUnknown* caller );
  void                        stop();
  void                        pause();
  void                        unpause();

  bool                        playingForwards () const { return _mode == PLAYING_FORWARDS; }
  bool                        playingReverse  () const { return _mode == PLAYING_REVERSE; }
  bool                        stopped ()         const { return _mode == STOPPED; }
  bool                        paused  ()         const { return _mode == PAUSED;  }

  bool                        isPlaying()        const { return playingForwards() || playingReverse(); }

  bool                        canPlay() const { return !( _frames.size() < 2 ); }
  bool                        empty()   const { return _frames.empty(); }

  //Iterators to access internal frames
  iterator                    end()       { return _frames.end(); }
  const_iterator              end() const { return _frames.end(); }

  iterator                    begin()       { return _frames.begin(); }
  const_iterator              begin() const { return _frames.begin(); }

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &file ) const;
  virtual bool                canInsert ( const std::string &file ) const;
  virtual bool                canOpen   ( const std::string &file ) const;
  virtual bool                canSave   ( const std::string &file ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  /// Create default GUI
  virtual void                createDefaultGUI ( Unknown *caller = 0x0 );

  /// Get the filters that correspond to what this document can do.
  virtual Filters             filtersExport() const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;

  /// Read the file and add it to existing document's data.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0 );

  /// Write the document to given file name. Does not rename this document.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0  ) const;  

  /// Write movie to external format ( i.e. avi ).  Not sure if these are the best functions for this.
  virtual Filters             filtersWrite() const;
  virtual void                writeMovie ( const Filename& filename, const Filenames& filenames );

  // Set/Get the last U value
  Frame                         lastU ( Parameter u );
  const Parameter &             lastU() const { return _lastU; }

  // Set/get the segment duration.
  const Parameter &             segmentDuration() const { return _segTime; }
  Parameter &                   segmentDuration()       { return _segTime; }
  void                          segmentDuration( Parameter segTime ) { _segTime = segTime; }

  void                          loop ( bool b ) { _loop = b; }
  bool                          loop () const   { return _loop; }

  osg::Node*                    buildAnimationPath();

protected:

  osg::Vec3                     _getEyePostion ( const osg::Vec3& center, float distance, const osg::Quat& rot ) const;

  virtual void                  _forward ( IUnknown* caller );
  virtual void                  _reverse ( IUnknown* caller );
  template < class Iter > void  _interpolate ( Iter begin, Iter end, DoubleCurve &curve, DependentContainer &rotations, IndependentSequence &params );
  template < class Iter > void  _play ( Iter begin, Iter end, IUnknown* caller );
  void                          _animatePath ( const DoubleCurve &curve, const DependentContainer &rotations, const IndependentSequence &param, const Parameter&, const Parameter&, IUnknown* caller );
  void                          _changeMode ( Mode );


  // Do not copy.
  Movie ( const Movie & );
  Movie &operator = ( const Movie & );

  /// Use reference counting.
  virtual ~Movie();

private:

  class ParserHelper
  {
  public:
    ParserHelper( Frames &frames );
    void startFrame     ( const std::string &s );
    void setDistance    ( const std::string &s );
    void startCenter    ( const std::string &s );
    void getCenterX     ( const std::string &s );
    void getCenterY     ( const std::string &s );
    void getCenterZ     ( const std::string &s );
    void finishCenter   ( const std::string &s );
    void startRotation  ( const std::string &s );
    void getRotationX   ( const std::string &s );
    void getRotationY   ( const std::string &s );
    void getRotationZ   ( const std::string &s );
    void getRotationW   ( const std::string &s );
    void finishRotation ( const std::string &s );
    void getName        ( const std::string &s );
    void getComment     ( const std::string &s );
    void finishFrame    ( const std::string &s );
  private:
    Frame *_frame;
    Frames &_frames;
    osg::Vec3 _center;
    osg::Quat _rotation;
  };

  Frames           _frames;
  unsigned int     _currentFrame;
  Parameter        _segTime;
  Parameter        _lastU;
  Mode             _mode;
  Mode             _lastMode;
  bool             _loop;


};


};// namespace Animate


#endif //__ANIMATE_MOVIE_H__

