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
#ifndef itkShapePriorSegmentationLevelSetFunction_h
#define itkShapePriorSegmentationLevelSetFunction_h

#include "itkSegmentationLevelSetFunction.h"
#include "itkShapeSignedDistanceFunction.h"

namespace itk
{
/** \class ShapePriorSegmentationLevelSetFunction
 *
 * \brief This function is used in ShapePriorSegmentationLevelSetFilter to
 * segment structures in an image based on user supplied edge potential map and
 * shape model.
 *
 * This class extends the basic LevelSetFunction with a shape prior term
 * as developed in \cite leventon2000.
 *
 * \f$ \zeta( \phi^{*} - \phi) \f$
 *
 * where \f$ \phi^{*} \f$ is the signed distance function from a target shape
 * and \f$ \zeta \f$ is a scalar constant.
 *
 * The target shape signed distance function is supplied through a
 * ShapeSignedDistanceFunction object.
 *
 * \sa LevelSetFunction
 * \sa SegmentationLevelSetImageFunction
 * \sa ShapeSignedDistanceFunction
 *
 * \ingroup FiniteDifferenceFunctions
 * \ingroup ITKLevelSets
 */
template <typename TImageType, typename TFeatureImageType = TImageType>
class ITK_TEMPLATE_EXPORT ShapePriorSegmentationLevelSetFunction
  : public SegmentationLevelSetFunction<TImageType, TFeatureImageType>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(ShapePriorSegmentationLevelSetFunction);

  /** Standard class type aliases. */
  using Self = ShapePriorSegmentationLevelSetFunction;
  using Superclass = SegmentationLevelSetFunction<TImageType, TFeatureImageType>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;
  using FeatureImageType = TFeatureImageType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(ShapePriorSegmentationLevelSetFunction);

  /** Extract some parameters from the superclass. */
  using typename Superclass::ImageType;
  using typename Superclass::NeighborhoodType;
  using typename Superclass::ScalarValueType;
  using typename Superclass::FeatureScalarType;
  using typename Superclass::RadiusType;
  using typename Superclass::FloatOffsetType;
  using typename Superclass::VectorImageType;
  using typename Superclass::PixelType;
  using typename Superclass::TimeStepType;
  using typename Superclass::IndexType;
  using typename Superclass::ContinuousIndexType;

  /** Extract some parameters from the superclass. */
  static constexpr unsigned int ImageDimension = Superclass::ImageDimension;

  /** ShapeFunction type alias support */
  using ShapeFunctionType = ShapeSignedDistanceFunction<double, Self::ImageDimension>;
  using ShapeFunctionPointer = typename ShapeFunctionType::ConstPointer;

  /** Zeta. The ShapePriorWeight scales the shape prior term values. */
  /** @ITKStartGrouping */
  void
  SetShapePriorWeight(const ScalarValueType p)
  {
    m_ShapePriorWeight = p;
  }
  ScalarValueType
  GetShapePriorWeight() const
  {
    return m_ShapePriorWeight;
  }
  /** @ITKEndGrouping */
  /** The ShapeFunction encapsulates the signed distance to the shape used to
   * influence the evolution of the level set. */
  /** @ITKStartGrouping */
  void
  SetShapeFunction(const ShapeFunctionType * ptr)
  {
    m_ShapeFunction = ptr;
  }
  const ShapeFunctionType *
  GetShapeFunction() const
  {
    return m_ShapeFunction;
  }
  /** @ITKEndGrouping */
  /** Compute the equation value with the additional shape prior term. */
  PixelType
  ComputeUpdate(const NeighborhoodType & neighborhood,
                void *                   gd,
                const FloatOffsetType & = FloatOffsetType(0.0)) override;

  /** Compute global time step from the global data structure. */
  TimeStepType
  ComputeGlobalTimeStep(void * gd) const override;

  /** A global data type used to store values needed to compute the time step.
   */
  struct ShapePriorGlobalDataStruct : public Superclass::GlobalDataStruct
  {
    ScalarValueType m_MaxShapePriorChange;
  };

  /** Returns a pointer to a global data structure for computing time step. */
  [[nodiscard]] void *
  GetGlobalDataPointer() const override
  {
    auto * ans = new ShapePriorGlobalDataStruct();

    ans->m_MaxAdvectionChange = ScalarValueType{};
    ans->m_MaxPropagationChange = ScalarValueType{};
    ans->m_MaxCurvatureChange = ScalarValueType{};
    ans->m_MaxShapePriorChange = ScalarValueType{};
    return ans;
  }

  /** Release the global data structure. */
  void
  ReleaseGlobalDataPointer(void * GlobalData) const override
  {
    delete (ShapePriorGlobalDataStruct *)GlobalData;
  }

protected:
  ShapePriorSegmentationLevelSetFunction();
  ~ShapePriorSegmentationLevelSetFunction() override = default;

  void
  PrintSelf(std::ostream & os, Indent indent) const override;

private:
  ShapeFunctionPointer m_ShapeFunction{};
  ScalarValueType      m_ShapePriorWeight{};
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkShapePriorSegmentationLevelSetFunction.hxx"
#endif

#endif
