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

#include "itkThresholdSegmentationLevelSetImageFilter.h"
#include "itkTestingMacros.h"

namespace TSIFTN
{

using ImageType = itk::Image<float, 3>;
using SeedImageType = itk::Image<signed char, 3>;

constexpr int V_WIDTH = 64;
constexpr int V_HEIGHT = 64;
constexpr int V_DEPTH = 64;

float
sphere(float x, float y, float z)
{
  const float dis =
    (x - float{ V_WIDTH } / 2.0) * (x - float{ V_WIDTH } / 2.0) / ((0.2f * V_WIDTH) * (0.2f * V_WIDTH)) +
    (y - float{ V_HEIGHT } / 2.0) * (y - float{ V_HEIGHT } / 2.0) / ((0.2f * V_HEIGHT) * (0.2f * V_HEIGHT)) +
    (z - float{ V_DEPTH } / 2.0) * (z - float{ V_DEPTH } / 2.0) / ((0.2f * V_DEPTH) * (0.2f * V_DEPTH));
  return (1.0f - dis);
}

void
evaluate_function(itk::Image<signed char, 3> * im, float (*f)(float, float, float))
{
  itk::Image<signed char, 3>::IndexType idx;

  for (int z = 0; z < V_DEPTH; ++z)
  {
    idx[2] = z;
    for (int y = 0; y < V_HEIGHT; ++y)
    {
      idx[1] = y;
      for (int x = 0; x < V_WIDTH; ++x)
      {
        idx[0] = x;
        if (f(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)) >= 0.0)
        {
          im->SetPixel(idx, 1);
        }
        else
        {
          im->SetPixel(idx, 0);
        }
      }
    }
  }
}

} // namespace TSIFTN


namespace itk
{

class RMSCommand : public Command
{
public:
  /** Smart pointer declaration methods */
  using Self = RMSCommand;
  using Superclass = Command;
  using Pointer = itk::SmartPointer<Self>;
  using ConstPointer = itk::SmartPointer<const Self>;
  itkOverrideGetNameOfClassMacro(RMSCommand);
  itkNewMacro(Self);

  /** Standard Command virtual methods */
  void
  Execute(Object * caller, const EventObject &) override
  {
    std::cout << (dynamic_cast<SparseFieldLevelSetImageFilter<::TSIFTN::SeedImageType, ::TSIFTN::ImageType> *>(caller))
                   ->GetRMSChange()
              << std::endl;
    std::cout << (dynamic_cast<SegmentationLevelSetImageFilter<::TSIFTN::SeedImageType, ::TSIFTN::ImageType> *>(caller))
                   ->GetSegmentationFunction()
                   ->GetPropagationWeight()
              << std::endl;
  }
  void
  Execute(const Object *, const EventObject &) override
  {
    std::cout << "ack" << std::endl;
  }

protected:
  RMSCommand() = default;
  ~RMSCommand() override = default;
};


class TSIFTNProgressCommand : public Command
{
public:
  /** Smart pointer declaration methods */
  using Self = TSIFTNProgressCommand;
  using Superclass = Command;
  using Pointer = itk::SmartPointer<Self>;
  using ConstPointer = itk::SmartPointer<const Self>;
  itkOverrideGetNameOfClassMacro(TSIFTNProgressCommand);
  itkNewMacro(Self);

  /** Standard Command virtual methods */
  void
  Execute(Object * caller, const EventObject &) override
  {
    const ProcessObject * process = dynamic_cast<ProcessObject *>(caller);
    std::cout << "Progress = " << process->GetProgress() << std::endl;
  }
  void
  Execute(const Object *, const EventObject &) override
  {
    std::cout << "ack" << std::endl;
  }

protected:
  TSIFTNProgressCommand() = default;
  ~TSIFTNProgressCommand() override = default;
};


} // namespace itk


