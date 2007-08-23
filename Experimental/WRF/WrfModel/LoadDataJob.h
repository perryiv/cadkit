
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job to load data from file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WRF_LOAD_DATA_JOB_H__
#define __WRF_LOAD_DATA_JOB_H__

#include "Experimental/WRF/WrfModel/Parser.h"
#include "Experimental/WRF/WrfModel/Channel.h"

#include "Usul/Jobs/Job.h"

#include <map>

class WRFDocument;

namespace osg { class Image; }

class LoadDataJob : public Usul::Jobs::Job
{
public:
  typedef Usul::Jobs::Job                             BaseClass;
  typedef std::pair < unsigned int, Channel::RefPtr > ReadRequest;
  typedef std::list < ReadRequest >                   ReadRequests;

  USUL_DECLARE_REF_POINTERS ( LoadDataJob );

  LoadDataJob ( const ReadRequests& requests, WRFDocument* document, const Parser& parser );

  void setSize ( unsigned int x, unsigned int y, unsigned int z );
protected:
  virtual ~LoadDataJob ();

  virtual void _started ();
  virtual void _finished ();
  osg::Image*  _createImage ( const ReadRequest& request, Parser::Data& data );

  ReadRequests _requests;
  WRFDocument* _document;
  Parser _parser;
  unsigned int _x;
  unsigned int _y;
  unsigned int _z;
};


#endif // __WRF_LOAD_DATA_JOB_H__
