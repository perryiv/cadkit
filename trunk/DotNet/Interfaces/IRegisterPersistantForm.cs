
namespace CadKit.Interfaces
{
  public interface PersistantFormData
  {
    void registerPersistanceForm(string name, System.Windows.Forms.Form form);
    bool hasPersistantFormData();
  }
}
