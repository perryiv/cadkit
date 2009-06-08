
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Tools
{
  public class AskForFilename
  {
    private string _title = "";
    private string _name = "";
    private string _persistentName = "";
    private CadKit.Interfaces.Filters _filters = null;


    /// <summary>
    /// Constructor.
    /// </summary>
    public AskForFilename(string title, string name, string persistentName, CadKit.Interfaces.Filters filters)
    {
      _title = title;
      _name = name;
      _persistentName = persistentName;
      _filters = filters;
    }


    /// <summary>
    /// Set/get the persistent name.
    /// </summary>
    public string PersistentName
    {
      get { return _persistentName; }
      set { _persistentName = value; }
    }


    /// <summary>
    /// Get/Set the title.
    /// </summary>
    public string Title
    {
      get { return _title; }
      set { _title = value; }
    }


    /// <summary>
    /// Get/Set the title.
    /// </summary>
    public string Name
    {
      get { return _name; }
      set { _name = value; }
    }


    /// <summary>
    /// Get/Set the filters.
    /// </summary>
    public CadKit.Interfaces.Filters Filters
    {
      get { return _filters; }
      set { _filters = value; }
    }


    /// <summary>
    /// Get the filename
    /// </summary>
    public string Filename
    {
      get { return this._askForFileName(this.Title); }
    }


    /// <summary>
    /// Ask for the file names.
    /// </summary>
    protected string _askForFileName(string title)
    {
      System.Windows.Forms.SaveFileDialog dialog = new System.Windows.Forms.SaveFileDialog();
      dialog.AddExtension = true;
      dialog.CheckPathExists = true;
      dialog.DereferenceLinks = true;
      dialog.RestoreDirectory = false;
      dialog.Title = System.String.Format("{0} -- {1}", this.Name, title);

      // Set filter string.
      string filterIndexKey = "filter_index";
      this._setFilterString(filterIndexKey, dialog);

      // Get initial directory.
      string initialDirectoryKey = "initial_directory";
      dialog.InitialDirectory = CadKit.Persistence.Registry.Instance.getString(this.PersistentName, initialDirectoryKey, "");

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
      if (null != this.Filters)
      {
        return this._formatFilterString(this.Filters);
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

  }
}
