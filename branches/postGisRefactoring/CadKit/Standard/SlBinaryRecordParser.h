
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlBinaryRecordParser.h: A class that parses binary records. It assumes 
//  that each record has a header section with a code (indicating the type of 
//  record) and, immediately following, the length of the record.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_BINARY_RECORD_PARSER_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_BINARY_RECORD_PARSER_CLASS_H_

#include "SlAssert.h"
#include "SlEndian.h"
#include "SlTypedefs.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <fstream>
#endif

#define SL_BRP_TA class RecordCode, class RecordLength
#define SL_BRP_TP RecordCode, RecordLength


namespace CadKit
{
template<SL_BRP_TA> class SlBinaryRecordParser
{
public:

  SlBinaryRecordParser();
  SlBinaryRecordParser ( const SlBinaryRecordParser<SL_BRP_TP> &parser ); // Don't use.

  void                        close();

  const RecordCode &          getRecordCode() const { return _recordCode; }
  const RecordLength &        getRecordLength() const { return _recordLength; }
  const unsigned int          getRecordHeaderLength() const { return ( sizeof ( RecordCode ) + sizeof ( RecordLength ) ); }

  bool                        open ( const char *filename );

  bool                        parse();

protected:

  virtual bool                _notify();

  void                        _parseRecordHeader();

  void                        _skip();

private:

  RecordCode _recordCode;
  RecordLength _recordLength;
  unsigned int _startOfRecord;
  std::ifstream _in;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_BRP_TA> inline SlBinaryRecordParser<SL_BRP_TP>::SlBinaryRecordParser() :
  _recordCode ( 0 ),
  _recordLength ( 0 ),
  _startOfRecord ( 0 )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_BRP_TA> inline SlBinaryRecordParser<SL_BRP_TP>::SlBinaryRecordParser ( const SlBinaryRecordParser<SL_BRP_TP> &parser )
{
  SL_ASSERT ( 0 ); // Don't use.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close the file.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_BRP_TA> inline void SlBinaryRecordParser<SL_BRP_TP>::close()
{
  _in.close();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the file.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_BRP_TA> inline bool SlBinaryRecordParser<SL_BRP_TP>::open ( const char *filename )
{
  SL_ASSERT ( filename );

  // Make sure there isn't a file already opened.
  if ( true == _in.is_open() )
    return false;

  // Open the input file.
  _in.open ( filename, std::ios::in | std::ios::binary );

  // Initialize.
  _recordCode = 0;
  _recordLength = 0;
  _startOfRecord = 0;

  // See if it opened.
  return ( true == _in.is_open() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Default notify function. Return false to stop parsing.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_BRP_TA> inline bool SlBinaryRecordParser<SL_BRP_TP>::_notify()
{
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the record header.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_BRP_TA> inline void SlBinaryRecordParser<SL_BRP_TP>::_parseRecordHeader()
{
  SL_ASSERT ( _in.is_open() );

  // Read the code and record size.
  _in.read ( (char *) &_recordCode, sizeof ( RecordCode ) );
  _in.read ( (char *) &_recordLength, sizeof ( RecordLength ) );

  // Swap the bytes if needed.
  if ( true == isLittleEndian() )
  {
    SL_VERIFY ( reverseBytes ( _recordCode ) );
    SL_VERIFY ( reverseBytes ( _recordLength ) );
  }

  // Sanity check.
  SL_ASSERT ( _recordLength > 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Skip to the end of the current record.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_BRP_TA> inline void SlBinaryRecordParser<SL_BRP_TP>::_skip()
{
  SL_ASSERT ( _in.is_open() );
  SL_ASSERT ( _recordLength >= this->getRecordHeaderLength() );

  // Put the cursor at the end of the record.
  _in.seekg ( _startOfRecord + (unsigned int) _recordLength, std::ios::beg );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the file.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_BRP_TA> inline bool SlBinaryRecordParser<SL_BRP_TP>::parse()
{
  SL_ASSERT ( _in.is_open() );

  // Loop until we reach the end of the file.
  while ( false == _in.eof() )
  {
    // Grab the current cursor's position.
    _startOfRecord = _in.tellg();

    // Read the record's code and length.
    this->_parseRecordHeader();

    // Notify the client.
    if ( false == this->_notify() )
    {
      // The client wants to stop.
      return false;
    }

    // Skip this record.
    this->_skip();
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Common types.
//
///////////////////////////////////////////////////////////////////////////////

typedef SlBinaryRecordParser<SlInt16,SlUint16> SlOpenFlightParser;


}; // End of namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_BINARY_RECORD_PARSER_CLASS_H_
