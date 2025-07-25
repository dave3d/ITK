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
#ifndef itkProjectedIterativeDeconvolutionImageFilter_hxx
#define itkProjectedIterativeDeconvolutionImageFilter_hxx


namespace itk
{

template <typename TSuperclass>
ProjectedIterativeDeconvolutionImageFilter<TSuperclass>::ProjectedIterativeDeconvolutionImageFilter()
  : m_ProjectionFilter(nullptr)
{}

template <typename TSuperclass>
ProjectedIterativeDeconvolutionImageFilter<TSuperclass>::~ProjectedIterativeDeconvolutionImageFilter()
{
  m_ProjectionFilter = nullptr;
}

template <typename TSuperclass>
void
ProjectedIterativeDeconvolutionImageFilter<TSuperclass>::Initialize(ProgressAccumulator * progress,
                                                                    float                 progressWeight,
                                                                    float                 iterationProgressWeight)
{
  this->Superclass::Initialize(progress, progressWeight, iterationProgressWeight);

  m_ProjectionFilter = ProjectionFilterType::New();
  const typename InternalImageType::PixelType zero{};
  m_ProjectionFilter->ThresholdBelow(zero);
}

template <typename TSuperclass>
void
ProjectedIterativeDeconvolutionImageFilter<TSuperclass>::Iteration(ProgressAccumulator * progress,
                                                                   float                 iterationProgressWeight)
{
  this->Superclass::Iteration(progress, iterationProgressWeight);

  m_ProjectionFilter->SetInput(this->m_CurrentEstimate);
  m_ProjectionFilter->UpdateLargestPossibleRegion();
  this->m_CurrentEstimate = m_ProjectionFilter->GetOutput();
  this->m_CurrentEstimate->DisconnectPipeline();
}

} // end namespace itk

#endif
