
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [System.STAThread]
    static void Main ( string[] args )
    {
      try
      {
        // Initialize unmanaged side.
        CadKit.Init.Glue.Init init = new CadKit.Init.Glue.Init();

        // Cache this thread's id.
        CadKit.Threads.Tools.MainThread mainThread = CadKit.Threads.Tools.MainThread.Instance;

        // Redirect standard output to a file.
        CadKit.Tools.RedirectOutput redirect = CadKit.Tools.RedirectOutput.Instance;

        // Setup code for the forms.
        System.Windows.Forms.Application.EnableVisualStyles();
        System.Windows.Forms.Application.SetCompatibleTextRenderingDefault(false);

        // Declare the application and name it.
        CadKit.Helios.Application.Instance.Name = "Helios";
        System.Console.WriteLine(CadKit.Helios.Application.Instance.Name);

        // Set the splash screen image.
        CadKit.Helios.Application.Instance.SplashImage = CadKit.Helios.Application.Instance.IconDir + "/splash_screen.jpg";

        // Declare main form and pass the persistant name.
        CadKit.Helios.MainForm form = new CadKit.Helios.MainForm("CadKit.Helios.MainForm");

        // Set the application's main form.
        CadKit.Helios.Application.Instance.MainForm = form;

        // Run the application.
        System.Windows.Forms.Application.Run(form);

        // Unload the native plugins.
        CadKit.Init.Glue.Plugins.unloadPlugins();
      }
      catch (System.Exception e)
      {
        if (null != e.Message && e.Message.Length > 0)
        {
          string message = System.String.Format("Error 1659684102: {0}", e.Message);
          System.Windows.Forms.MessageBox.Show(message, "Error");
          System.Console.WriteLine(message);
        }
      }
      finally
      {
        // Wait for cleanup.
        System.GC.Collect();
        System.GC.WaitForPendingFinalizers();
      }
    }
  }
}
