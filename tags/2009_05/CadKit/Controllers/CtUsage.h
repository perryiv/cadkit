
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  CtUsage: The usage string for command-line execution.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_TRANSLATION_CONTROLLER_USAGE_STRING_H_
#define _CADKIT_TRANSLATION_CONTROLLER_USAGE_STRING_H_

#include <algorithm>


namespace CadKit {
namespace CtUsage {


///////////////////////////////////////////////////////////////////////////////
//
//  Return the long usage string.
//
///////////////////////////////////////////////////////////////////////////////

std::string getLong()
{
  return std::string ( 
  "\n\
  \n\
  --print-progress <priority>, -pp <priority>\n\
  \n\
      Print to stdout any progress messages with a priority level <= 'priority'.\n\
      The 'priority' needs to be >= 0.\n\
  \n\
  --print-warnings, -pw\n\
  \n\
      Print warning messages to stdout.\n\
  \n\
  --print-info, -pi\n\
  \n\
      Print general information messages to stdout.\n\
  \n\
  --verbose, -v\n\
  \n\
      Verbose output to stdout, same as '-pw -pi -pp 1'.\n\
  \n\
  --all-lods, -al\n\
  \n\
      Where applicable, translate all of the Levels-of-Detail (LODs)'.\n\
  \n\
  --high-lod, -hl\n\
  \n\
      Where applicable, translate only the highest Level-of-Detail (LOD)'.\n\
  \n\
  --low-lod, -ll\n\
  \n\
      Where applicable, translate only the lowest Level-of-Detail (LOD)'.\n\
  \n\
  --zero <low high>, -z\n\
  \n\
      Any number in the range [low,high] is set to 0.\n\
  \n\
  --output-binary, -ob\n\
  \n\
      If possible, the output should be in binary.\n\
  \n\
  --output-ascii, -oa\n\
  \n\
      If possible, the output should be in ASCII.\n"
  ) + std::string (
  "\n\
  --output-directory <directory name>, -od\n\
  \n\
      Specify the output directory. The default is the same directory\n\
      as the corresponding input file. Regardless, the output filename\n\
      will be the same as the input filename, but with a different extension.\n\
  \n\
  --num-decimals <decimals>, -nd\n\
  \n\
      For ASCII output formats, specify the number of decimals.\n\
  \n\
  --scale <x y z>, -s\n\
  \n\
      Scale the geometry. See note below.\n\
  \n\
  --rotate <x y z deg>, -r\n\
  \n\
      Rotate the geometry 'deg' degrees about the axis (x,y,z).\n\
      See note below.\n\
  \n\
  --output-units <units>, -ou\n\
  \n\
      Scale the geometry from its current system of units into the\n\
      specified units.\n\
  \n\
  --help, -h\n\
  \n\
      Print this extended help message.\n\
  \n\
  Note: Geometric transformations happen in the order in which the arguments\n\
        are parsed. Any arbitrary combination of transformations is permitted.\n\
" );

//  -ea <action>      What to do if an error is encountered. Possible actions:\n\
//                      exit:     Exit the program.\n\
//                      continue: Continue executing is possible.\n\
//  -wa <action>      What to do if a warning is encountered. Possible actions:\n\
//                      exit:     Exit the program.\n\
//                      continue: Continue executing is possible.\n\
//  --error-action    Same as '-ea'.\n\
//  --warning-action  Same as '-wa'.

}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the short usage string.
//
///////////////////////////////////////////////////////////////////////////////

std::string getShort()
{
  // Get the long string.
  const std::string usage ( getLong() );

  // Get the start of the string.
  std::string::const_iterator start = usage.begin(), stop;

  // Define the sub-strings we want to search for.
  const std::string dashes ( "--" );
  const std::string newline ( "\n" );

  // This is the string we will return.
  std::string answer ( "\n" );

  // Loop through the string.
  while ( start != usage.end() )
  {
    // Search for the sub-string.
    start = std::search ( start, usage.end(), dashes.begin(), dashes.end() );

    // If we are not at the end...
    if ( start != usage.end() )
    {
      // Now find the end of the line.
      stop = std::search ( start, usage.end(), newline.begin(), newline.end() );

      // Append this range to our string.
      answer += "  ";
      answer.append ( start, ++stop );

      // Update the starting position.
      start = stop;
    }
  }

  // Return the short string.
  return answer;
}


}; // namespace CtUsage
}; // namespace CadKit

#endif // _CADKIT_TRANSLATION_CONTROLLER_USAGE_STRING_H_
