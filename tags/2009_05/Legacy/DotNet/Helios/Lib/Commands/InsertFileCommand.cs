
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class InsertFileCommand : CadKit.Helios.Commands.OpenDocumentCommand
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public InsertFileCommand(object caller)
      : base(caller)
    {
      _text = "&Insert...";
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      string[] names = this._askForFileNames("Insert Files");

      // Insert the files.
      foreach (string name in names)
      {
        try
        {
          this._insert(name);
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 1393551747: {0}", e.Message);
        }
      }
    }


    /// <summary>
    /// Return the filter string.
    /// </summary>
    protected override string _getFilterString()
    {
      CadKit.Interfaces.Filters filters = new CadKit.Interfaces.Filters();
      CadKit.Interfaces.IFiltersInsert filtersInsert = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IFiltersInsert;
      if (null != filtersInsert)
      {
        filters.AddRange(filtersInsert.Filters);
      }
      return this._formatFilterString(filters);
    }


    /// <summary>
    /// Insert the file.
    /// </summary>
    private void _insert(string name)
    {
      // Should be true.
      System.Diagnostics.Debug.Assert(false == CadKit.Helios.Application.Instance.MainForm.InvokeRequired);

      // This will start a separate thread and immediately return.
      InsertDocumentTask task = new InsertDocumentTask(name, _caller);
    }


    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      CadKit.Interfaces.IFileInsert insert = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IFileInsert;
      return (null != insert);
    }
  }
}
