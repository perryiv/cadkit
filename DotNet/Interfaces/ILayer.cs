
namespace CadKit.Interfaces
{
  public interface ILayer : IReferenced
  {
    string Name { get; set; }
    bool Shown { get; }
    bool NeedsUpdate { get; set; }

    void show();
    void modify();
    void hide();
    void addToFavorites();
  }
}
