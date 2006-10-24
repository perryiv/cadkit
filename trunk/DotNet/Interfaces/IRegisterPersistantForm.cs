
namespace CadKit.Interfaces
{
  public interface IPersistantFormData
  {
    void registerPersistanceForm(string name, System.Windows.Forms.Form form);
    bool hasPersistantFormData();
  }
}
