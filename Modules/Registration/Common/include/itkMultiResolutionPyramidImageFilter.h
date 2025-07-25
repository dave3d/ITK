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
#ifndef itkMultiResolutionPyramidImageFilter_h
#define itkMultiResolutionPyramidImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkArray2D.h"

namespace itk
{
/** \class MultiResolutionPyramidImageFilter
 * \brief Framework for creating images in a multi-resolution
 * pyramid.
 *
 * MultiResolutionPyramidImageFilter provides a generic framework to
 * to create a image pyramid according to a user defined
 * multi-resolution schedule.
 *
 * The multi-resolution schedule is specified in terms for
 * shrink factors at each multi-resolution level for each dimension.
 *
 * A user can either use the default schedules or specify
 * each factor in the schedules directly.
 *
 * The schedule is stored as an unsigned int matrix.
 * An element of the table can be access via the double bracket
 * notation: table[resLevel][dimension]
 *
 * For example:
 *   8 4 4
 *   4 4 2
 *
 * is a schedule for two computation level. In the first (coarsest)
 * level the image is reduce by a factor of 8 in the column dimension,
 * factor of 4 in the row dimension and factor of 4 in the slice dimension.
 * In the second level, the image is reduce by a factor of 4 in the column
 * dimension, 4 is the row dimension and 2 in the slice dimension.
 *
 * The method SetNumberOfLevels() set the number of
 * computation levels in the pyramid. This method will
 * allocate memory for the multi-resolution schedule table.
 * This method generates defaults tables with the starting
 * shrink factor for all dimension set to  2^(NumberOfLevel - 1).
 * All factors are halved for all subsequent levels.
 * For example if the number of levels was set to 4, the default table is:
 *
 * 8 8 8
 * 4 4 4
 * 2 2 2
 * 1 1 1
 *
 * The user can get a copy of the schedule via GetSchedule()
 * They may make alteration and reset it using SetSchedule().
 *
 * A user can create a default table by specifying the starting
 * shrink factors via methods SetStartingShrinkFactors()
 * The factors for subsequent level is generated by
 * halving the factor or setting to one, depending on which is larger.
 *
 * For example, for 4 levels and starting factors of 8,8,4
 * the default table would be:
 *
 * 8 8 4
 * 4 4 2
 * 2 2 1
 * 1 1 1
 *
 * When this filter is updated, NumberOfLevels outputs are produced.
 * The N'th output correspond to the N'th level of the pyramid.
 *
 * To generate each output image, Gaussian smoothing is first performed
 * using a DiscreteGaussianImageFilter with variance (shrink factor / 2)^2.
 * The smoothed image is then downsampled using a ResampleImageFilter.
 *
 * Note that even if the shrink factors are all equal to one, a smoothing
 * will still be applied. The output at the finest level of the pyramid
 * will thus typically be a smoothed version of the input.
 *
 * This class is templated over the input image type and the output image
 * type.
 *
 * This filter uses multithreaded filters to perform the smoothing and
 * downsampling.
 *
 * This filter supports streaming.
 *
 * \sa DiscreteGaussianImageFilter
 * \sa ShrinkImageFilter
 *
 * \ingroup PyramidImageFilter MultiThreaded Streamed
 * \ingroup ITKRegistrationCommon
 */
template <typename TInputImage, typename TOutputImage>
class ITK_TEMPLATE_EXPORT MultiResolutionPyramidImageFilter : public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(MultiResolutionPyramidImageFilter);

  /** Standard class type aliases. */
  using Self = MultiResolutionPyramidImageFilter;
  using Superclass = ImageToImageFilter<TInputImage, TOutputImage>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(MultiResolutionPyramidImageFilter);

  /** ScheduleType type alias support */
  using ScheduleType = Array2D<unsigned int>;

  /** ImageDimension enumeration. */
  static constexpr unsigned int ImageDimension = TInputImage::ImageDimension;
  static constexpr unsigned int OutputImageDimension = TOutputImage::ImageDimension;

  /** Inherit types from Superclass. */
  using typename Superclass::InputImageType;
  using typename Superclass::OutputImageType;
  using typename Superclass::InputImagePointer;
  using typename Superclass::OutputImagePointer;
  using typename Superclass::InputImageConstPointer;

