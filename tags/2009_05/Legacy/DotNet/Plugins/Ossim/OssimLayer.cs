
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Ossim
{
  public class OssimLayer : System.Windows.Forms.UserControl
  {
    private System.Collections.Generic.List<CadKit.Interfaces.ILayer> _layers = new System.Collections.Generic.List<CadKit.Interfaces.ILayer>();
    private CadKit.Interfaces.IOptionsPage _page;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.ListView _listView;
    private System.Windows.Forms.Button _browseButton;

    public OssimLayer(CadKit.Interfaces.IOptionsPage page)
    {
      this.InitializeComponent();

      this.Load += this._load;

      _page = page;
      if (null != _page)
        _page.Apply += this._apply;

      _listView.View = System.Windows.Forms.View.Details;
      _listView.Columns.Add("Filename");
      _listView.Columns.Add("Code");
    }

    #region Windows Designer Generated Code

    private void InitializeComponent()
    {
      this._browseButton = new System.Windows.Forms.Button();
      this.panel1 = new System.Windows.Forms.Panel();
      this._listView = new System.Windows.Forms.ListView();
      this.panel1.SuspendLayout();
      this.SuspendLayout();
      // 
      // _browseButton
      // 
      this._browseButton.Location = new System.Drawing.Point(19, 20);
      this._browseButton.Name = "_browseButton";
      this._browseButton.Size = new System.Drawing.Size(75, 23);
      this._browseButton.TabIndex = 1;
      this._browseButton.Text = "Browse...";
      this._browseButton.UseVisualStyleBackColor = true;
      this._browseButton.Click += new System.EventHandler(this._browseButton_Click);
      // 
      // panel1
      // 
      this.panel1.Controls.Add(this._listView);
      this.panel1.Controls.Add(this._browseButton);
      this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panel1.Location = new System.Drawing.Point(0, 0);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(557, 250);
      this.panel1.TabIndex = 2;
      // 
      // _listView
      // 
      this._listView.Location = new System.Drawing.Point(19, 64);
      this._listView.Name = "_listView";
      this._listView.Size = new System.Drawing.Size(504, 132);
      this._listView.TabIndex = 2;
      this._listView.UseCompatibleStateImageBehavior = false;
      // 
      // OssimLayer
      // 
      this.BackColor = System.Drawing.SystemColors.Control;
      this.Controls.Add(this.panel1);
      this.Name = "OssimLayer";
      this.Size = new System.Drawing.Size(557, 250);
      this.panel1.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion


    /// <summary>
    /// Called when the form is loaded.
    /// </summary>
    void _load(object sender, System.EventArgs e)
    {
      System.Windows.Forms.Form parent = this.FindForm();
      if (null != parent)
        parent.FormClosing += this._formClosing;
    }

    /// <summary>
    /// Called when the parent form is closing.
    /// </summary>
    private void _formClosing(object sender, System.Windows.Forms.FormClosingEventArgs e)
    {
      System.Windows.Forms.Form parent = this.FindForm();
      if (null != parent)
        parent.FormClosing -= this._formClosing;

      if (null != _page)
        _page.Apply -= this._apply;
    }


    /// <summary>
    /// Called when the apply (or ok) button is pressed.
    /// </summary>
    private void _apply(CadKit.Interfaces.IOptionsForm form, CadKit.Interfaces.IOptionsPage page)
    {
      CadKit.Interfaces.ILayerList layerList = CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.ILayerList;

      if (null != layerList)
      {
        foreach (CadKit.Interfaces.ILayer layer in _layers)
        {
          layerList.addLayer(layer, this);
        }
      }
    }


    /// <summary>
    /// 
    /// </summary>
    private void _browseButton_Click(object sender, System.EventArgs e)
    {
      try
      {
        System.Windows.Forms.OpenFileDialog dialog = new System.Windows.Forms.OpenFileDialog();
        dialog.Multiselect = true;
        dialog.Filter = "All image files (*.tif, *.jpg)|*.tif; *.jpg|All Files (*.*)|*.*";

        string persistantName = "ossim_image_dialog";

        // Get filter index.
        string filterIndexKey = persistantName + "filter_index";
        dialog.FilterIndex = CadKit.Persistence.Registry.Instance.getInt(persistantName, filterIndexKey, 1);

        // Get initial directory.
        string initialDirectoryKey = persistantName + "initial_directory";
        dialog.InitialDirectory = CadKit.Persistence.Registry.Instance.getString(persistantName, initialDirectoryKey, CadKit.Helios.Application.Instance.Directory);

        if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
        {
          if (dialog.FileNames.Length > 0)
          {
            System.IO.FileInfo info = new System.IO.FileInfo(dialog.FileNames[0]);
            CadKit.Persistence.Registry.Instance.setString(persistantName, initialDirectoryKey, info.DirectoryName);
            CadKit.Persistence.Registry.Instance.setInt(persistantName, filterIndexKey, dialog.FilterIndex);

            foreach (string s in dialog.FileNames)
            {
              _addLayer(s);
            }
          }
        }
      }
      catch (System.Exception ex)
      {
        System.Console.WriteLine("Error 2754833304: {0}", ex.Message);
      }
    }

    private void _addLayer(string s)
    {
      System.IO.FileInfo info = new System.IO.FileInfo(s);
      if (".kwl" == info.Extension)
      {
        CadKit.OSSIMPlanet.Glue.KwlLayer layer = new CadKit.OSSIMPlanet.Glue.KwlLayer(s);
        _layers.Add(layer);

        System.Windows.Forms.ListViewItem item = new System.Windows.Forms.ListViewItem(s);
        item.SubItems.Add(layer.StateCode.ToString());
        _listView.Items.Add(item);
      }
      else
      {
        CadKit.OSSIMPlanet.Glue.ImageLayer layer = new CadKit.OSSIMPlanet.Glue.ImageLayer(s);
        _layers.Add(layer);

        System.Windows.Forms.ListViewItem item = new System.Windows.Forms.ListViewItem(s);
        item.SubItems.Add(layer.StateCode.ToString());
        _listView.Items.Add(item);
      }
    }
  }
}
