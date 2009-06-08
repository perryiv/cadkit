
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Program to convert image files to JPEG.
//
///////////////////////////////////////////////////////////////////////////////

namespace ToJpeg
{
  class Program
  {
    static void Main(string[] args)
    {
      foreach (string file in args)
      {
        try
        {
          System.Console.Write("Reading file: {0} ... ", file);
          using (System.Drawing.Image image = System.Drawing.Image.FromFile(file))
          {
            System.Console.WriteLine("Done");

            System.IO.FileInfo info = new System.IO.FileInfo(file);
            string output = file.Replace(info.Extension, ".jpg");

            System.Console.Write("Writing file: {0} ... ", output);

            // Create parameters to specify quality.
            System.Drawing.Imaging.ImageCodecInfo codec = Program.getCodecInfo("image/jpeg");
            System.Drawing.Imaging.Encoder encoder = System.Drawing.Imaging.Encoder.Quality;
            System.Drawing.Imaging.EncoderParameters parameters = new System.Drawing.Imaging.EncoderParameters(1);
            parameters.Param[0] = new System.Drawing.Imaging.EncoderParameter(encoder, 100L);

            // Write the file.
            image.Save(output, codec, parameters);
            System.Console.WriteLine("Done");
          }
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error processing file '", file, ", ", e.Message);
        }
      }
    }

    static System.Drawing.Imaging.ImageCodecInfo getCodecInfo(string mimeType)
    {
      System.Drawing.Imaging.ImageCodecInfo[] codecs = System.Drawing.Imaging.ImageCodecInfo.GetImageEncoders();
      foreach (System.Drawing.Imaging.ImageCodecInfo codec in codecs)
      {
        if (codec.MimeType == mimeType)
        {
          return codec;
        }
      }
      throw new System.Exception("Failed to find codec for " + mimeType);
    }
  }
}
