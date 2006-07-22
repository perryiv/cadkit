
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Options dialog that resembles Mozilla's.
//  See: http://www.differentpla.net/node/340
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios
{
  public class OptionsForm : CadKit.Persistence.Form
  {
    /// <summary>
    /// Local types.
    /// </summary>
    class Pages : System.Collections.Generic.List<CadKit.Helios.OptionsPage> { }

    /// <summary>
    /// Data members.
    /// </summary>
    private Pages _pages = new Pages();
    private System.Windows.Forms.TableLayoutPanel _outerTable;
    private System.Windows.Forms.Panel _buttonPanel;
    private System.Windows.Forms.TableLayoutPanel _innerTable;
    private System.Windows.Forms.ListView _listView;
    private System.Windows.Forms.Panel _contentPanel;
    private System.Windows.Forms.Button _okButton;
    private System.Windows.Forms.Button _cancelButton;

    /// <summary>
    /// Constructor
    /// </summary>
    public OptionsForm(string persistentName)
    {
      this.PersistentName = (null == persistentName) ? this.GetType().ToString() : persistentName;
      this.InitializeComponent();
      this.MinimizeBox = false;
      this.MaximizeBox = false;
      this.ShowInTaskbar = false;
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this._outerTable = new System.Windows.Forms.TableLayoutPanel();
      this._buttonPanel = new System.Windows.Forms.Panel();
      this._okButton = new System.Windows.Forms.Button();
      this._cancelButton = new System.Windows.Forms.Button();
      this._innerTable = new System.Windows.Forms.TableLayoutPanel();
      this._listView = new System.Windows.Forms.ListView();
      this._contentPanel = new System.Windows.Forms.Panel();
      this._outerTable.SuspendLayout();
      this._buttonPanel.SuspendLayout();
      this._innerTable.SuspendLayout();
      this.SuspendLayout();
      // 
      // _outerTable
      // 
      this._outerTable.ColumnCount = 1;
      this._outerTable.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this._outerTable.Controls.Add(this._buttonPanel, 0, 1);
      this._outerTable.Controls.Add(this._innerTable, 0, 0);
      this._outerTable.Dock = System.Windows.Forms.DockStyle.Fill;
      this._outerTable.Location = new System.Drawing.Point(0, 0);
      this._outerTable.Name = "_outerTable";
      this._outerTable.RowCount = 2;
      this._outerTable.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this._outerTable.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 60F));
      this._outerTable.Size = new System.Drawing.Size(673, 342);
      this._outerTable.TabIndex = 0;
      // 
      // _buttonPanel
      // 
      this._buttonPanel.BackColor = System.Drawing.SystemColors.Control;
      this._buttonPanel.Controls.Add(this._okButton);
      this._buttonPanel.Controls.Add(this._cancelButton);
      this._buttonPanel.Dock = System.Windows.Forms.DockStyle.Fill;
      this._buttonPanel.Location = new System.Drawing.Point(3, 285);
      this._buttonPanel.Name = "_buttonPanel";
      this._buttonPanel.Size = new System.Drawing.Size(667, 54);
      this._buttonPanel.TabIndex = 0;
      // 
      // _okButton
      // 
      this._okButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this._okButton.Location = new System.Drawing.Point(502, 20);
      this._okButton.Name = "_okButton";
      this._okButton.Size = new System.Drawing.Size(75, 25);
      this._okButton.TabIndex = 0;
      this._okButton.Text = "OK";
      this._okButton.UseVisualStyleBackColor = true;
      this._okButton.Click += new System.EventHandler(this._okButtonClick);
      // 
      // _cancelButton
      // 
      this._cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this._cancelButton.Location = new System.Drawing.Point(583, 20);
      this._cancelButton.Name = "_cancelButton";
      this._cancelButton.Size = new System.Drawing.Size(75, 25);
      this._cancelButton.TabIndex = 1;
      this._cancelButton.Text = "Cancel";
      this._cancelButton.UseVisualStyleBackColor = true;
      this._cancelButton.Click += new System.EventHandler(this._cancelButtonClick);
      // 
      // _innerTable
      // 
      this._innerTable.ColumnCount = 2;
      this._innerTable.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
      this._innerTable.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this._innerTable.Controls.Add(this._listView, 0, 0);
      this._innerTable.Controls.Add(this._contentPanel, 1, 0);
      this._innerTable.Dock = System.Windows.Forms.DockStyle.Fill;
      this._innerTable.Location = new System.Drawing.Point(3, 3);
      this._innerTable.Name = "_innerTable";
      this._innerTable.RowCount = 1;
      this._innerTable.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this._innerTable.Size = new System.Drawing.Size(667, 276);
      this._innerTable.TabIndex = 1;
      // 
      // _listView
      // 
      this._listView.Dock = System.Windows.Forms.DockStyle.Fill;
      this._listView.Location = new System.Drawing.Point(3, 3);
      this._listView.MultiSelect = false;
      this._listView.Name = "_listView";
      this._listView.Size = new System.Drawing.Size(121, 270);
      this._listView.TabIndex = 0;
      this._listView.UseCompatibleStateImageBehavior = false;
      this._listView.SelectedIndexChanged += new System.EventHandler(this._selectedPageChanged);
      // 
      // _contentPanel
      // 
      this._contentPanel.BackColor = System.Drawing.SystemColors.Control;
      this._contentPanel.Dock = System.Windows.Forms.DockStyle.Fill;
      this._contentPanel.Location = new System.Drawing.Point(130, 3);
      this._contentPanel.Name = "_contentPanel";
      this._contentPanel.Size = new System.Drawing.Size(534, 270);
      this._contentPanel.TabIndex = 1;
      // 
      // OptionsForm
      // 
      this.ClientSize = new System.Drawing.Size(673, 342);
      this.Controls.Add(this._outerTable);
      this.Name = "OptionsForm";
      this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this._formClosing);
      this.Load += new System.EventHandler(this._formLoaded);
      this._outerTable.ResumeLayout(false);
      this._buttonPanel.ResumeLayout(false);
      this._innerTable.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    /// <summary>
    /// Called when the button is pressed.
    /// </summary>
    private void _okButtonClick(object sender, System.EventArgs e)
    {
      lock (_mutex)
      {
        this.ok();
      }
    }

    /// <summary>
    /// Called when the button is pressed.
    /// </summary>
    private void _cancelButtonClick(object sender, System.EventArgs e)
    {
      lock (_mutex)
      {
        this.Close();
      }
    }

    /// <summary>
    /// Called when the form closes.
    /// </summary>
    private void _formClosing(object sender, System.Windows.Forms.FormClosingEventArgs e)
    {
      lock (_mutex)
      {
      }
    }

    /// <summary>
    /// Save the changes and close the form.
    /// </summary>
    public void ok()
    {
      lock (_mutex)
      {
        this.apply();
        this.Close();
      }
    }

    /// <summary>
    /// Apply the changes.
    /// </summary>
    public void apply()
    {
      lock (_mutex)
      {
        foreach (OptionsPage page in _pages)
        {
          try
          {
            page.apply();
          }
          catch (System.Exception e)
          {
            System.Console.WriteLine("Error 1014455425: {0}", e.Message);
          }
        }
      }
    }

    /// <summary>
    /// Add a page.
    /// </summary>
    public void add(CadKit.Helios.OptionsPage page)
    {
      lock (_mutex)
      {
        if (null == page || null == page.Page || null == page.Name || null == page.Image)
          throw new System.ArgumentNullException("Error 7852244530: null page or page property given");

        _pages.Add(page);

        if (null == _listView.LargeImageList)
        {
          _listView.LargeImageList = new System.Windows.Forms.ImageList();
        }

        System.Drawing.Size size = _listView.LargeImageList.ImageSize;
        _listView.LargeImageList.ImageSize = new System.Drawing.Size(System.Math.Max(size.Width, page.Image.Width), System.Math.Max(size.Height, page.Image.Height));
        _listView.Items.Add(page.Name, _listView.LargeImageList.Images.Count);
        _listView.LargeImageList.Images.Add(page.Image);
      }
    }

    /// <summary>
    /// Add a page.
    /// </summary>
    public CadKit.Helios.OptionsPage add(string name, System.Drawing.Image image)
    {
      lock (_mutex)
      {
        CadKit.Helios.OptionsPage page = new CadKit.Helios.OptionsPage(name, image);
        this.add(page);
        return page;
      }
    }

    /// <summary>
    /// Add a page.
    /// </summary>
    public CadKit.Helios.OptionsPage add(string name, string iconFile)
    {
      lock (_mutex)
      {
        System.IO.FileInfo info = new System.IO.FileInfo(iconFile);
        string path = (true == info.Exists) ? iconFile : (CadKit.Helios.Application.Instance.IconDir + "/" + iconFile);
        System.Drawing.Image image = CadKit.Images.Image.load(path);
        return this.add(name, image);
      }
    }

    /// <summary>
    /// Called when the selected page changed.
    /// </summary>
    private void _selectedPageChanged(object sender, System.EventArgs args)
    {
      lock (_mutex)
      {
        try
        {
          System.Windows.Forms.ListView lv = sender as System.Windows.Forms.ListView;
          if (null == lv || null == lv.SelectedItems || lv.SelectedItems.Count < 1)
            return;

          int index = lv.SelectedItems[0].Index;
          CadKit.Helios.OptionsPage page = ((index >= 0 && index < _pages.Count) ? _pages[index] : null);
          if (null != page && null != page.Page)
          {
            _contentPanel.Controls.Clear();
            _contentPanel.Controls.Add(page.Page);
            page.Page.Dock = System.Windows.Forms.DockStyle.Fill;
            page.Page.Show();
          }
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 7515917920: {0}", e.Message);
        }
      }
    }

    /// <summary>
    /// Called when the form is loaded.
    /// </summary>
    private void _formLoaded(object sender, System.EventArgs args)
    {
      lock (_mutex)
      {
        try
        {
          _listView.Focus();
          if (_listView.Items.Count > 0)
          {
            _listView.Items[0].Selected = true;
          }
        }
        catch (System.Exception e)
        {
          System.Console.WriteLine("Error 1840228368: {0}", e.Message);
        }
      }
    }
  }
}
