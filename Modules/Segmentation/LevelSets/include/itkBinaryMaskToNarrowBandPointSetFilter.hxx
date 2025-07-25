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
#ifndef itkBinaryMaskToNarrowBandPointSetFilter_hxx
#define itkBinaryMaskToNarrowBandPointSetFilter_hxx

#include "itkNumericTraits.h"
#include "itkProgressReporter.h"

namespace itk
{
/**
 *
 */
template <typename TInputImage, typename TOutputMesh>
BinaryMaskToNarrowBandPointSetFilter<TInputImage, TOutputMesh>::BinaryMaskToNarrowBandPointSetFilter()
  : m_DistanceFilter(DistanceFilterType::New())
  , m_RescaleFilter(RescaleFilterType::New())
  , m_BandWidth(5)
{
  // Modify superclass default values, can be overridden by subclasses
  this->SetNumberOfRequiredInputs(1);

  m_RescaleFilter->SetOutputMinimum(-0.5);
  m_RescaleFilter->SetOutputMaximum(0.5);

  m_DistanceFilter->SetInput(m_RescaleFilter->GetOutput());
  m_DistanceFilter->NarrowBandingOn();
  m_DistanceFilter->SetNarrowBandwidth(m_BandWidth);

  const PointDataContainerPointer pointData = PointDataContainer::New();

  const OutputMeshPointer mesh = this->GetOutput();

  mesh->SetPointData(pointData);
}

/**
 *
 */
template <typename TInputImage, typename TOutputMesh>
void
BinaryMaskToNarrowBandPointSetFilter<TInputImage, TOutputMesh>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "BandWidth: " << m_BandWidth << std::endl;
}

/**
 *
 */
template <typename TInputImage, typename TOutputMesh>
void
BinaryMaskToNarrowBandPointSetFilter<TInputImage, TOutputMesh>::SetInput(const InputImageType * inputImage)
{
  // This const_cast is needed due to the lack of
  // const-correctness in the ProcessObject.
  this->SetNthInput(0, const_cast<InputImageType *>(inputImage));
}

/**
 *
 */
template <typename TInputImage, typename TOutputMesh>
void
BinaryMaskToNarrowBandPointSetFilter<TInputImage, TOutputMesh>::GenerateData()
{
  m_DistanceFilter->SetNarrowBandwidth(m_BandWidth);

  m_RescaleFilter->SetInput(this->GetInput(0));

  m_DistanceFilter->Update();

  const OutputMeshPointer      mesh = this->GetOutput();
  const InputImageConstPointer image = this->GetInput(0);

  const PointsContainerPointer    points = PointsContainer::New();
  const PointDataContainerPointer pointData = PointDataContainer::New();

  const NodeContainerPointer nodes = m_DistanceFilter->GetOutputNarrowBand();

  const typename std::vector<NodeType>::size_type numberOfPixels = nodes->Size();
  ProgressReporter                                progress(this, 0, static_cast<SizeValueType>(numberOfPixels));

  typename NodeContainer::ConstIterator       nodeItr = nodes->Begin();
  const typename NodeContainer::ConstIterator lastNode = nodes->End();

  PointType point;

  while (nodeItr != lastNode)
  {
    const NodeType & node = nodeItr.Value();
    const float      distance = node.GetValue();
    if (itk::Math::abs(distance) < m_BandWidth)
    {
      image->TransformIndexToPhysicalPoint(node.GetIndex(), point);
      points->push_back(point);
      pointData->push_back(distance);
    }
    ++nodeItr;
    progress.CompletedPixel();
  }

  mesh->SetPoints(points);
  mesh->SetPointData(pointData);

  // This indicates that the current BufferedRegion is equal to the
  // requested region. This action prevents useless re-executions of
  // the pipeline.
  mesh->SetBufferedRegion(mesh->GetRequestedRegion());
}
} // end namespace itk

#endif
