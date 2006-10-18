
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Threads.Test
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [System.STAThread]
    static void Main(string[] args)
    {
      try
      {
        // Cache this thread's id.
        CadKit.Threads.Tools.MainThread temp = CadKit.Threads.Tools.MainThread.Instance;

        // Setup code for the forms.
        System.Windows.Forms.Application.EnableVisualStyles();
        System.Windows.Forms.Application.SetCompatibleTextRenderingDefault(false);

        // Run the application.
        System.Windows.Forms.Application.Run(new CadKit.Threads.Test.Form());
      }
      catch (System.Exception e)
      {
        if (null != e.Message && e.Message.Length > 0)
        {
          System.Console.WriteLine(e.Message);
        }
      }
    }
  }
}
