
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlManip: I/O manipulator class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_IO_MANIPULATOR_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_IO_MANIPULATOR_CLASS_H_


namespace CadKit {


/////////////////////////////////////////////////////////////////////////////
//
//  Write the value to the stream with the correct width.
//
/////////////////////////////////////////////////////////////////////////////

template < typename T > struct SlManip
{
  SlManip ( const T &value, unsigned int width ) : 
    _value ( value ), 
    _width ( width )
  {
    // Empty.
  }

  const T &value() const { return _value; }
  unsigned int width() const { return _width; }

protected:

  T _value;
  unsigned int _width;
};


/////////////////////////////////////////////////////////////////////////////
//
//  Write the floating-point value to the stream with the correct number 
//  of decimals and total width.
//
/////////////////////////////////////////////////////////////////////////////

template < typename T > struct SlFloatManip : public SlManip<T>
{
  SlFloatManip ( const T &value, unsigned int numDecimals, unsigned int width ) : 
    SlManip<T> ( value, width ),
    _numDecimals ( numDecimals )
  {
    // Empty.
  }

  unsigned int numDecimals() const { return _numDecimals; }

protected:

  unsigned int _numDecimals;
};


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_IO_MANIPULATOR_CLASS_H_
