using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace DT.Minerva.App
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main()
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

        form.Icon = new System.Drawing.Icon(CadKit.Helios.Application.Instance.IconDir + "/minerva_logo_64.ico");

        // Set the application's main form.
        CadKit.Helios.Application.Instance.MainForm = form;

        // Run the application.
        System.Windows.Forms.Application.Run(form);
        System.GC.Collect();
        System.GC.WaitForPendingFinalizers();
      }
      catch (Exception e)
      {
        System.Windows.Forms.MessageBox.Show(e.Message);
      }
    }
  }
}