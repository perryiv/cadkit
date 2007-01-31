
struct GreyTag{};
struct GreyAlphaTag{};
struct RGBTag{};
struct RGBATag{};


class RGBPolicy
{
  typedef RGBTag PixelFormatTag;
};


template < class Policy > class Image
{
  Policy::PixelFormatTag PixelFormatTag;
};

namespace Detail
{
  template < class InputPixelFormat, class OutputPixelFormat > struct RGB2Grey;

  template <> struct RGB2Grey < RGBTag, GreyTag >
  {
    static convert ( ... )
    {
    }
  };

  template <> struct RGB2Grey < RGBATag, GreyAlphaTag >
  {
    static convert ( ... )
    {
    }
  };
}

template < class InputImgeType, class OutputImageType > void rgb2grey ( InputImgeType &in, OutputImageType &out )
{
  RGB2Grey < InputImgeType::PixelFormatTag, OutputImageType::PixelFormatTag >::convert ( in, out );
}

}


ImageBase *makeGreyScale ( ImageBase &img )
{
  switch ( img.numCHannels() )
  {
  case 3:
    ImageRGB *
    Images::Algorithms::rgb2grey ( 
  }
}


void foo()
{
  ImageBase *_image;

  _image = Images::Algorithms::makeGreyScale ( *_image );
}