  /** Set the number of multi-resolution levels. The matrix containing the
   * schedule will be resized accordingly.  The schedule is populated with
   * default values.  At the coarsest (0) level, the shrink factors are set
   * 2^(nlevel - 1) for all dimension. These shrink factors are halved for
   * subsequent levels.  The number of levels is clamped to a minimum value
   * of 1.  All shrink factors are also clamped to a minimum value of 1. */
  virtual void
  SetNumberOfLevels(unsigned int num);

  /** Get the number of multi-resolution levels. */
  itkGetConstMacro(NumberOfLevels, unsigned int);

  /** Set a multi-resolution schedule.  The input schedule must have only
   * ImageDimension number of columns and NumberOfLevels number of rows.  For
   * each dimension, the shrink factor must be non-increasing with respect to
   * subsequent levels. This function will clamp shrink factors to satisfy
   * this condition.  All shrink factors less than one will also be clamped
   * to the value of 1. */
  virtual void
  SetSchedule(const ScheduleType & schedule);

  /** Get the multi-resolution schedule. */
  itkGetConstReferenceMacro(Schedule, ScheduleType);

  /** Set the starting shrink factor for the coarsest (0) resolution
   * level. The schedule is then populated with defaults values obtained by
   * halving the factors at the previous level.  All shrink factors are
   * clamped to a minimum value of 1. */
  virtual void
  SetStartingShrinkFactors(unsigned int factor);

  virtual void
  SetStartingShrinkFactors(const unsigned int * factors);

  /** Get the starting shrink factors */
  [[nodiscard]] const unsigned int *
  GetStartingShrinkFactors() const;

  /** Test if the schedule is downward divisible. This method returns true if
   * at every level, the shrink factors are divisible by the shrink factors at
   * the next level. */
  static bool
  IsScheduleDownwardDivisible(const ScheduleType & schedule);

  /** MultiResolutionPyramidImageFilter produces images which are of
   * different resolution and different pixel spacing than its input image.
   * As such, MultiResolutionPyramidImageFilter needs to provide an
   * implementation for GenerateOutputInformation() in order to inform the
   * pipeline execution model.  The original documentation of this method is
   * below.  \sa ProcessObject::GenerateOutputInformaton() */
  void
  GenerateOutputInformation() override;

  /** Given one output whose requested region has been set, this method sets
   * the requested region for the remaining output images.  The original
   * documentation of this method is below.  \sa
   * ProcessObject::GenerateOutputRequestedRegion(); */
  void
  GenerateOutputRequestedRegion(DataObject * refOutput) override;

  /** MultiResolutionPyramidImageFilter requires a larger input requested
   * region than the output requested regions to accommodate the shrinkage and
   * smoothing operations. As such, MultiResolutionPyramidImageFilter needs
   * to provide an implementation for GenerateInputRequestedRegion().  The
   * original documentation of this method is below.  \sa
   * ProcessObject::GenerateInputRequestedRegion() */
  void
  GenerateInputRequestedRegion() override;

  itkSetMacro(MaximumError, double);
  itkGetConstReferenceMacro(MaximumError, double);

  itkSetMacro(UseShrinkImageFilter, bool);
  itkGetConstMacro(UseShrinkImageFilter, bool);
  itkBooleanMacro(UseShrinkImageFilter);

  itkConceptMacro(SameDimensionCheck, (Concept::SameDimension<ImageDimension, OutputImageDimension>));
  itkConceptMacro(OutputHasNumericTraitsCheck, (Concept::HasNumericTraits<typename TOutputImage::PixelType>));

protected:
  MultiResolutionPyramidImageFilter();
  ~MultiResolutionPyramidImageFilter() override = default;
  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  /** Generate the output data for non-downward divisible schedules. */
  void
  GenerateData() override;

  double m_MaximumError{};

  unsigned int m_NumberOfLevels{};
  ScheduleType m_Schedule{};

  bool m_UseShrinkImageFilter{};
};
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkMultiResolutionPyramidImageFilter.hxx"
#endif

#endif
