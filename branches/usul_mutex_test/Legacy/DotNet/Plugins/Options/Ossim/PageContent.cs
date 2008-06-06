
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
        _latLongGrid.Checked = settings.LatLongGrid;

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

        if(settings.ElevationEnabled != _elevation.Checked)
          settings.ElevationEnabled = _elevation.Checked;

        if(settings.EphemerisEnabled != _ephemeris.Checked)
          settings.EphemerisEnabled = _ephemeris.Checked;

        if(settings.HudEnabled != _hud.Checked)
          settings.HudEnabled = _hud.Checked;

        if(settings.LatLongGrid != _latLongGrid.Checked)
          settings.LatLongGrid = _latLongGrid.Checked;

        if(settings.ElevationCacheDir != _cacheDir.Text )
          settings.ElevationCacheDir = _cacheDir.Text;

        float heightExag = System.Convert.ToSingle(_heightExag.Value);

        if (settings.HeightExageration != heightExag)
          settings.HeightExageration = heightExag;

        float maxLevelDetail = System.Convert.ToSingle(_maxLevelDetail.Value);

        if (settings.MaxLevelDetail != maxLevelDetail)
          settings.MaxLevelDetail = maxLevelDetail;

        float elevationPatch = System.Convert.ToSingle(_elevationPatch.Value);

        if (settings.ElevationPatchSize != elevationPatch)
          settings.ElevationPatchSize = elevationPatch;
      }
    }
  }
}
