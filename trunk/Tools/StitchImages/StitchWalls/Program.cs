
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Program to stitch drum walls together.
//
///////////////////////////////////////////////////////////////////////////////

namespace StitchWalls
{
  class Program
  {
    static void Main(string[] args)
    {
      // Check program arguments.
      string program = System.Reflection.Assembly.GetEntryAssembly().GetName().Name;
      if (5 != args.Length && 7 != args.Length)
      {
        System.Console.WriteLine("Usage: {0} {1}", program, "<viz0 image> <viz1 image> ... <viz4 or viz6 image>");
        return;
      }

      // Get the info for the first input image is.
      System.IO.FileInfo info = new System.IO.FileInfo ( args[0] );

      // Generate a unique output name.
      string output = info.DirectoryName + '\\' + System.Guid.NewGuid().ToString() + info.Extension;

      // Run the stitching program.
      System.Diagnostics.Process process = new System.Diagnostics.Process();
      process.StartInfo.FileName = "stitch_images.exe";
      if (5 == args.Length)
      {
        process.StartInfo.Arguments = System.String.Format("{0} {1} {2} {3} {4} {5}", output, args[4], args[2], args[0], args[1], args[3]);
      }
      else
      {
        process.StartInfo.Arguments = System.String.Format("{0} {1} {2} {3} {4} {5} {6} {7}", output, args[6], args[4], args[2], args[0], args[1], args[3], args[5]);
      }
      process.Start();
      process.WaitForExit();
    }
  }
}
