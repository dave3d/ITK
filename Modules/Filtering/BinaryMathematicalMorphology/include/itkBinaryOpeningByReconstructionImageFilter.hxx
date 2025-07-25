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
#ifndef itkBinaryOpeningByReconstructionImageFilter_hxx
#define itkBinaryOpeningByReconstructionImageFilter_hxx

#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryReconstructionByDilationImageFilter.h"
#include "itkProgressAccumulator.h"

namespace itk
{

template <typename TInputImage, typename TKernel>
BinaryOpeningByReconstructionImageFilter<TInputImage, TKernel>::BinaryOpeningByReconstructionImageFilter()
  : m_ForegroundValue(NumericTraits<PixelType>::max())
  , m_BackgroundValue(PixelType{})
{}

template <typename TInputImage, typename TKernel>
void
BinaryOpeningByReconstructionImageFilter<TInputImage, TKernel>::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // We need all the input.
  const InputImagePointer input = const_cast<InputImageType *>(this->GetInput());
  if (input)
  {
    input->SetRequestedRegion(input->GetLargestPossibleRegion());
  }
}

template <typename TInputImage, typename TKernel>
void
BinaryOpeningByReconstructionImageFilter<TInputImage, TKernel>::GenerateData()
{
  // Allocate the outputs
  this->AllocateOutputs();

  /** set up erosion and dilation methods */
  auto erode = BinaryErodeImageFilter<InputImageType, OutputImageType, TKernel>::New();
  erode->SetForegroundValue(m_ForegroundValue); // Intensity value to erode
  erode->SetBackgroundValue(m_BackgroundValue); // Replacement value of eroded voxels
  erode->SetKernel(this->GetKernel());
  erode->SetInput(this->GetInput());
  erode->ReleaseDataFlagOn();
  erode->SetNumberOfWorkUnits(this->GetNumberOfWorkUnits());

  auto dilate = BinaryReconstructionByDilationImageFilter<OutputImageType>::New();
  dilate->SetForegroundValue(m_ForegroundValue);
  dilate->SetBackgroundValue(m_BackgroundValue);
  dilate->SetMarkerImage(erode->GetOutput());
  dilate->SetMaskImage(this->GetInput());
  dilate->SetFullyConnected(m_FullyConnected);
  dilate->ReleaseDataFlagOn();
  dilate->SetNumberOfWorkUnits(this->GetNumberOfWorkUnits());

  /** set up the minipipeline */
  auto progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);
  progress->RegisterInternalFilter(erode, .8f);
  progress->RegisterInternalFilter(dilate, .2f);

  /** execute the minipipeline */
  dilate->GraftOutput(this->GetOutput());
  dilate->Update();
  this->GraftOutput(dilate->GetOutput());
}

template <typename TInputImage, typename TKernel>
void
BinaryOpeningByReconstructionImageFilter<TInputImage, TKernel>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "ForegroundValue: " << static_cast<typename NumericTraits<PixelType>::PrintType>(m_ForegroundValue)
     << std::endl;
  os << indent << "BackgroundValue: " << static_cast<typename NumericTraits<PixelType>::PrintType>(m_BackgroundValue)
     << std::endl;
  itkPrintSelfBooleanMacro(FullyConnected);
}

} // end namespace itk
#endif
