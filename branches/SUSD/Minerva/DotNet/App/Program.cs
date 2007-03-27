
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.App
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [System.STAThread]
    static void Main()
    {
      Program._main();
    }


    /// <summary>
    /// Run the program.
    /// </summary>
    private static void _main()
    {
      try
      {
        // Run the program.
        Program._run();

        // Clean up.
        System.GC.Collect();
        System.GC.WaitForPendingFinalizers();

        // Should be zero objects left.
        if (0 != CadKit.Referenced.Base.TotalNumObjects)
        {
          System.Diagnostics.Trace.WriteLine(System.String.Format("{0} objects remaining", CadKit.Referenced.Base.TotalNumObjects));
          System.Diagnostics.Trace.WriteLine(CadKit.Referenced.Base.ExistingObjects);
        }
      }
      catch (System.Exception e)
      {
        System.Windows.Forms.MessageBox.Show(e.Message);
      }
    }


    /// <summary>
    /// Run the program.
    /// </summary>
    private static void _run()
    {
      try
      {
        // Do this first.
        CadKit.Init.Glue.Init init = new CadKit.Init.Glue.Init();

        // Cache this thread's id.
        CadKit.Threads.Tools.MainThread temp = CadKit.Threads.Tools.MainThread.Instance;

        // Redirect standard output to a file.
        CadKit.Tools.RedirectOutput redirect = CadKit.Tools.RedirectOutput.Instance;

        // Setup code for the forms.
        System.Windows.Forms.Application.EnableVisualStyles();
        System.Windows.Forms.Application.SetCompatibleTextRenderingDefault(false);

        // Declare the application and name it.
        CadKit.Helios.Application.Instance.Name = "Minerva";
        System.Console.WriteLine(CadKit.Helios.Application.Instance.Name);

        // Set the splash screen image.
        CadKit.Helios.Application.Instance.SplashImage = CadKit.Helios.Application.Instance.IconDir + "/dt-logo-transparent.gif";

        // Declare main form and pass the persistant name.
        CadKit.Helios.MainForm form = new CadKit.Helios.MainForm("DT.Minerva.Lib.MainForm");

        // Add icon.
        form.Icon = CadKit.Images.Image.icon(CadKit.Helios.Application.Instance.IconDir + "/minerva_logo_16.png");

        // Set the application's main form.
        CadKit.Helios.Application.Instance.MainForm = form;

        // Run the application.
        System.Windows.Forms.Application.Run(form);

        // Unload the native plugins.
        CadKit.Init.Glue.Plugins.unloadPlugins();
      }
      catch (System.Exception e)
      {
        System.Windows.Forms.MessageBox.Show(e.Message);
      }
    }
  }
}
