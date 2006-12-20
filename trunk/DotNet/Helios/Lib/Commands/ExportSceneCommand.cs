
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class ExportSceneCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Data members.
    /// </summary>
    protected CadKit.Interfaces.SceneExport.Option _option = CadKit.Interfaces.SceneExport.Option.ENTIRE_SCENE;

    /// <summary>
    /// Constructor
    /// </summary>
    public ExportSceneCommand(object caller)
      : base()
    {
      _caller = caller;
      _text = "Export Scene";
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
        dialog.Title = System.String.Format("{0} -- {1}", CadKit.Helios.Application.Instance.Name, _text);

        CadKit.Interfaces.IExportScene export = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IExportScene;
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
          export.export(filename, _option);
        }
      }
      catch (System.Exception exception)
      {
        System.Console.WriteLine("Error 314952192: exception caught while trying to export scene.");
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
