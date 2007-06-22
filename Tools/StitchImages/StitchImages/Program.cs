
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
//  Program to stitch images together.
//
///////////////////////////////////////////////////////////////////////////////

namespace StitchImage
{
  class Program
  {
    ///////////////////////////////////////////////////////////////////////////
    //
    //  Data members and types.
    //
    ///////////////////////////////////////////////////////////////////////////

    private class Images : System.Collections.Generic.List<System.Drawing.Image> { }
    static System.DateTime _last = System.DateTime.Now;


    ///////////////////////////////////////////////////////////////////////////
    //
    //  Report progress.
    //
    ///////////////////////////////////////////////////////////////////////////

    static void progress(int x)
    {
      System.DateTime now = System.DateTime.Now;
      if ((now - _last) > System.TimeSpan.FromSeconds(1))
      {
        System.Console.Write("Writing row {0}\r", x);
        _last = now;
      }
    }


    ///////////////////////////////////////////////////////////////////////////
    //
    //  Run the program.
    //
    ///////////////////////////////////////////////////////////////////////////

    static void run(System.Collections.Generic.List<string> files)
    {
      // Check program arguments.
      string program = System.Reflection.Assembly.GetEntryAssembly().GetName().Name;
      if (files.Count < 3)
      {
        throw new System.Exception(System.String.Format
          ("Usage: {0} {1}", program, "<output image> <input image 1> <input image 2> [input image 3] ..."));
      }

      // Make the final image.
      string output = files[0];
      files.RemoveAt(0);

      // Save the width and height.
      System.Drawing.Size size = new System.Drawing.Size(0, 0);

      // Open the image files.
      Images images = new Images();
      foreach (string file in files)
      {
        System.Drawing.Image image = new System.Drawing.Bitmap(file);
        images.Add(image);
        size.Width += image.Width;
        size.Height = (image.Height > size.Height) ? image.Height : size.Height;
        System.Console.WriteLine("Image '{0}' is {1}", file, image.Size);
      }

      // Make the final image.
      System.Console.WriteLine("Allocating final image '{0}' of size {1}", output, size);
      System.Drawing.Bitmap answer = new System.Drawing.Bitmap(size.Width, size.Height);

      // Reset the size.
      size = new System.Drawing.Size(0, 0);

      foreach (System.Drawing.Bitmap image in images)
      {
        int width = image.Width;
        int height = image.Height;

        System.Console.WriteLine("Current image size: {0}x{1}", image.Width, image.Height);

        for (int i = 0; i < height; ++i)
        {
          int y = i;
          StitchImage.Program.progress(i);
          for (int j = 0; j < width; ++j)
          {
            int x = j + size.Width;
            //System.Diagnostics.Debug.WriteLine(System.String.Format("i = {0}, j = {1}, x = {2}, y = {3}", i, j, x, y));
            answer.SetPixel(x, y, image.GetPixel(j, i));
          }
        }

        size.Width += image.Width;
        size.Height = (image.Height > size.Height) ? image.Height : size.Height;
        System.Console.WriteLine("Pixels written: {0}x{1}", size.Width, size.Height);
      }

      System.Console.WriteLine("Writing final image: {0}", output);
      answer.Save(output);
      System.Console.WriteLine("Done writing image: {0}", output);
    }


    ///////////////////////////////////////////////////////////////////////////
    //
    //  Main function.
    //
    ///////////////////////////////////////////////////////////////////////////

    static void Main(string[] args)
    {
      try
      {
        StitchImage.Program.run(new System.Collections.Generic.List<string>(args));
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine(e.Message);
      }
    }
  }
}
