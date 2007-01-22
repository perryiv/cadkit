
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Options.Ossim
{
  public partial class PageContent : System.Windows.Forms.UserControl
  {
    private CadKit.Interfaces.IOptionsPage _page = null;

    public PageContent(CadKit.Interfaces.IOptionsPage page)
    {
      InitializeComponent();

      this.Load += this._load;

      _page = page;
      if (null != _page)
        _page.Apply += this._apply;

      this.Resize += new System.EventHandler(PageContent_Resize);

      if (CadKit.Documents.Manager.Instance.ActiveDocument is CadKit.Interfaces.IOssimPlanetSettings)
      {
        CadKit.Interfaces.IOssimPlanetSettings settings = (CadKit.Interfaces.IOssimPlanetSettings)CadKit.Documents.Manager.Instance.ActiveDocument;
        _elevation.Checked = settings.ElevationEnabled;
        _ephemeris.Checked = settings.EphemerisEnabled;
        _hud.Checked = settings.HudEnabled;

        _cacheDir.Text = settings.ElevationCacheDir;

        _heightExag.Value = new decimal(settings.HeightExageration);
        _maxLevelDetail.Value = new decimal(settings.MaxLevelDetail);
        _elevationPatch.Value = new decimal(settings.ElevationPatchSize);
      }
    }


    /// <summary>
    /// 
    /// </summary>
    void PageContent_Resize(object sender, System.EventArgs e)
    {
      _ossimPlanetGroupBox.Width = this.Width - 15;
      _ossimPlanetGroupBox.Height = this.Height - 5;
    }


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
      if (CadKit.Documents.Manager.Instance.ActiveDocument is CadKit.Interfaces.IOssimPlanetSettings)
      {
        CadKit.Interfaces.IOssimPlanetSettings settings = (CadKit.Interfaces.IOssimPlanetSettings)CadKit.Documents.Manager.Instance.ActiveDocument;
        settings.ElevationEnabled = _elevation.Checked;
        settings.EphemerisEnabled = _ephemeris.Checked;
        settings.HudEnabled = _hud.Checked;

        settings.ElevationCacheDir = _cacheDir.Text;

        settings.HeightExageration = System.Convert.ToSingle(_heightExag.Value);
        settings.MaxLevelDetail = System.Convert.ToSingle(_maxLevelDetail.Value);
        settings.ElevationPatchSize = System.Convert.ToSingle(_elevationPatch.Value);
      }
    }
  }
}
