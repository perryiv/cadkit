
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach & Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Template image class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_ITK_TEMPLATE_IMAGE_CLASS_H_
#define _IMAGES_ITK_TEMPLATE_IMAGE_CLASS_H_

#include "Images/ITK/BaseImage.h"
#include "Images/ITK/TypeTraits.h"

#include "ITK/Code/Common/itkImage.h"
#include "ITK/Code/IO/itkImageFileReader.h"
#include "ITK/Code/BasicFilters/itkRGBToLuminanceImageFilter.h"
#include "ITK/Code/Numerics/Statistics/itkScalarImageToListAdaptor.h"
#include "ITK/Code/Numerics/Statistics/itkListSampleToHistogramGenerator.h"

#include "usul/Types/Types.h"
#include "usul/Errors/Assert.h"
#include "usul/Interfaces/IGetImageData.h"

#include <algorithm>
#include <xutility>


namespace Images {


template
<
  class ValueType_,
  unsigned int NumChannels_
>
class ItkImage : public Images::ITK::BaseImage
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef Images::ITK::BaseImage BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef itk::Image < ValueType_, NumChannels_ > ImageType;
  typedef typename ImageType::Pointer ImagePtr;
  typedef typename ImageType::ValueType ValueType;
  typedef itk::ImageRegionConstIterator < ImageType > ConstIteratorType;
  typedef itk::ImageRegionIterator < ImageType > IteratorType;
  typedef Usul::Interfaces::IGetImageData < 1142286591u, ValueType > IGetImageData;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Smart-pointer definitions.
  //
  /////////////////////////////////////////////////////////////////////////////

  USUL_DECLARE_REF_POINTERS ( ItkImage );


  /////////////////////////////////////////////////////////////////////////////
  //
  //  The dimension.
  //
  /////////////////////////////////////////////////////////////////////////////

  enum { NUM_CHANNELS = NumChannels_ };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ItkImage ( Unknown *pixels = 0x0 ) : BaseClass(),
    _image ( ImageType::New() )
  {
    // Set the pixels.
    this->setPixels ( pixels );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ItkImage ( const ItkImage &image ) : BaseClass ( image ),
    _image ( ImageType::New() )
  {
    // Set regions and allocate.
    _image->SetLargestPossibleRegion ( image._image->GetLargestPossibleRegion() );
    _image->SetBufferedRegion        ( image._image->GetBufferedRegion()        );
    _image->SetRequestedRegion       ( image._image->GetRequestedRegion()       );
    _image->Allocate();

    // Set all pixel values.
    ConstIteratorType in ( image._image, image._image->GetLargestPossibleRegion() );
    IteratorType out ( _image, _image->GetLargestPossibleRegion() );
    for ( in.GoToBegin(), out.GoToBegin(); !in.IsAtEnd(); ++in, ++out )
      out.Set ( in.Get() );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set this image's pixels.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void setPixels ( Unknown *pixels )
  {
    // See if there are pixels.
    IGetImageData::QueryPtr data ( pixels );
    if ( !data.valid() )
      return;

    // Get the pixels.
    typedef typename IGetImageData::Values ImageDataValues;
    ImageDataValues v;
    data->getImageValues ( v );

    // Set the starting index and size of our image.
    typename ImageType::IndexType start;
    typename ImageType::SizeType size;
    start.Fill ( 0 );
    size.Fill ( v.size() );

    // Set the region.
    typename ImageType::RegionType region;
    region.SetIndex ( start );
    region.SetSize ( size );
    _image->SetRegions( region );

    // Now we can actually allocate.
    _image->Allocate();

    // Set the values of our image.
    IteratorType itr ( _image, _image->GetRequestedRegion() );
    unsigned int count ( 0 );
    for ( itr.GoToBegin(); !itr.IsAtEnd(); ++itr )
      itr.Set ( v.at ( count++ ) );

    // Sanity check.
    USUL_ASSERT ( v.size() == count );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the number of channels.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual unsigned int channels() const
  {
    return NUM_CHANNELS;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return a clone of this image.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual BaseClass *clone() const
  {
    return new ItkImage ( *this );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the number of columns.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual unsigned int columns() const
  {
    return 0; // TODO.
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Calculate the histogram.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void histogram ( Histogram &h, unsigned int numBins ) const
  {
    typedef itk::Statistics::ScalarImageToListAdaptor<ImageType> AdaptorType;
    typedef itk::Statistics::ListSampleToHistogramGenerator<AdaptorType,ValueType_> GeneratorType;
    typedef GeneratorType::HistogramType HistogramType;

    typename AdaptorType::Pointer adaptor ( AdaptorType::New() );
    adaptor->SetImage ( _image );

    typename HistogramType::SizeType bins;
    bins.Fill ( numBins );

    typename GeneratorType::Pointer generator ( GeneratorType::New() );
    generator->SetListSample ( adaptor );
    generator->SetNumberOfBins ( bins );
    generator->SetMarginalScale ( 1 );
    generator->Update();

    typename HistogramType::ConstPointer hist ( generator->GetOutput() );
    numBins = hist->Size();
    h.resize ( numBins );
    for ( unsigned int i = 0; i < numBins; ++i )
    {
      h.at ( i ) = hist->GetFrequency ( i, 0 );
      USUL_ASSERT ( 0 == h.at ( i ) );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Convert to grayscale.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void toGrayScale()
  {
    USUL_ASSERT ( 0 ); // TODO
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Read the file.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void read ( const std::string &name )
  {
    typedef itk::ImageFileReader<ImageType> ReaderType;
    typedef typename ReaderType::Pointer ReaderPtr;
    ReaderPtr reader ( ReaderType::New() );
    reader->SetFileName ( name.c_str() );
    reader->Update();
    _image = reader->GetOutput();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the number of rows.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual unsigned int rows() const
  {
    USUL_ASSERT ( 0 );
    return 0; // TODO.
  }

protected:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Use reference counting.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual ~ItkImage()
  {
  }

private:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  No assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  ItkImage &operator = ( const ItkImage &image );


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Data members.
  //
  /////////////////////////////////////////////////////////////////////////////

  ImagePtr _image;
};


} // Images


#endif // _IMAGES_ITK_TEMPLATE_IMAGE_CLASS_H_
