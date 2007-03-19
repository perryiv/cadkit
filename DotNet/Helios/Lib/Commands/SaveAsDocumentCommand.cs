
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class SaveAsDocumentCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Local data members.
    /// </summary>
    private string _persistentName = "CadKit.Helios.Commands.SaveAsDocumentCommand.SaveFileDialog";

    /// <summary>
    /// Constructor.
    /// </summary>
    public SaveAsDocumentCommand(object caller)
      : base()
    {
      _caller = caller;
      _text = "Save As...";
    }


    /// <summary>
    /// 
    /// </summary>
    public override void execute()
    {
      try
      {
        string filename = this._askForFileName("Save As");

        CadKit.Interfaces.IFileSave fileSave = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IFileSave;
        if (null != fileSave)
        {
          fileSave.saveAs(filename, _caller);
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 933316032: {0}", e.Message);
      }
    }


    /// <summary>
    /// Ask for the file names.
    /// </summary>
    protected string _askForFileName(string title)
    {
      System.Windows.Forms.SaveFileDialog dialog = new System.Windows.Forms.SaveFileDialog();
      dialog.AddExtension = true;
      dialog.CheckFileExists = true;
      dialog.CheckPathExists = true;
      dialog.DereferenceLinks = true;
      dialog.RestoreDirectory = false;
      dialog.Title = System.String.Format("{0} -- {1}", CadKit.Helios.Application.Instance.Name, title);

      // Set filter string.
      string filterIndexKey = "filter_index";
      this._setFilterString(filterIndexKey, dialog);

      // Get initial directory.
      string initialDirectoryKey = "initial_directory";
      dialog.InitialDirectory = CadKit.Persistence.Registry.Instance.getString(this.PersistentName, initialDirectoryKey, CadKit.Helios.Application.Instance.Directory);

      // Show dialog and get names.
      dialog.ShowDialog();
      string name = dialog.FileName;

      // Save final directory and filter index.
      if (null != name && name.Length > 0)
      {
        System.IO.FileInfo info = new System.IO.FileInfo(name);
        CadKit.Persistence.Registry.Instance.setString(this.PersistentName, initialDirectoryKey, info.DirectoryName);
        CadKit.Persistence.Registry.Instance.setInt(this.PersistentName, filterIndexKey, dialog.FilterIndex);
      }

      // Return names.
      return name;
    }


    /// <summary>
    /// Set the filter string.
    /// </summary>
    private void _setFilterString(string key, System.Windows.Forms.SaveFileDialog dialog)
    {
      string finalFilter = this._getFilterString();

      try
      {
        dialog.Filter = finalFilter;
      }
      catch (System.ArgumentException)
      {
        // No telling what filter strings the plugins will make.
      }

      // Get filter index.
      dialog.FilterIndex = CadKit.Persistence.Registry.Instance.getInt(this.PersistentName, key, 1);
    }


    /// <summary>
    /// Return the filter string.
    /// </summary>
    protected virtual string _getFilterString()
    {
      CadKit.Interfaces.IFiltersSave filtersSave = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IFiltersSave;

      if (null != filtersSave)
      {
        CadKit.Interfaces.Filters filters = new CadKit.Interfaces.Filters();
        filters.AddRange(filtersSave.Filters);
        return this._formatFilterString(filters);
      }

      return "";
    }


    /// <summary>
    /// Return the properly formatted filter string.
    /// </summary>
    protected string _formatFilterString(CadKit.Interfaces.Filters filters)
    {
      filters.Sort(this._compareFilters);

      string finalFilter = "";
      foreach (CadKit.Interfaces.Filter filter in filters)
      {
        finalFilter += (filter.Text + '|' + filter.Extensions + '|');
      }

      finalFilter = finalFilter.TrimEnd(new char[] { '|' });
      return finalFilter;
    }


    /// <summary>
    /// Compare two filters.
    /// </summary>
    private int _compareFilters(CadKit.Interfaces.Filter a, CadKit.Interfaces.Filter b)
    {
      return a.Text.CompareTo(b.Text);
    }


    /// <summary>
    /// 
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      CadKit.Interfaces.IDocument document = CadKit.Documents.Manager.Instance.ActiveDocument;
      if (null != document)
      {
        return document.Modified && document is CadKit.Interfaces.IFileSave;
      }

      return false;
    }


    /// <summary>
    /// Set/get the persistent name.
    /// </summary>
    protected string PersistentName
    {
      get { return _persistentName; }
    }
  }
}
