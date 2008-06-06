
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to stitch images together.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_GIL_ACTION_STITCHER_CLASS_H_
#define _IMAGES_GIL_ACTION_STITCHER_CLASS_H_

#ifdef _MSC_VER
#define NOMINMAX
#endif

#include "Images/GIL/Exceptions/Exceptions.h"
#include "Images/GIL/IO/Base.h"

#include "Usul/Exceptions/Thrower.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Math/MinMax.h"
#include "Usul/MPL/SameType.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Types/Types.h"

#include <boost/gil/extension/dynamic_image/algorithm.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

#include <boost/shared_ptr.hpp>

#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define NULL_IMAGE ImagePtr ( static_cast<Image *> ( 0x0 ) )
#define OUTPUT_STREAM if ( 0x0 != out ) (*out)
#define OUTPUT_STREAM_IF(exp) if ( 0x0 != out && true == (exp) ) (*out)

namespace Images {
namespace GIL {
namespace Actions {


template < class Policy > class Stitcher
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Important typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef Stitcher<Policy> ThisType;
  typedef typename Policy::ImageType Image;
  typedef typename Image::point_t Dimensions;
  typedef boost::shared_ptr<Image> ImagePtr;
  typedef GIL::IO::Base<Image> IOBase;
  typedef typename IOBase::RefPtr IOPtr;
  typedef std::map<std::string,IOPtr> IOMap;
  typedef std::vector<std::string> Strings;
  typedef std::vector<Strings> FileNames;
  typedef std::vector<std::string> CommandLine;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Stitcher() :
    _target  ( NULL_IMAGE ),
    _ioMap   (),
    _input   (),
    _quality ( 100 ),
    _output  (),
    _ignored (),
    _dim     ( 0, 0 )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ~Stitcher()
  {
    _target = NULL_IMAGE;
    _ioMap.clear();
    _input.clear();
    _output.clear();
    _ignored.clear();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Add IO module.
  //
  /////////////////////////////////////////////////////////////////////////////

  void add ( const std::string &ext, IOBase *module )
  {
    _ioMap[ext] = IOPtr ( module );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return command-line arguments.
  //
  /////////////////////////////////////////////////////////////////////////////

  static CommandLine arguments()
  {
    CommandLine args;
    args.push_back ( "--input <tile 1> <tile 2> [tile 3] ... [tile n]" );
    args.push_back ( "--output <output>" );
    args.push_back ( "--quality <[1,100]> (jpeg only, default is 100)" );
    args.push_back ( "--columns <number of columns in the tiles> (default is number of input tiles)" );
    args.push_back ( "--dimensions <width and height of output image> (default is sum of all input tiles)" );
    return args;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return command-line arguments.
  //
  /////////////////////////////////////////////////////////////////////////////

  static std::string arguments ( const std::string &delimiter )
  {
    std::ostringstream s;

    const CommandLine args ( ThisType::arguments() );
    for ( CommandLine::const_iterator i = args.begin(); i != args.end(); ++i )
    {
      s << delimiter << (*i);
    }

    return s.str();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Check dimensions for overflow potential.
  //
  /////////////////////////////////////////////////////////////////////////////

  void checkOverflow ( const Dimensions &dim, std::ostream *out = &std::cout ) const
  {
    typedef typename Dimensions::value_type DimValueType;
    typedef Usul::Types::Uint64 Uint64;

    const Uint64 max16u ( std::numeric_limits<Usul::Types::Uint16>::max() );
    const Uint64 max16  ( std::numeric_limits<Usul::Types::Int16 >::max() );
    const Uint64 max32u ( std::numeric_limits<Usul::Types::Uint32>::max() );
    const Uint64 max32  ( std::numeric_limits<Usul::Types::Int32 >::max() );

    const Uint64 w ( dim.x );
    const Uint64 h ( dim.y );

    const bool overflow16u ( w > max16u || h > max16u );
    const bool overflow16  ( w > max16  || h > max16  );
    const bool overflow32u ( w > max32u || h > max32u );
    const bool overflow32  ( w > max32  || h > max32  );

    OUTPUT_STREAM_IF ( overflow16  ) << "Warning 2178537368: Dimensions exceed max 16-bit signed integer"   << std::endl;
    OUTPUT_STREAM_IF ( overflow16u ) << "Warning 1124026021: Dimensions exceed max 16-bit unsigned integer" << std::endl;
    OUTPUT_STREAM_IF ( overflow32  ) << "Warning 2213525290: Dimensions exceed max 32-bit signed integer"   << std::endl;
    OUTPUT_STREAM_IF ( overflow32u ) << "Warning 7837455660: Dimensions exceed max 32-bit unsigned integer" << std::endl;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the number of columns.
  //
  /////////////////////////////////////////////////////////////////////////////

  unsigned int columns() const
  {
    return static_cast<unsigned int> ( ( _input.empty() ) ? 0 : _input.front().size() );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the number of rows.
  //
  /////////////////////////////////////////////////////////////////////////////

  unsigned int rows() const
  {
    return static_cast<unsigned int> ( _input.size() );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the target dimensions.
  //
  /////////////////////////////////////////////////////////////////////////////

  Dimensions dimensions() const
  {
    return _dim;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the target dimensions.
  //
  /////////////////////////////////////////////////////////////////////////////

  void dimensions ( const Dimensions &dim )
  {
    _dim.x = Usul::Math::absolute ( dim.x );
    _dim.y = Usul::Math::absolute ( dim.y );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the ignored command-line arguments.
  //
  /////////////////////////////////////////////////////////////////////////////

  Strings ignored() const
  {
    return _ignored;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the output file name.
  //
  /////////////////////////////////////////////////////////////////////////////

  std::string output() const
  {
    return _output;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the output file name.
  //
  /////////////////////////////////////////////////////////////////////////////

  void output ( const std::string &file )
  {
    _output = file;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the input file names.
  //
  /////////////////////////////////////////////////////////////////////////////

  FileNames input() const
  {
    return _input;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the input file names.
  //
  /////////////////////////////////////////////////////////////////////////////

  void input ( const FileNames &input )
  {
    // Initialize the number of columns.
    unsigned int numColumns ( static_cast<unsigned int> ( ( input.empty() ) ? 0 : input.front().size() ) );

    // Check input for consistant size.
    for ( FileNames::const_iterator i = input.begin(); i != input.end(); ++i )
    {
      const Strings &row ( *i );
      if ( row.size() != numColumns )
      {
        throw std::runtime_error ( "Error 3232399187: Inconsistant number of columns in input file matrix" );
      }
    }

    // Set input matrix.
    _input = input;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return maximum dimensions of all input files.
  //
  /////////////////////////////////////////////////////////////////////////////

  Dimensions maxInputDimensions() const
  {
    USUL_ASSERT_SAME_TYPE ( Dimensions, boost::gil::point2<std::ptrdiff_t> );

    // Initialize the dimensions.
    Dimensions maxDim ( 0, 0 );

    // Loop through the files.
    for ( FileNames::const_iterator i = _input.begin(); i != _input.end(); ++i )
    {
      const Strings &row ( *i );
      for ( Strings::const_iterator j = row.begin(); j != row.end(); ++j )
      {
        const std::string file ( *j );

        // Read the dimensions.
        IOPtr reader ( this->_getIO ( file ) );
        Dimensions dim ( reader->dimensions ( file ) );

        // Update the max.
        maxDim.x = Usul::Math::maximum ( dim.x, maxDim.x );
        maxDim.y = Usul::Math::maximum ( dim.y, maxDim.y );
      }
    }

    // Return the maximum dimensions of any input file.
    return maxDim;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return target dimensions.
  //
  /////////////////////////////////////////////////////////////////////////////

  Dimensions targetDimensions ( Dimensions &maxInputDim ) const
  {
    USUL_ASSERT_SAME_TYPE ( Dimensions, boost::gil::point2<std::ptrdiff_t> );

    if ( 0 != _dim.x && 0 != _dim.y )
    {
      return _dim;
    }
    else
    {
      const FileNames::size_type rows ( _input.size() );
      const Strings::size_type columns ( ( rows > 0 ) ? _input.front().size() : 0 );

      Dimensions dim;
      dim.x = maxInputDim.x * static_cast<Dimensions::value_type> ( columns );
      dim.y = maxInputDim.y * static_cast<Dimensions::value_type> ( rows );
      return dim;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Parse the command line.
  //
  /////////////////////////////////////////////////////////////////////////////

  void parse ( const CommandLine &args, std::ostream *out = &std::cout )
  {
    // Initialize.
    Strings files;
    files.reserve ( args.size() );
    unsigned int columns ( 0 );
    unsigned int quality ( _quality );
    std::string output;
    Dimensions dim ( 0, 0 );

    // Loop through the command-line arguments.
    {
      for ( CommandLine::const_iterator i = args.begin() + 1; i != args.end(); ++i )
      {
        // Get the current word.
        const std::string word ( *i );

        if ( "--input" == word )
        {
          // Grab all the input files.
          ++i;
          CommandLine::const_iterator j = i;
          while ( j != args.end() )
          {
            const std::string file ( *j );
            if ( '-' == file.at(0) )
            {
              break;
            }
            if ( false == Usul::Predicates::FileExists::test ( file ) )
            {
              throw Images::GIL::Exceptions::CommandLineException ( "Error 7789741330: Cannot read input file: " + file );
            }
            files.push_back ( file );
            ++j;
          }

          // Set next iterator for outer loop.
          i = j - 1;
        }

        else if ( "--output" == word )
        {
          ++i;
          output = ( ( args.end() != i ) ? *i : "" );
        }

        else if ( "--quality" == word )
        {
          ++i;
          if ( args.end() != i )
          {
            std::istringstream in ( *i );
            in >> quality;
          }
        }

        else if ( "--columns" == word )
        {
          ++i;
          if ( args.end() != i )
          {
            std::istringstream in ( *i );
            in >> columns;
          }
        }

        else if ( "--dimensions" == word )
        {
          ++i;
          if ( args.end() != i )
          {
            std::istringstream in ( *i );
            in >> dim.x;
          }
          ++i;
          if ( args.end() != i )
          {
            std::istringstream in ( *i );
            in >> dim.y;
          }
        }

        else
        {
          OUTPUT_STREAM << "Warning: Ignoring argument: " << word << std::endl;
          _ignored.push_back ( word );
        }
      }
    }

    // Make sure there is an output file.
    if ( output.empty() )
    {
      Usul::Exceptions::Thrower<Images::GIL::Exceptions::CommandLineException>
        ( "Error 1433726558: No output file specified" );
    }

    // Make sure there are enough input files.
    if ( files.size() < 1 )
    {
      Usul::Exceptions::Thrower<Images::GIL::Exceptions::CommandLineException>
        ( "Error 3468882899: Number of input files is ", files.size() );
    }

    // Set the number of columns.
    columns = static_cast<unsigned int> ( ( ( 0 == columns || columns > files.size() ) ? files.size() : columns ) );

    // Calculate the number of rows.
    const unsigned int rows ( static_cast<unsigned int> ( ( files.size() == columns ) ? 1 : ( files.size() / columns ) ) );

    // Make sure the number of rows makes sense.
    if ( files.size() != ( rows * columns ) )
    {
      Usul::Exceptions::Thrower<Images::GIL::Exceptions::CommandLineException>
        ( "Error 3760601841: Number of input files (", files.size(), ") is not evenly divisible by specified number of columns (", columns, ")" );
    }

    // Set input files.
    {
      FileNames matrix;
      matrix.reserve ( rows );
      Strings::const_iterator first = files.begin();
      for ( unsigned int i = 0; i < rows; ++i )
      {
        Strings::const_iterator last ( first + columns );
        matrix.push_back ( Strings ( first, last ) );
        first = last;
      }
      this->input ( matrix );
    }

    // Set the quality.
    this->quality ( quality );

    // Set the output file.
    this->output ( output );

    // Set the dimensions.
    this->dimensions ( dim );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Parse the command line.
  //
  /////////////////////////////////////////////////////////////////////////////

  void parse ( int argc, char **argv, std::ostream *out = &std::cout )
  {
    this->parse ( CommandLine ( argv, argv + argc ), out );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the quality.
  //
  /////////////////////////////////////////////////////////////////////////////

  void quality ( unsigned int q )
  {
    // Keep it in range.
    _quality = ( Usul::Math::maximum<unsigned int> ( 1, ( Usul::Math::minimum<unsigned int> ( 100, q ) ) ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Stitch the input tiles together.
  //
  /////////////////////////////////////////////////////////////////////////////

  void stitch ( std::ostream *out = &std::cout )
  {
    // Initialize the target image.
    _target = NULL_IMAGE;

    // Handle no input images.
    if ( _input.empty() || _input.front().empty() )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 2629708771: No input files specified" );
    }

    // Determine dimensions.
    Dimensions maxInputDim ( this->maxInputDimensions() );

    // Get the target image dimensions.
    Dimensions targetDim ( this->targetDimensions ( maxInputDim ) );

    // Calculate the size to make each input image before copying it.
    const bool useInput ( 0 != _dim.x && 0 != _dim.y );
    Dimensions tileDim ( ( ( useInput ) ? ( _dim.x / this->columns() ) : maxInputDim.x ),
                         ( ( useInput ) ? ( _dim.y / this->rows()    ) : maxInputDim.y ) );

    // Check for overflow.
    this->checkOverflow ( targetDim, out );

    // Allocate the target image.
    OUTPUT_STREAM << "Allocating output image with dimensions (" << targetDim.x << ',' << targetDim.y << ") ... " << std::flush;
    ImagePtr target ( new Image ( targetDim ) );
    std::cout << "Done" << std::endl;

    // Initialize the corner of the view.
    Dimensions topLeft ( 0, 0 );

    // Loop through the rows.
    for ( unsigned int i = 0; i < _input.size(); ++i )
    {
      const Strings &row ( _input.at ( i ) );

      // Loop through the columns.
      for ( unsigned int j = 0; j < row.size(); ++j )
      {
        const std::string &file ( row.at ( j ) );
        OUTPUT_STREAM << "Reading tile [" << i << ',' << j << "] '" << file << "' ... " << std::flush;

        // Read the tile.
        ImagePtr tile ( new Image );
        IOPtr reader ( this->_getIO ( file ) );
        reader->read ( file, *tile );

        // Resize the image if we should.
        if ( tile->dimensions() != tileDim  )
        {
          std::cout << "Resizing ... " << std::flush;
          ImagePtr tempImage ( new Image ( tileDim ) );
          boost::gil::resize_view 
            ( boost::gil::const_view ( *tile ), 
              boost::gil::view ( *tempImage ), 
              boost::gil::bilinear_sampler() );
          tile = tempImage;
        }

        // Copy all the pixels from the input image to the current view.
        std::cout << "Copying pixels ... " << std::flush;
        boost::gil::copy_and_convert_pixels 
          ( boost::gil::const_view ( *tile ), 
            boost::gil::subimage_view ( boost::gil::view ( *target ), 
            topLeft, tile->dimensions() ) );
        std::cout << "Done" << std::endl;

        // Move the original of the next view.
        topLeft.x += tileDim.x;
      }

      // Move the original of the next view.
      topLeft.x = 0;
      topLeft.y += tileDim.y;
    }

    // If we get to here then set the target.
    _target = target;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Write the target image to disk.
  //
  /////////////////////////////////////////////////////////////////////////////

  void write ( std::ostream *out = &std::cout ) const
  {
    // If there's no target image then punt.
    if ( 0x0 == _target.get() )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 3492988052: Have to stitch input tiles before writing the target image" );
    }

    // Write the image.
    OUTPUT_STREAM << "Writing file: " << _output << " ... " << std::flush;
    IOPtr writer ( this->_getIO ( _output ) );
    writer->write ( _output, *_target, _quality );
    OUTPUT_STREAM << "Done" << std::endl;
  }

protected:


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the IO module for the given file extension.
  //
  /////////////////////////////////////////////////////////////////////////////

  IOPtr _getIO ( const std::string &file ) const
  {
    const std::string ext ( Usul::File::extension ( file ) );
    IOMap::const_iterator i ( _ioMap.find ( ext ) );
    if ( _ioMap.end() == i || 0x0 == i->second.get() )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 2482728091: No IO module specified for file extension '", ext, "'" );
    }
    return i->second;
  }


private:


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Data members.
  //
  /////////////////////////////////////////////////////////////////////////////

  ImagePtr _target;
  IOMap _ioMap;
  FileNames _input;
  unsigned int _quality;
  std::string _output;
  Strings _ignored;
  Dimensions _dim;
};


} // namespace Actions
} // namespace GIL
} // namespace Images


#undef NULL_IMAGE
#undef OUTPUT_STREAM


#endif // _IMAGES_GIL_ACTION_STITCHER_CLASS_H_
