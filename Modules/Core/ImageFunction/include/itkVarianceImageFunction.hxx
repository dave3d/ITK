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
#ifndef itkVarianceImageFunction_hxx
#define itkVarianceImageFunction_hxx


#include "itkConstNeighborhoodIterator.h"

namespace itk
{

template <typename TInputImage, typename TCoordinate>
VarianceImageFunction<TInputImage, TCoordinate>::VarianceImageFunction()
  : m_NeighborhoodRadius(1)
{}

template <typename TInputImage, typename TCoordinate>
void
VarianceImageFunction<TInputImage, TCoordinate>::PrintSelf(std::ostream & os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "NeighborhoodRadius: " << m_NeighborhoodRadius << std::endl;
}

template <typename TInputImage, typename TCoordinate>
auto
VarianceImageFunction<TInputImage, TCoordinate>::EvaluateAtIndex(const IndexType & index) const -> RealType
{
  RealType sum;
  RealType sumOfSquares;
  RealType var;

  sum = RealType{};
  sumOfSquares = RealType{};

  if (!this->GetInputImage())
  {
    return (NumericTraits<RealType>::max());
  }

  if (!this->IsInsideBuffer(index))
  {
    return (NumericTraits<RealType>::max());
  }

  // Create an N-d neighborhood kernel, using a zeroflux boundary condition
  auto kernelSize = InputImageType::SizeType::Filled(m_NeighborhoodRadius);

  ConstNeighborhoodIterator<InputImageType> it(
    kernelSize, this->GetInputImage(), this->GetInputImage()->GetBufferedRegion());

  // Set the iterator at the desired location
  it.SetLocation(index);

  // Walk the neighborhood
  const unsigned int size = it.Size();
  for (unsigned int i = 0; i < size; ++i)
  {
    const auto value = static_cast<RealType>(it.GetPixel(i));
    sum += value;
    sumOfSquares += value * value;
  }

  const auto num = static_cast<double>(size);
  var = (sumOfSquares - (sum * sum / num)) / (num - 1.0);

  return (var);
}
} // end namespace itk

#endif
