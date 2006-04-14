
#ifndef __QUICK_TIME_HELPER_H__
#define __QUICK_TIME_HELPER_H__

// Quick Time headers.
#include "QTML.h"
#include "Movies.h"

#include <string>

class QuickTimeHelper
{
public:

  QuickTimeHelper();
  ~QuickTimeHelper();

  void initialize();

  void checkError ( OSErr error );

  void createMovie ( const std::string& filename );

  void createTrackAndMedia ( unsigned int width, unsigned int height );

  void insertTrackIntoMedia ();

  void initImageAddition();

  void addImage ( unsigned char *data, unsigned int size );

  struct ScopedMediaEdits
  {
    ScopedMediaEdits ( QuickTimeHelper& qt );
    ~ScopedMediaEdits ();

    void init();
  private:
    ScopedMediaEdits(); // Do not use.

    QuickTimeHelper &_qt;
  };

private:
  Movie _movie;
  short _resRef;

  Track _track;
  Media _media;
};

#endif
