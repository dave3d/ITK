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
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkTranslationTransform.h"
#include "itkTestingMacros.h"

/*
 * Simple test to run using unix 'time' function for speed test.
 */

int
itkMeanSquaresImageToImageMetricv4SpeedTest(int argc, char * argv[])
{
  if (argc < 3)
  {
    std::cerr << "usage: " << itkNameOfTestExecutableMacro(argv) << ": image-dimension number-of-reps" << std::endl;
    return EXIT_FAILURE;
  }
  const int imageSize = std::stoi(argv[1]);
  const int numberOfReps = std::stoi(argv[2]);

  std::cout << "image dim: " << imageSize << ", reps: " << numberOfReps << std::endl;

  constexpr unsigned int imageDimensionality = 3;
  using ImageType = itk::Image<double, imageDimensionality>;

  auto                           size = ImageType::SizeType::Filled(imageSize);
  constexpr ImageType::IndexType index{};
  const ImageType::RegionType    region{ index, size };

  /* Create simple test images. */
  auto fixedImage = ImageType::New();
  fixedImage->SetRegions(region);
  fixedImage->Allocate();

  auto movingImage = ImageType::New();
  movingImage->SetRegions(region);
  movingImage->Allocate();

  /* Fill images */
  itk::ImageRegionIterator<ImageType> itFixed(fixedImage, region);
  itFixed.GoToBegin();
  unsigned int count = 1;
  while (!itFixed.IsAtEnd())
  {
    itFixed.Set(count);
    count++;
    ++itFixed;
  }

  itk::ImageRegionIteratorWithIndex<ImageType> itMoving(movingImage, region);

  itMoving.GoToBegin();
  count = 1;

  while (!itMoving.IsAtEnd())
  {
    itMoving.Set(count * count);
    count++;
    ++itMoving;
  }

  /* Transforms */
  using FixedTransformType = itk::TranslationTransform<double, imageDimensionality>;
  using MovingTransformType = itk::TranslationTransform<double, imageDimensionality>;

  auto fixedTransform = FixedTransformType::New();
  auto movingTransform = MovingTransformType::New();

  fixedTransform->SetIdentity();
  movingTransform->SetIdentity();

  /* The metric */
  using MetricType = itk::MeanSquaresImageToImageMetricv4<ImageType, ImageType, ImageType>;

  auto metric = MetricType::New();

  /* Assign images and transforms.
   * By not setting a virtual domain image or virtual domain settings,
   * the metric will use the fixed image for the virtual domain. */
  metric->SetFixedImage(fixedImage);
  metric->SetMovingImage(movingImage);
  metric->SetFixedTransform(fixedTransform);
  metric->SetMovingTransform(movingTransform);

  /* Initialize. */
  std::cout << "Calling Initialize..." << std::endl;
  metric->Initialize();

  // Evaluate with GetValueAndDerivative
  MetricType::MeasureType    valueReturn1 = NAN;
  MetricType::DerivativeType derivativeReturn;

  MetricType::MeasureType sum{};
  for (int r = 0; r < numberOfReps; ++r)
  {
    metric->GetValueAndDerivative(valueReturn1, derivativeReturn);
    // Sum results to prevent optimizations
    sum += valueReturn1 + derivativeReturn[0];
  }

  std::cout << "sum: " << sum << std::endl;

  return EXIT_SUCCESS;
}
