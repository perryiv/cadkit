
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/GeoRSS/Export.h"

#include "Minerva/Core/Data/DataObject.h"

#include "Usul/Math/Vector2.h"

#include <vector>

namespace Minerva {
namespace Layers {
namespace GeoRSS {  

class MINERVA_GEORSS_EXPORT Item : public Minerva::Core::Data::DataObject
{
public:
  typedef Minerva::Core::Data::DataObject  BaseClass;
  typedef Usul::Math::Vec2ui               Size;
  typedef OsgTools::Widgets::Item          WidgetItem;
  typedef std::vector<std::string>         Categories;

  USUL_DECLARE_REF_POINTERS ( Item );
  
  Item();
  
  /// Set/get the categories.
  void                    categories ( const Categories& );
  const Categories&       categories() const;

  /// DataObject has been clicked.
  virtual WidgetItem*     clicked ( Usul::Interfaces::IUnknown* caller = 0x0 ) const;
  
  /// Set/get the date.
  void                    date ( const std::string& );
  const std::string&      date() const;

  /// Set/get the image filename.
  void                    imageFilename ( const std::string& );
  const std::string&      imageFilename() const;

  /// Set/get the image size.
  void                    imageSize ( unsigned int width, unsigned int height );
  Size                    imageSize() const;

protected:
      
  virtual ~Item();

private:
  
  std::string _date;
  std::string _imageFilename;
  Size _imageSize;
  Categories _categories;
};
    
}
}
}
