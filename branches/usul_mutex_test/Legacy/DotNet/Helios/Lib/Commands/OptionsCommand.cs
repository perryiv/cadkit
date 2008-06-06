
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class OptionsCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public OptionsCommand(object caller)
      : base()
    {
      _caller = caller;
      _text = "&Options...";
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      try
      {
        CadKit.Helios.OptionsForm form = new CadKit.Helios.OptionsForm("CadKit.Helios.OptionsCommand._execute");
        form.Owner = CadKit.Helios.Application.Instance.MainForm;
        form.Icon = CadKit.Helios.Application.Instance.MainForm.Icon;
        form.Text = CadKit.Helios.Application.Instance.Name + " - Options";
        this._addPages(form);
        if (form.NumPages > 0)
          form.ShowDialog();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1648821079: Failed to show options dialog: {0}", e.Message);
      }
    }


    /// <summary>
    /// All all the pages.
    /// </summary>
    private void _addPages(CadKit.Interfaces.IOptionsForm form)
    {
      try
      {
        CadKit.Interfaces.IOptionsPageAdd[] pages = CadKit.Plugins.Manager.Instance.getAll<CadKit.Interfaces.IOptionsPageAdd>();
        foreach (CadKit.Interfaces.IOptionsPageAdd page in pages)
        {
          try
          {
            this._addPage(form, page);
          }
          catch (System.Exception e)
          {
            System.Console.WriteLine("Error 4696038720: Failed to load option page: {0}", e.Message);
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3413639661: Failed to load all option pages: {0}", e.Message);
      }
    }


    /// <summary>
    /// All the page.
    /// </summary>
    private void _addPage(CadKit.Interfaces.IOptionsForm form, CadKit.Interfaces.IOptionsPageAdd adder)
    {
      if (null == form || null == adder)
        throw new System.ArgumentNullException("Error 3336043850: invalid arguments given", null as System.Exception);

      string name = adder.Name;
      if (null == name)
        throw new System.Exception("Error 2195698280: null name for the page");

      System.Drawing.Image image = adder.Image as System.Drawing.Image;
      string file = adder.Image as string;
      if (null == file && null == image)
        throw new System.Exception("Error 9877185980: no image for page");

      CadKit.Interfaces.IOptionsPage ipage = null;
      if (null != file)
        ipage = form.newPage(adder.Name, file);

      if (null == ipage)
        ipage = form.newPage(adder.Name, image);

      if (null == ipage)
        throw new System.Exception("Error 2999750716: failed to make new options page");

      System.Windows.Forms.Control content = adder.contents(ipage) as System.Windows.Forms.Control;
      if (null == content)
        throw new System.Exception("Error 7244448840: null control for the page content");

      System.Windows.Forms.Control contents = ipage.Contents as System.Windows.Forms.Control;
      if (null == contents)
        throw new System.Exception("Error 3796372180: options page content area is not a recognized type");

      form.add(ipage);
      contents.Controls.Add(content);
      content.Dock = System.Windows.Forms.DockStyle.Fill;
    }


    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      CadKit.Interfaces.IOptionsPageAdd[] pages = CadKit.Plugins.Manager.Instance.getAll<CadKit.Interfaces.IOptionsPageAdd>();
      return ( pages.Length > 0 );
    }
  }
}
