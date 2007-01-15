
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Plugins.Document
{
  class ModifyLayerJob : CadKit.Threads.Jobs.Job
  {
    private DT.Minerva.Interfaces.ILayer _layer = null;
    private DT.Minerva.Plugins.Document.Glue.DllGlue _application = null;

    public ModifyLayerJob(DT.Minerva.Interfaces.ILayer layer, DT.Minerva.Plugins.Document.Glue.DllGlue application)
    {
      _layer = layer;
      _application = application;
    }

    public void _showLayer(CadKit.Threads.Jobs.Job job)
    {
      _application.modifyLayer(_layer, job.Progress);
    }

    public void _removeLayer(CadKit.Threads.Jobs.Job job)
    {
      _application.removeLayer(_layer);
    }
  }
}
