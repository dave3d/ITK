/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkTestingComparisonImageFilter_h
#define itkTestingComparisonImageFilter_h

#include "itkArray.h"
#include "itkNumericTraits.h"
#include "itkImageToImageFilter.h"
#include <mutex>

namespace itk::Testing
{
/**
 * \class ComparisonImageFilter
 * \brief Implements comparison between two images.
 *
 * This filter is used by the testing system to compute the difference between
 * a valid image and an image produced by the test. The comparison value is
 * computed by visiting all the pixels in the baseline images and comparing
 * their values with the pixel values in the neighborhood of the homologous
 * pixel in the other image.
 *
 * \ingroup IntensityImageFilters   MultiThreaded
 * \ingroup ITKTestKernel
 */
template <typename TInputImage, typename TOutputImage>
class ITK_TEMPLATE_EXPORT ComparisonImageFilter : public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(ComparisonImageFilter);

  /** Standard class type aliases. */
  using Self = ComparisonImageFilter;
  using Superclass = ImageToImageFilter<TInputImage, TOutputImage>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(ComparisonImageFilter);

  /** Some convenient type alias. */
  using InputImageType = TInputImage;
  using InputPixelType = typename InputImageType::PixelType;
  using OutputImageType = TOutputImage;
  using OutputPixelType = typename OutputImageType::PixelType;
  using OutputImageRegionType = typename OutputImageType::RegionType;
  using RealType = typename NumericTraits<OutputPixelType>::RealType;
  using AccumulateType = typename NumericTraits<RealType>::AccumulateType;

  /** Set the valid image input.  This will be input 0.  */
  itkSetInputMacro(ValidInput, InputImageType);

  /** Set the test image input.  This will be input 1.  */
  itkSetInputMacro(TestInput, InputImageType);

  /** Verify that the origin, spacing, and direction of both images match
   */
  itkSetMacro(VerifyInputInformation, bool);
  itkGetConstMacro(VerifyInputInformation, bool);
  itkBooleanMacro(VerifyInputInformation);

  /** Set/Get the maximum distance away to look for a matching pixel.
      Default is 0. */
  itkSetMacro(ToleranceRadius, int);
  itkGetConstMacro(ToleranceRadius, int);

  /** Set/Get the minimum threshold for pixels to be different.
      Default is 0. */
  itkSetMacro(DifferenceThreshold, OutputPixelType);
  itkGetConstMacro(DifferenceThreshold, OutputPixelType);

  /** Set/Get ignore boundary pixels.  Useful when resampling may have
   *    introduced difference pixel values along the image edge
   *    Default = false */
  itkSetMacro(IgnoreBoundaryPixels, bool);
  itkGetConstMacro(IgnoreBoundaryPixels, bool);
  itkBooleanMacro(IgnoreBoundaryPixels);

  /** Get statistical attributes for those pixels which exceed the
   * tolerance and radius parameters */
  itkGetConstMacro(MinimumDifference, OutputPixelType);
  itkGetConstMacro(MaximumDifference, OutputPixelType);
  itkGetConstMacro(MeanDifference, RealType);
  itkGetConstMacro(TotalDifference, AccumulateType);
  itkGetConstMacro(NumberOfPixelsWithDifferences, SizeValueType);

protected:
  ComparisonImageFilter();
  ~ComparisonImageFilter() override = default;

  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  /** ComparisonImageFilter can be implemented as a multithreading filter.  Therefore, this implementation provides a
   * DynamicThreadedGenerateData() routine which is called for each region to process.
   */
  void
  DynamicThreadedGenerateData(const OutputImageRegionType &) override;

  void
  BeforeThreadedGenerateData() override;

  void
  AfterThreadedGenerateData() override;

  void
  VerifyInputInformation() const override;

  OutputPixelType m_DifferenceThreshold{};

  RealType        m_MeanDifference{};
  OutputPixelType m_MinimumDifference{ NumericTraits<OutputPixelType>::max() };
  OutputPixelType m_MaximumDifference{ NumericTraits<OutputPixelType>::NonpositiveMin() };
  bool            m_VerifyInputInformation{ true };

  AccumulateType m_TotalDifference{};

  SizeValueType m_NumberOfPixelsWithDifferences{ 0 };

  int m_ToleranceRadius{ 0 };

private:
  bool m_IgnoreBoundaryPixels{ false };

  std::mutex m_Mutex;
};
} // namespace itk::Testing

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkTestingComparisonImageFilter.hxx"
#endif


#endif
