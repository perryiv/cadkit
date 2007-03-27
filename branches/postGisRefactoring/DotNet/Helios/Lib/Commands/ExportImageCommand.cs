
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ExportImageCommand : CadKit.Commands.Command
  {
    public ExportImageCommand(object caller)
      : base()
    {
      _caller = caller;
      _text = "Export Image";
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      try
      {
        System.Windows.Forms.SaveFileDialog dialog = new System.Windows.Forms.SaveFileDialog();
        dialog.AddExtension = true;
        dialog.Title = CadKit.Helios.Application.Instance.Name + " -- Export Image";

        CadKit.Interfaces.IExportImage export = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IExportImage;
        if (null != export)
        {
          string finalFilter = "";
          CadKit.Interfaces.Filters filters = export.Filters;
          foreach (CadKit.Interfaces.Filter filter in filters)
          {
            finalFilter += (filter.Text + '|' + filter.Extensions + '|');
          }
          finalFilter = finalFilter.TrimEnd(new char[] { '|' });
          dialog.Filter = finalFilter;
          dialog.ShowDialog();
          string filename = dialog.FileName;
          export.export(filename);
        }
      }
      catch (System.Exception exception)
      {
        System.Console.WriteLine("Error 2610699216: exception caught while trying to export image.");
        System.Console.WriteLine("Message: {0}", exception.Message);
      }
    }

    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      return CadKit.Documents.Manager.Instance.ActiveView != null;
    }
  }
}
