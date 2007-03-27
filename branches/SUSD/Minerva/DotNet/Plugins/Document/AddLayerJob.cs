
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.Document
{
  class AddLayerJob : CadKit.Threads.Jobs.Job
  {
    private CadKit.Interfaces.ILayer _layer = null;
    private DT.Minerva.Plugins.Document.Glue.DllGlue _application = null;

    public AddLayerJob(CadKit.Interfaces.ILayer layer, DT.Minerva.Plugins.Document.Glue.DllGlue application)
    {
      _layer = layer;
      _application = application;
    }

    public void _showLayer(CadKit.Threads.Jobs.Job job)
    {
      _application.addLayer(_layer, job.Progress);
    }

    public void _removeLayer(CadKit.Threads.Jobs.Job job)
    {
      _application.removeLayer(_layer);
    }
  }
}
