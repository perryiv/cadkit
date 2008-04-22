
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class OpenDocumentCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Local data members.
    /// </summary>
    private string _persistentName = "CadKit.Helios.Commands.OpenDocumentCommand.OpenFileDialog";


    /// <summary>
    /// Constructor.
    /// </summary>
    public OpenDocumentCommand(object caller)
      : base()
    {
      _caller = caller;
      _text = "&Open...";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/open_document.png");
      _toolIcon = _menuIcon;
      _keys = System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O;
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      string[] names = this._askForFileNames("Open Document");

      // Open the files.
      foreach (string name in names)
      {
        try
        {
          this._open(name);
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 8905677560: {0}", e.Message);
        }
      }
    }


    /// <summary>
    /// Open the file.
    /// </summary>
    protected void _open(string name)
    {
      // Should be true.
      System.Diagnostics.Debug.Assert(false == CadKit.Helios.Application.Instance.MainForm.InvokeRequired);

      // This will start a separate thread and immediately return.
      OpenDocumentTask task = new OpenDocumentTask(name, _caller, this);
    }


    /// <summary>
    /// Ask for the file names.
    /// </summary>
    protected string[] _askForFileNames(string title)
    {
      System.Windows.Forms.OpenFileDialog dialog = new System.Windows.Forms.OpenFileDialog();
      dialog.AddExtension = true;
      dialog.CheckFileExists = true;
      dialog.CheckPathExists = true;
      dialog.DereferenceLinks = true;
      dialog.RestoreDirectory = false;
      dialog.Title = System.String.Format("{0} -- {1}", CadKit.Helios.Application.Instance.Name, title);
      dialog.Multiselect = true;

      // Set filter string.
      string filterIndexKey = "filter_index";
      this._setFilterString(filterIndexKey, dialog);

      // Get initial directory.
      string initialDirectoryKey = "initial_directory";
      dialog.InitialDirectory = CadKit.Persistence.Registry.Instance.getString(this.PersistentName, initialDirectoryKey, CadKit.Helios.Application.Instance.Directory);

      // Show dialog and get names.
      dialog.ShowDialog();
      string[] names = dialog.FileNames;

      // Save final directory and filter index.
      if (null != names && names.Length > 0)
      {
        System.IO.FileInfo info = new System.IO.FileInfo(names[0]);
        CadKit.Persistence.Registry.Instance.setString(this.PersistentName, initialDirectoryKey, info.DirectoryName);
        CadKit.Persistence.Registry.Instance.setInt(this.PersistentName, filterIndexKey, dialog.FilterIndex);
      }

      // Return names.
      return names;
    }


    /// <summary>
    /// Set the filter string.
    /// </summary>
    private void _setFilterString(string key, System.Windows.Forms.OpenFileDialog dialog)
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
      CadKit.Interfaces.IFiltersOpen[] filtersOpen = CadKit.Plugins.Manager.Instance.getAll<CadKit.Interfaces.IFiltersOpen>();
      CadKit.Interfaces.Filters filters = new CadKit.Interfaces.Filters();
      foreach (CadKit.Interfaces.IFiltersOpen filterOpen in filtersOpen)
      {
        filters.AddRange(filterOpen.Filters);
      }
      return this._formatFilterString(filters);
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
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      return CadKit.Plugins.Manager.Instance.has<CadKit.Interfaces.IDocumentOpen>();
    }


    /// <summary>
    /// Set/get the persistent name.
    /// </summary>
    protected string PersistentName
    {
      get { using (this.Lock.read()) { return _persistentName; } }
      set { using (this.Lock.write()) { _persistentName = value; } }
    }
  }
}
