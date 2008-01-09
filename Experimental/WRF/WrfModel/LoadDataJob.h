
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
  typedef std::pair < unsigned int, unsigned int >   ReadRequest;

  USUL_DECLARE_REF_POINTERS ( LoadDataJob );

  LoadDataJob ( const ReadRequest& request, WRFDocument* document, const Parser& parser );

protected:
  virtual ~LoadDataJob ();

  virtual void _started ();

  ReadRequest _request;
  WRFDocument* _document;
  Parser _parser;
};


#endif // __WRF_LOAD_DATA_JOB_H__
