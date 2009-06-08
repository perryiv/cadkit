
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
//  Program to capture the screen pixels.
//
//  See the comment "Doing it with C# itself is much simpler..." here:
//  http://www.codeproject.com/csharp/cscapturescreen1.asp
//
///////////////////////////////////////////////////////////////////////////////

namespace ScreenShot
{
  class Program
  {
    private System.Drawing.Bitmap _getDesktopImage(System.Drawing.Rectangle rect)
    {
      System.Drawing.Bitmap bitmap = new System.Drawing.Bitmap(rect.Width, rect.Height);
      System.Drawing.Graphics graphics = System.Drawing.Graphics.FromImage(bitmap);
      graphics.CopyFromScreen(0, 0, rect.Left, rect.Top, new System.Drawing.Size(rect.Width, rect.Height));
      return bitmap;
    }

    public void saveScreenImage ( string file )
    {
      // Grab screen pixels.
      System.Drawing.Rectangle bounds = System.Windows.Forms.Screen.PrimaryScreen.Bounds;
      System.Drawing.Bitmap bitmap = this._getDesktopImage(bounds);

      // Create parameters to specify quality.
      System.Drawing.Imaging.ImageCodecInfo codec = Program.getCodecInfo("image/jpeg");
      System.Drawing.Imaging.Encoder encoder = System.Drawing.Imaging.Encoder.Quality;
      System.Drawing.Imaging.EncoderParameters parameters = new System.Drawing.Imaging.EncoderParameters(1);
      parameters.Param[0] = new System.Drawing.Imaging.EncoderParameter(encoder, 100L);

      // Make the output file name.
      string output = ((null == file) ? System.String.Format("{0}_screen_shot.jpg", System.DateTime.Now.ToFileTime()) : file);

      // Write the file.
      bitmap.Save(output, codec, parameters);
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

    static void Main(string[] args)
    {
      try
      {
        Program program = new Program();
        program.saveScreenImage((1 == args.Length) ? args[0] : null);
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine(e.Message);
      }
    }
  }
}
