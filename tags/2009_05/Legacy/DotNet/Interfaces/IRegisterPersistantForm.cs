
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IPersistantFormData
  {
    void registerPersistanceForm(string name, System.Windows.Forms.Form form);
    bool hasPersistantFormData();
  }
}
