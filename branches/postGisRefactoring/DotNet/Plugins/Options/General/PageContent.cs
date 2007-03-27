
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Options.General
{
  public partial class PageContent : System.Windows.Forms.UserControl
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();
    private System.ComponentModel.IContainer _components = null;
    private System.Windows.Forms.TableLayoutPanel _tableLayoutPanel = null;
    private System.Windows.Forms.GroupBox _groupBox = null;
    private System.Windows.Forms.RadioButton _standAloneRadioButton = null;
    private System.Windows.Forms.RadioButton _systemMdiRadioButton = null;
    private System.Windows.Forms.RadioButton _tabbedMdiRadioButton;
    private CadKit.Interfaces.IOptionsPage _page;

    /// <summary>
    /// Constants
    /// </summary>
    private string REGISTRY_SECTION = "CadKit.Options.General";
    private string USE_STAND_ALONE_WINDOWS = "use_stand_alone_windows";
    private string USE_SYSTEM_MDI_CHILD_WINDOWS = "use_system_mdi_child_windows";
    private string USE_TABBED_MDI_CHILD_WINDOWS = "use_tabbed_mdi_child_windows";

    /// <summary>
    /// Constructor
    /// </summary>
    public PageContent ( CadKit.Interfaces.IOptionsPage page ) : base()
    {
      InitializeComponent();
      this.Load += this._load;

      _page = page;
      if ( null != _page )
        _page.Apply += this._apply;

      _standAloneRadioButton.Checked = CadKit.Persistence.Registry.Instance.getBool(REGISTRY_SECTION, USE_STAND_ALONE_WINDOWS, true);
      _systemMdiRadioButton.Checked = CadKit.Persistence.Registry.Instance.getBool(REGISTRY_SECTION, USE_SYSTEM_MDI_CHILD_WINDOWS, false);
      _tabbedMdiRadioButton.Checked = CadKit.Persistence.Registry.Instance.getBool(REGISTRY_SECTION, USE_TABBED_MDI_CHILD_WINDOWS, false);
    }

    /// <summary> 
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      lock (_mutex)
      {
        if (disposing && (_components != null))
        {
          _components.Dispose();
        }
        base.Dispose(disposing);
      }
    }

    #region Component Designer generated code

    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this._tableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
      this._groupBox = new System.Windows.Forms.GroupBox();
      this._systemMdiRadioButton = new System.Windows.Forms.RadioButton();
      this._standAloneRadioButton = new System.Windows.Forms.RadioButton();
      this._tabbedMdiRadioButton = new System.Windows.Forms.RadioButton();
      this._tableLayoutPanel.SuspendLayout();
      this._groupBox.SuspendLayout();
      this.SuspendLayout();
      // 
      // _tableLayoutPanel
      // 
      this._tableLayoutPanel.ColumnCount = 1;
      this._tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this._tableLayoutPanel.Controls.Add(this._groupBox, 0, 0);
      this._tableLayoutPanel.Dock = System.Windows.Forms.DockStyle.Fill;
      this._tableLayoutPanel.Location = new System.Drawing.Point(0, 0);
      this._tableLayoutPanel.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
      this._tableLayoutPanel.Name = "_tableLayoutPanel";
      this._tableLayoutPanel.RowCount = 2;
      this._tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this._tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this._tableLayoutPanel.Size = new System.Drawing.Size(330, 251);
      this._tableLayoutPanel.TabIndex = 0;
      // 
      // _groupBox
      // 
      this._groupBox.Controls.Add(this._tabbedMdiRadioButton);
      this._groupBox.Controls.Add(this._systemMdiRadioButton);
      this._groupBox.Controls.Add(this._standAloneRadioButton);
      this._groupBox.Dock = System.Windows.Forms.DockStyle.Fill;
      this._groupBox.Location = new System.Drawing.Point(2, 2);
      this._groupBox.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
      this._groupBox.Name = "_groupBox";
      this._groupBox.Padding = new System.Windows.Forms.Padding(2, 2, 2, 2);
      this._groupBox.Size = new System.Drawing.Size(326, 81);
      this._groupBox.TabIndex = 0;
      this._groupBox.TabStop = false;
      this._groupBox.Text = "Document View Styles";
      // 
      // _mdiRadioButton
      // 
      this._systemMdiRadioButton.AutoSize = true;
      this._systemMdiRadioButton.Location = new System.Drawing.Point(7, 41);
      this._systemMdiRadioButton.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
      this._systemMdiRadioButton.Name = "_mdiRadioButton";
      this._systemMdiRadioButton.Size = new System.Drawing.Size(113, 17);
      this._systemMdiRadioButton.TabIndex = 1;
      this._systemMdiRadioButton.TabStop = true;
      this._systemMdiRadioButton.Text = "System MDI Views";
      this._systemMdiRadioButton.UseVisualStyleBackColor = true;
      // 
      // _standAloneRadioButton
      // 
      this._standAloneRadioButton.AutoSize = true;
      this._standAloneRadioButton.Location = new System.Drawing.Point(7, 20);
      this._standAloneRadioButton.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
      this._standAloneRadioButton.Name = "_standAloneRadioButton";
      this._standAloneRadioButton.Size = new System.Drawing.Size(114, 17);
      this._standAloneRadioButton.TabIndex = 0;
      this._standAloneRadioButton.TabStop = true;
      this._standAloneRadioButton.Text = "Stand-Alone Views";
      this._standAloneRadioButton.UseVisualStyleBackColor = true;
      // 
      // _tabbedRadioButton
      // 
      this._tabbedMdiRadioButton.AutoSize = true;
      this._tabbedMdiRadioButton.Location = new System.Drawing.Point(7, 62);
      this._tabbedMdiRadioButton.Margin = new System.Windows.Forms.Padding(2);
      this._tabbedMdiRadioButton.Name = "_tabbedRadioButton";
      this._tabbedMdiRadioButton.Size = new System.Drawing.Size(116, 17);
      this._tabbedMdiRadioButton.TabIndex = 2;
      this._tabbedMdiRadioButton.TabStop = true;
      this._tabbedMdiRadioButton.Text = "Tabbed MDI Views";
      this._tabbedMdiRadioButton.UseVisualStyleBackColor = true;
      // 
      // PageContent
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add(this._tableLayoutPanel);
      this.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
      this.Name = "PageContent";
      this.Size = new System.Drawing.Size(330, 251);
      this._tableLayoutPanel.ResumeLayout(false);
      this._groupBox.ResumeLayout(false);
      this._groupBox.PerformLayout();
      this.ResumeLayout(false);

    }

    #endregion

    /// <summary>
    /// Called when the form is loaded.
    /// </summary>
    void _load(object sender, System.EventArgs e)
    {
      lock (_mutex)
      {
        System.Windows.Forms.Form parent = this.FindForm();
        if (null != parent)
          parent.FormClosing += this._formClosing;
      }
    }

    /// <summary>
    /// Called when the parent form is closing.
    /// </summary>
    private void _formClosing(object sender, System.Windows.Forms.FormClosingEventArgs e)
    {
      lock (_mutex)
      {
        System.Windows.Forms.Form parent = this.FindForm();
        if (null != parent)
          parent.FormClosing -= this._formClosing;

        if (null != _page)
          _page.Apply -= this._apply;
      }
    }

    /// <summary>
    /// Save the state in the registry.
    /// </summary>
    private void _saveState()
    {
      lock (_mutex)
      {
        CadKit.Persistence.Registry.Instance.setBool(REGISTRY_SECTION, USE_STAND_ALONE_WINDOWS, _standAloneRadioButton.Checked);
        CadKit.Persistence.Registry.Instance.setBool(REGISTRY_SECTION, USE_SYSTEM_MDI_CHILD_WINDOWS, _systemMdiRadioButton.Checked);
        CadKit.Persistence.Registry.Instance.setBool(REGISTRY_SECTION, USE_TABBED_MDI_CHILD_WINDOWS, _tabbedMdiRadioButton.Checked);
      }
    }

    /// <summary>
    /// Called when the apply (or ok) button is pressed.
    /// </summary>
    private void _apply(CadKit.Interfaces.IOptionsForm form, CadKit.Interfaces.IOptionsPage page)
    {
      lock (_mutex)
      {
        this._saveState();
      }
    }
  }
}
