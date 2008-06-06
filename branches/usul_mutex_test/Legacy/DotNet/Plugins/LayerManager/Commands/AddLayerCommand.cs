
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.LayerManager.Commands
{
  class AddLayerCommand : CadKit.Commands.Command
  {
    private CadKit.Plugins.LayerManager.Layers _form = null;

    /// <summary>
    /// Constructor.
    /// </summary>
    public AddLayerCommand(object caller, CadKit.Plugins.LayerManager.Layers form)
      : base()
    {
      _form = form;
      _caller = caller;
      _text = "Add Layer";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/add_layer_command.gif");
      _toolIcon = _menuIcon;
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      try
      {
        CadKit.Helios.OptionsForm form = new CadKit.Helios.OptionsForm("CadKit.Plugins.LayerManager.AddLayer");
        form.Owner = CadKit.Helios.Application.Instance.MainForm;
        form.Icon = CadKit.Helios.Application.Instance.MainForm.Icon;
        form.Text = CadKit.Helios.Application.Instance.Name + " - Options";
        this._addPages(form);
        if (form.NumPages > 0)
        {
          CadKit.Interfaces.IRenderLoop renderLoop = CadKit.Documents.Manager.Instance.ActiveView as CadKit.Interfaces.IRenderLoop;

          bool useRenderLoop = false;

          // Turn off render loop before we show the dialog.
          if (null != renderLoop)
          {
            useRenderLoop = renderLoop.UseRenderLoop;
            renderLoop.UseRenderLoop = false;
          }

          try
          {
            if (form.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
              _form.addLayers();
            }
          }
          finally
          {
            // Restore the render loop state.
            if (null != renderLoop)
              renderLoop.UseRenderLoop = useRenderLoop;
          }
        }
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
        CadKit.Interfaces.ICreateLayer[] pages = CadKit.Plugins.Manager.Instance.getAll<CadKit.Interfaces.ICreateLayer>();
        foreach (CadKit.Interfaces.ICreateLayer page in pages)
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
    private void _addPage(CadKit.Interfaces.IOptionsForm form, CadKit.Interfaces.ICreateLayer adder)
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
      return (null != CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.ILayerList);
    }
  }
}