int
itkThresholdSegmentationLevelSetImageFilterTest(int, char *[])
{

  TSIFTN::ImageType::RegionType            reg;
  TSIFTN::ImageType::RegionType::SizeType  sz;
  TSIFTN::ImageType::RegionType::IndexType idx;
  idx[0] = idx[1] = idx[2] = 0;
  sz[0] = sz[1] = sz[2] = 64;
  reg.SetSize(sz);
  reg.SetIndex(idx);

  const TSIFTN::ImageType::Pointer     inputImage = TSIFTN::ImageType::New();
  const TSIFTN::SeedImageType::Pointer seedImage = TSIFTN::SeedImageType::New();
  inputImage->SetRegions(reg);
  seedImage->SetRegions(reg);
  inputImage->Allocate();
  seedImage->Allocate();

  // Starting surface is a sphere in the center of the image.
  TSIFTN::evaluate_function(seedImage, TSIFTN::sphere);

  // Target surface is a diamond
  //  TSIFTN::ImageType::IndexType idx;
  for (idx[2] = 0; idx[2] < 64; idx[2]++)
    for (idx[1] = 0; idx[1] < 64; idx[1]++)
      for (idx[0] = 0; idx[0] < 64; idx[0]++)
      {
        float val = 0;
        for (unsigned int i = 0; i < 3; ++i)
        {
          if (idx[i] < 32)
          {
            val += idx[i];
          }
          else
          {
            val += 64 - idx[i];
          }
        }
        inputImage->SetPixel(idx, val);
      }

  using FilterType = itk::ThresholdSegmentationLevelSetImageFilter<::TSIFTN::SeedImageType, ::TSIFTN::ImageType>;

  auto filter = FilterType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(filter, ThresholdSegmentationLevelSetImageFilter, SegmentationLevelSetImageFilter);


  filter->SetInput(seedImage);
  ITK_TEST_SET_GET_VALUE(seedImage, filter->GetInitialImage());

  filter->SetInitialImage(seedImage);
  ITK_TEST_SET_GET_VALUE(seedImage, filter->GetInput());
  ITK_TEST_SET_GET_VALUE(seedImage, filter->GetInitialImage());

  filter->SetFeatureImage(inputImage);
  ITK_TEST_SET_GET_VALUE(inputImage, filter->GetFeatureImage());

  auto advectionImage = FilterType::SegmentationFunctionType::VectorImageType::New();
  filter->SetAdvectionImage(advectionImage);
  ITK_TEST_SET_GET_VALUE(advectionImage, filter->GetAdvectionImage());

  constexpr typename FilterType::ValueType upperThreshold = 63;
  filter->SetUpperThreshold(upperThreshold);
  ITK_TEST_SET_GET_VALUE(upperThreshold, filter->GetUpperThreshold());

  constexpr typename FilterType::ValueType lowerThreshold = 50;
  filter->SetLowerThreshold(lowerThreshold);
  ITK_TEST_SET_GET_VALUE(lowerThreshold, filter->GetLowerThreshold());

  constexpr typename FilterType::ValueType edgeWeight = 0.0;
  filter->SetEdgeWeight(edgeWeight);
  ITK_TEST_SET_GET_VALUE(edgeWeight, filter->GetEdgeWeight());

  constexpr int smoothingIterations = 5;
  filter->SetSmoothingIterations(smoothingIterations);
  ITK_TEST_SET_GET_VALUE(smoothingIterations, filter->GetSmoothingIterations());

  constexpr typename FilterType::ValueType smoothingTimeStep = 0.1;
  filter->SetSmoothingTimeStep(smoothingTimeStep);
  ITK_TEST_SET_GET_VALUE(smoothingTimeStep, filter->GetSmoothingTimeStep());

  constexpr typename FilterType::ValueType smoothingConductance = 0.8;
  filter->SetSmoothingConductance(smoothingConductance);
  ITK_TEST_SET_GET_VALUE(smoothingConductance, filter->GetSmoothingConductance());

  filter->SetMaximumRMSError(0.04);
  filter->SetNumberOfIterations(10);

  auto reverseExpansionDirection = true;

  filter->SetUseNegativeFeaturesOff();
  filter->SetUseNegativeFeaturesOn(); // Change the default behavior of the speed
                                      // function so that negative values result in
                                      // surface growth.
  filter->SetUseNegativeFeatures(reverseExpansionDirection);
  ITK_TEST_SET_GET_VALUE(reverseExpansionDirection, filter->GetUseNegativeFeatures());

  ITK_TEST_SET_GET_BOOLEAN(filter, ReverseExpansionDirection, reverseExpansionDirection);

  auto autoGenerateSpeedAdvection = true;
  ITK_TEST_SET_GET_BOOLEAN(filter, AutoGenerateSpeedAdvection, autoGenerateSpeedAdvection);

  const itk::RMSCommand::Pointer c = itk::RMSCommand::New();
  filter->AddObserver(itk::IterationEvent(), c);

  const itk::TSIFTNProgressCommand::Pointer progress = itk::TSIFTNProgressCommand::New();
  filter->AddObserver(itk::ProgressEvent(), progress);

  filter->SetIsoSurfaceValue(0.5); //<--- IMPORTANT!  Default is zero.

  ITK_TRY_EXPECT_NO_EXCEPTION(filter->Update());

  std::cout << "Done first trial" << std::endl;
  // Repeat to make sure that the filter is reinitialized properly
  filter->SetNumberOfIterations(5);

  ITK_TRY_EXPECT_NO_EXCEPTION(filter->Update());

  std::cout << "Done second trial" << std::endl;

  // Write the output for debugging purposes
  //       itk::ImageFileWriter<TSIFTN::ImageType>::Pointer writer
  //          = itk::ImageFileWriter<TSIFTN::ImageType>::New();
  //        itk::RawImageIO<float, 3>::Pointer io = itk::RawImageIO<float, 3>::New();
  //        io->SetFileTypeToBinary();
  //        io->SetFileDimensionality(3);
  //        io->SetByteOrderToLittleEndian();
  //        writer->SetImageIO(io);

  //        itk::CastImageFilter<TSIFTN::SeedImageType, TSIFTN::ImageType>::Pointer
  //         caster = itk::CastImageFilter<TSIFTN::SeedImageType, TSIFTN::ImageType>::New();
  //        caster->SetInput(seedImage);
  //        caster->Update();

  // writer->SetInput(caster->GetOutput());
  //     writer->SetInput(filter->GetSpeedImage());
  //        writer->SetInput(filter->GetFeatureImage());
  // writer->SetInput(inputImage);
  //        writer->SetInput(filter->GetOutput());
  //       writer->SetFileName("output.raw");
  //        writer->Write();

  return EXIT_SUCCESS;
}
