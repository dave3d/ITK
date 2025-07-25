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
#ifndef itkCompositeTransform_hxx
#define itkCompositeTransform_hxx


namespace itk
{

template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::GetTransformCategory() const -> TransformCategoryEnum
{
  // Check if linear
  const bool isLinearTransform = this->IsLinear();
  if (isLinearTransform)
  {
    return Self::TransformCategoryEnum::Linear;
  }

  // Check if displacement field
  bool isDisplacementFieldTransform = true;
  for (long tind = static_cast<long>(this->GetNumberOfTransforms()) - 1; tind >= 0; tind--)
  {
    if (this->GetNthTransformToOptimize(tind) && (this->GetNthTransformConstPointer(tind)->GetTransformCategory() !=
                                                  Self::TransformCategoryEnum::DisplacementField))
    {
      isDisplacementFieldTransform = false;
      break;
    }
  }

  if (isDisplacementFieldTransform)
  {
    return Self::TransformCategoryEnum::DisplacementField;
  }

  return Self::TransformCategoryEnum::UnknownTransformCategory;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformPoint(const InputPointType & inputPoint) const
  -> OutputPointType
{

  /* Apply in reverse queue order.  */
  OutputPointType outputPoint(inputPoint);
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputPoint = (*it)->TransformPoint(outputPoint);
  }
  return outputPoint;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformVector(const InputVectorType & inputVector) const
  -> OutputVectorType
{
  OutputVectorType outputVector(inputVector);


  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputVector = (*it)->TransformVector(outputVector);
  }

  return outputVector;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformVector(const InputVectorType & inputVector,
                                                                      const InputPointType &  inputPoint) const
  -> OutputVectorType
{
  OutputVectorType outputVector(inputVector);
  OutputPointType  outputPoint(inputPoint);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputVector = (*it)->TransformVector(outputVector, outputPoint);
    outputPoint = (*it)->TransformPoint(outputPoint);
  }

  return outputVector;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformVector(const InputVnlVectorType & inputVector,
                                                                      const InputPointType &     inputPoint) const
  -> OutputVnlVectorType
{
  OutputVnlVectorType outputVector(inputVector);
  OutputPointType     outputPoint(inputPoint);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputVector = (*it)->TransformVector(outputVector, outputPoint);
    outputPoint = (*it)->TransformPoint(outputPoint);
  }

  return outputVector;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformVector(const InputVnlVectorType & inputVector) const
  -> OutputVnlVectorType
{
  OutputVnlVectorType outputVector(inputVector);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputVector = (*it)->TransformVector(outputVector);
  }

  return outputVector;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformVector(const InputVectorPixelType & inputVector) const
  -> OutputVectorPixelType
{
  OutputVectorPixelType outputVector(inputVector);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputVector = (*it)->TransformVector(outputVector);
  }

  return outputVector;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformVector(const InputVectorPixelType & inputVector,
                                                                      const InputPointType &       inputPoint) const
  -> OutputVectorPixelType
{
  OutputVectorPixelType outputVector(inputVector);
  OutputPointType       outputPoint(inputPoint);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputVector = (*it)->TransformVector(outputVector, outputPoint);
    outputPoint = (*it)->TransformPoint(outputPoint);
  }

  return outputVector;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformCovariantVector(
  const InputCovariantVectorType & inputVector) const -> OutputCovariantVectorType
{
  OutputCovariantVectorType outputVector(inputVector);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputVector = (*it)->TransformCovariantVector(outputVector);
  }

  return outputVector;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformCovariantVector(
  const InputCovariantVectorType & inputVector,
  const InputPointType &           inputPoint) const -> OutputCovariantVectorType
{
  OutputCovariantVectorType outputVector(inputVector);
  OutputPointType           outputPoint(inputPoint);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputVector = (*it)->TransformCovariantVector(outputVector, outputPoint);
    outputPoint = (*it)->TransformPoint(outputPoint);
  }

  return outputVector;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformCovariantVector(
  const InputVectorPixelType & inputVector) const -> OutputVectorPixelType
{
  OutputVectorPixelType outputVector(inputVector);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputVector = (*it)->TransformCovariantVector(outputVector);
  }

  return outputVector;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformCovariantVector(const InputVectorPixelType & inputVector,
                                                                               const InputPointType & inputPoint) const
  -> OutputVectorPixelType
{
  OutputVectorPixelType outputVector(inputVector);
  OutputPointType       outputPoint(inputPoint);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputVector = (*it)->TransformCovariantVector(outputVector, outputPoint);
    outputPoint = (*it)->TransformPoint(outputPoint);
  }

  return outputVector;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformDiffusionTensor3D(
  const InputDiffusionTensor3DType & inputTensor,
  const InputPointType &             inputPoint) const -> OutputDiffusionTensor3DType
{
  OutputDiffusionTensor3DType outputTensor(inputTensor);
  OutputPointType             outputPoint(inputPoint);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputTensor = (*it)->TransformDiffusionTensor3D(outputTensor, outputPoint);
    outputPoint = (*it)->TransformPoint(outputPoint);
  }

  return outputTensor;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformDiffusionTensor3D(
  const InputVectorPixelType & inputTensor,
  const InputPointType &       inputPoint) const -> OutputVectorPixelType
{
  OutputVectorPixelType outputTensor(inputTensor);
  OutputPointType       outputPoint(inputPoint);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputTensor = (*it)->TransformDiffusionTensor3D(outputTensor, outputPoint);
    outputPoint = (*it)->TransformPoint(outputPoint);
  }

  return outputTensor;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformDiffusionTensor3D(
  const InputDiffusionTensor3DType & inputTensor) const -> OutputDiffusionTensor3DType
{
  OutputDiffusionTensor3DType outputTensor(inputTensor);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputTensor = (*it)->TransformDiffusionTensor3D(outputTensor);
  }

  return outputTensor;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformDiffusionTensor3D(
  const InputVectorPixelType & inputTensor) const -> OutputVectorPixelType
{
  OutputVectorPixelType outputTensor(inputTensor);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputTensor = (*it)->TransformDiffusionTensor3D(outputTensor);
  }

  return outputTensor;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformSymmetricSecondRankTensor(
  const InputSymmetricSecondRankTensorType & inputTensor,
  const InputPointType &                     inputPoint) const -> OutputSymmetricSecondRankTensorType
{
  OutputSymmetricSecondRankTensorType outputTensor(inputTensor);
  OutputPointType                     outputPoint(inputPoint);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputTensor = (*it)->TransformSymmetricSecondRankTensor(outputTensor, outputPoint);
    outputPoint = (*it)->TransformPoint(outputPoint);
  }

  return outputTensor;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformSymmetricSecondRankTensor(
  const InputVectorPixelType & inputTensor,
  const InputPointType &       inputPoint) const -> OutputVectorPixelType
{
  OutputVectorPixelType outputTensor(inputTensor);
  OutputPointType       outputPoint(inputPoint);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputTensor = (*it)->TransformSymmetricSecondRankTensor(outputTensor, outputPoint);
    outputPoint = (*it)->TransformPoint(outputPoint);
  }

  return outputTensor;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformSymmetricSecondRankTensor(
  const InputSymmetricSecondRankTensorType & inputTensor) const -> OutputSymmetricSecondRankTensorType
{
  OutputSymmetricSecondRankTensorType outputTensor(inputTensor);

  /* Apply in reverse queue order.  */
  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputTensor = (*it)->TransformSymmetricSecondRankTensor(outputTensor);
  }

  return outputTensor;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::TransformSymmetricSecondRankTensor(
  const InputVectorPixelType & inputTensor) const -> OutputVectorPixelType
{
  OutputVectorPixelType outputTensor(inputTensor);


  for (auto it = this->m_TransformQueue.rbegin(); it != this->m_TransformQueue.rend(); ++it)
  {
    outputTensor = (*it)->TransformSymmetricSecondRankTensor(outputTensor);
  }

  return outputTensor;
}


template <typename TParametersValueType, unsigned int VDimension>
bool
CompositeTransform<TParametersValueType, VDimension>::GetInverse(Self * inverse) const
{
  // NOTE: CompositeTransform delegates to
  //      individual transform for setting FixedParameters
  //      inverse->SetFixedParameters( this->GetFixedParameters() );
  inverse->ClearTransformQueue();
  for (auto it = this->m_TransformQueue.begin(); it != this->m_TransformQueue.end(); ++it)
  {
    const TransformTypePointer inverseTransform = ((*it)->GetInverseTransform()).GetPointer();
    if (!inverseTransform)
    {
      inverse->ClearTransformQueue();
      return false;
    }

    /* Push to front to reverse the transform order */
    inverse->PushFrontTransform(inverseTransform);
  }

  /* Copy the optimization flags */
  inverse->m_TransformsToOptimizeFlags.clear();
  for (const bool m_TransformsToOptimizeFlag : this->m_TransformsToOptimizeFlags)
  {
    inverse->m_TransformsToOptimizeFlags.push_front(m_TransformsToOptimizeFlag);
  }

  return true;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::GetInverseTransform() const -> InverseTransformBasePointer
{
  return Superclass::InvertTransform(*this);
}


template <typename TParametersValueType, unsigned int VDimension>
void
CompositeTransform<TParametersValueType, VDimension>::ComputeJacobianWithRespectToParameters(
  const InputPointType & p,
  JacobianType &         outJacobian) const
{
  /* Returns a concatenated MxN array, holding the Jacobian of each sub
   * transform that is selected for optimization. The order is the same
   * as that in which they're applied, i.e. reverse order.
   * M rows = dimensionality of the transforms
   * N cols = total number of parameters in the selected sub transforms. */
  outJacobian.SetSize(VDimension, this->GetNumberOfLocalParameters());
  JacobianType jacobianWithRespectToPosition;
  this->ComputeJacobianWithRespectToParametersCachedTemporaries(p, outJacobian, jacobianWithRespectToPosition);
}

template <typename TParametersValueType, unsigned int VDimension>
void
CompositeTransform<TParametersValueType, VDimension>::ComputeJacobianWithRespectToParametersCachedTemporaries(
  const InputPointType & p,
  JacobianType &         outJacobian,
  JacobianType &         cacheJacobian) const
{
  // NOTE: This must have been done outside of outJacobian.SetSize( VDimension, this->GetNumberOfLocalParameters() );
  assert(outJacobian.rows() == VDimension && outJacobian.cols() == this->GetNumberOfLocalParameters());

  if (this->GetNumberOfTransforms() == 1)
  {
    const TransformType * const transform = this->GetNthTransformConstPointer(0);
    transform->ComputeJacobianWithRespectToParameters(p, outJacobian);
    return;
  }

  NumberOfParametersType offset{};

  OutputPointType transformedPoint(p);

  /*
   * Composite transform $T is composed of $T0(p0,x), $T1(p1,x) and $T2(p2, x) as:
   *
   * T(p0, p1, p2, x)
   * = T0(p0, T1(p1, T2(p2, x)))
   *
   * p0, p1, p2 are the transform parameters for transform T0, T1, T2
   * respectively.
   *
   * Let p = (p0, p1, p2).
   *  x2 = T2(p2, x).
   *  x1 = T1(p1, x2).
   *
   *
   * The following loop computes dT/dp:
   *
   * dT/dp
   * = (dT/dp0, dT/dp1, dT/dp2)
   * = ( dT0/dp0 | x1 ),
   *   ( dT0/dT1 | x1 ) * ( dT1/dp1 | x2 ),
   *   ( ( dT0/dT1 | x1 ) * ( dT1/dT2 | x2 ) * ( dT2/dp2 | x )
   *
   * In the first iteration, it computes
   *   dT2/dp2 | x
   *
   * In the second iteration, it computes
   *   dT1/dp1 | x2
   *
   *  and it computes
   *   dT1/dT2 | x2, and left multiplying to  dT2/dp2 | x
   *
   * In the third iteration, it computes
   *   dT0/dp0 | x1,
   *
   *  and it computes
   *   dT0/dT1 | x1, and left multiplying to
   *    ( dT1/dT2 | x2 ) * ( dT2/dp2 | x )
   *    and ( dT1/dp1 | x2 )
   *
   */
  for (long tind = (long)this->GetNumberOfTransforms() - 1; tind >= 0; --tind)
  {
    /* Get a raw pointer for efficiency, avoiding SmartPointer register/unregister */
    const TransformType * const transform = this->GetNthTransformConstPointer(tind);

    const NumberOfParametersType offsetLast = offset;

    if (this->GetNthTransformToOptimize(tind))
    {
      /* Copy from another matrix, element-by-element */
      /* The matrices are row-major, so block copy is less obviously
       * better */

      const NumberOfParametersType numberOfLocalParameters = transform->GetNumberOfLocalParameters();

      cacheJacobian.SetSize(VDimension, numberOfLocalParameters);
      transform->ComputeJacobianWithRespectToParameters(transformedPoint, cacheJacobian);
      outJacobian.update(cacheJacobian, 0, offset);
      offset += numberOfLocalParameters;
    }

    /* The composite transform needs to compose previous jacobians
     * (those closer to the originating point) with the current
     * transform's jacobian.  We therefore update the previous
     * jacobian by multiplying the current matrix jumping over the
     * first transform. The matrix here refers to  dT/dx at the point.
     * For example, in the affine transform, this is the affine matrix.
     *
     * Also, noted the multiplication contains all the affine matrix from
     * all transforms no matter they are going to be optimized or not.
     *
     * Update every old term by left multiplying dTk / dT{k-1}
     * do this before computing the transformedPoint for the next
     * iteration.
     */
    if (offsetLast > 0)
    {
      JacobianPositionType jacobianWithRespectToPosition;
      transform->ComputeJacobianWithRespectToPosition(transformedPoint, jacobianWithRespectToPosition);

      // Perform the following matrix multiplication in-place:
      // outJacobian[0:VDimension,0:offsetLast] = jacobianWithRespectToPosition*outJacobian[0:VDimension,0:offsetLast]
      assert(jacobianWithRespectToPosition.rows() == VDimension);
      double temp[VDimension];
      for (unsigned int c = 0; c < offsetLast; ++c)
      {
        for (unsigned int r = 0; r < VDimension; ++r)
        {
          temp[r] = 0.0;
          // update_j[r][c] = 0.0;
          for (unsigned int k = 0; k < VDimension; ++k)
          {
            // update_j[r][c]
            temp[r] += jacobianWithRespectToPosition[r][k] * outJacobian[k][c];
          }
        }
        for (unsigned int r = 0; r < VDimension; ++r)
        {
          outJacobian[r][c] = temp[r];
        }
      }
    }

    /* Transform the point so it's ready for next transform's Jacobian */
    transformedPoint = transform->TransformPoint(transformedPoint);
  }
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::GetParameters() const -> const ParametersType &
{
  const TransformQueueType & transforms = this->GetTransformsToOptimizeQueue();
  if (transforms.size() == 1)
  {
    // Return directly to avoid copying. Most often we'll have only a single
    // active transform, so we'll end up here.
    return transforms[0]->GetParameters();
  }
  else
  {
    /* Resize destructively. But if it's already this size, nothing is done so
     * it's efficient. */
    this->m_Parameters.SetSize(this->GetNumberOfParameters());

    NumberOfParametersType offset{};

    for (auto it = transforms.rbegin(); it != transforms.rend(); ++it)
    {
      const ParametersType & subParameters = (*it)->GetParameters();
      /* use vnl_vector data_block() to get data ptr */
      std::copy_n(subParameters.data_block(), subParameters.Size(), &(this->m_Parameters.data_block())[offset]);
      offset += subParameters.Size();
    }
  }

  return this->m_Parameters;
}


template <typename TParametersValueType, unsigned int VDimension>
void
CompositeTransform<TParametersValueType, VDimension>::SetParameters(const ParametersType & inputParameters)
{
  /* We do not copy inputParameters into m_Parameters,
   * to avoid unnecessary copying. */

  /* Assumes input params are concatenation of the parameters of the
     sub transforms currently selected for optimization, in
     the order of the queue from begin() to end(). */
  TransformQueueType transforms = this->GetTransformsToOptimizeQueue();

  /* Verify proper input size. */
  if (inputParameters.Size() != this->GetNumberOfParameters())
  {
    itkExceptionMacro("Input parameter list size is not expected size. " << inputParameters.Size() << " instead of "
                                                                         << this->GetNumberOfParameters() << '.');
  }

  if (transforms.size() == 0)
  {
    return; // Nothing to do, no transforms.
  }

  if (transforms.size() == 1)
  {
    /* Avoid unnecessary copying. See comments below */
    if (&inputParameters == &this->m_Parameters)
    {
      transforms[0]->SetParameters(transforms[0]->GetParameters());
    }
    else
    {
      transforms[0]->SetParameters(inputParameters);
    }
  }
  else
  {
    NumberOfParametersType offset{};
    auto                   it = transforms.end();

    do
    {
      --it;
      /* If inputParams is same object as m_Parameters, we just pass
       * each sub-transforms own m_Parameters in. This is needed to
       * avoid unnecessary copying of parameters in the sub-transforms,
       * while still allowing SetParameters to do any operations on the
       * parameters to update member variable states. A hack. */
      if (&inputParameters == &this->m_Parameters)
      {
        (*it)->SetParameters((*it)->GetParameters());
      }
      else
      {
        const size_t parameterSize = (*it)->GetParameters().Size();
        (*it)->CopyInParameters(&(inputParameters.data_block())[offset],
                                &(inputParameters.data_block())[offset] + parameterSize);
        offset += static_cast<NumberOfParametersType>(parameterSize);
      }

    } while (it != transforms.begin());
  }
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::GetFixedParameters() const -> const FixedParametersType &
{
  TransformQueueType transforms = this->GetTransformsToOptimizeQueue();
  /* Resize destructively. But if it's already this size, nothing is done so
   * it's efficient. */
  this->m_FixedParameters.SetSize(this->GetNumberOfFixedParameters());

  NumberOfParametersType offset{};

  for (auto it = transforms.rbegin(); it != transforms.rend(); ++it)
  {
    const FixedParametersType & subFixedParameters = (*it)->GetFixedParameters();
    /* use vnl_vector data_block() to get data ptr */
    std::copy_n(
      subFixedParameters.data_block(), subFixedParameters.Size(), &(this->m_FixedParameters.data_block())[offset]);
    offset += subFixedParameters.Size();
  }

  return this->m_FixedParameters;
}

template <typename TParametersValueType, unsigned int VDimension>
void
CompositeTransform<TParametersValueType, VDimension>::SetFixedParameters(const FixedParametersType & inputParameters)
{
  /* Assumes input params are concatenation of the parameters of the
   * sub transforms currently selected for optimization. */
  TransformQueueType transforms = this->GetTransformsToOptimizeQueue();

  NumberOfParametersType offset{};


  /* Verify proper input size. */
  if (inputParameters.Size() != this->GetNumberOfFixedParameters())
  {
    itkExceptionMacro("Input parameter list size is not expected size. " << inputParameters.Size() << " instead of "
                                                                         << this->GetNumberOfFixedParameters() << '.');
  }
  this->m_FixedParameters = inputParameters;

  for (auto it = transforms.rbegin(); it != transforms.rend(); ++it)
  {
    const size_t fixedParameterSize = (*it)->GetFixedParameters().Size();
    (*it)->CopyInFixedParameters(&(this->m_FixedParameters.data_block())[offset],
                                 &(this->m_FixedParameters.data_block())[offset] + fixedParameterSize);
    offset += static_cast<NumberOfParametersType>(fixedParameterSize);
  }
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::GetNumberOfParameters() const -> NumberOfParametersType
{
  /* Returns to total number of params in all transforms currently
   * set to be used for optimized.
   * NOTE: We might want to optimize this only to store the result and
   * only re-calc when the composite object has been modified.
   * However, it seems that number of parameter might change for dense
   * field transforms (deformation, bspline) during processing and
   * we wouldn't know that in this class, so this is safest. */
  NumberOfParametersType result{};


  for (long tind = (long)this->GetNumberOfTransforms() - 1; tind >= 0; tind--)
  {
    if (this->GetNthTransformToOptimize(tind))
    {
      const TransformType * transform = this->GetNthTransformConstPointer(tind);
      result += transform->GetNumberOfParameters();
    }
  }
  return result;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::GetNumberOfLocalParameters() const -> NumberOfParametersType
{
  if (this->GetMTime() == this->m_LocalParametersUpdateTime)
  {
    return this->m_NumberOfLocalParameters;
  }

  /* Returns to total number of *local* params in all transforms currently
   * set to be used for optimized.
   * Note that unlike in GetNumberOfParameters(), we don't expect the
   * number of local parameters to possibly change. */
  NumberOfParametersType result{};
  for (long tind = (long)this->GetNumberOfTransforms() - 1; tind >= 0; tind--)
  {
    if (this->GetNthTransformToOptimize(tind))
    {
      const TransformType * transform = this->GetNthTransformConstPointer(tind);
      result += transform->GetNumberOfLocalParameters();
    }
  }
  this->m_NumberOfLocalParameters = result;

  this->m_LocalParametersUpdateTime = this->GetMTime();

  return result;
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::GetNumberOfFixedParameters() const -> NumberOfParametersType
{
  /* Returns to total number of params in all transforms currently
   * set to be used for optimized.
   * NOTE: We might want to optimize this only to store the result and
   * only re-calc when the composite object has been modified. */
  NumberOfParametersType result{};

  for (long tind = (long)this->GetNumberOfTransforms() - 1; tind >= 0; tind--)
  {
    if (this->GetNthTransformToOptimize(tind))
    {
      const TransformType * transform = this->GetNthTransformConstPointer(tind);
      result += transform->GetFixedParameters().Size();
    }
  }
  return result;
}


template <typename TParametersValueType, unsigned int VDimension>
void
CompositeTransform<TParametersValueType, VDimension>::UpdateTransformParameters(const DerivativeType & update,
                                                                                ScalarType             factor)
{
  /* Update parameters within the sub-transforms set to be optimized. */
  /* NOTE: We might want to thread this over each sub-transform, if we
   * find we're working with longer lists of sub-transforms that do
   * not implement any threading of their own for UpdateTransformParameters.
   * Since the plan is for an UpdateTransformParameters functor that is
   * user-assignable, we would need a method in the
   * functor to return whether or not it does threading. If all sub-transforms
   * return that they don't thread, we could do each sub-transform in its
   * own thread from here. */
  const NumberOfParametersType numberOfParameters = this->GetNumberOfParameters();

  if (update.Size() != numberOfParameters)
  {
    itkExceptionMacro("Parameter update size, " << update.Size()
                                                << ", must "
                                                   " be same as transform parameter size, "
                                                << numberOfParameters << std::endl);
  }

  NumberOfParametersType offset{};


  for (long tind = (long)this->GetNumberOfTransforms() - 1; tind >= 0; tind--)
  {
    if (this->GetNthTransformToOptimize(tind))
    {
      TransformType * subtransform = this->GetNthTransformModifiablePointer(tind);
      /* The input values are in a monolithic block, so we have to point
       * to the subregion corresponding to the individual subtransform.
       * This simply creates an Array object with data pointer, no
       * memory is allocated or copied.
       * NOTE: the use of const_cast is used to avoid a deep copy in the underlying vnl_vector
       * by using LetArrayManageMemory=false, and being very careful here we can
       * ensure that casting away const-ness does not result in memory corruption. */
      auto * nonConstDataRefForPerformance =
        const_cast<typename DerivativeType::ValueType *>(&((update.data_block())[offset]));
      const DerivativeType subUpdate(nonConstDataRefForPerformance, subtransform->GetNumberOfParameters(), false);
      /* This call will also call SetParameters, so don't need to call it
       * explicitly here. */
      subtransform->UpdateTransformParameters(subUpdate, factor);
      offset += subtransform->GetNumberOfParameters();
    }
  }
  this->Modified();
}


template <typename TParametersValueType, unsigned int VDimension>
auto
CompositeTransform<TParametersValueType, VDimension>::GetTransformsToOptimizeQueue() const -> const TransformQueueType &
{
  /* Update the list of transforms to use for optimization only if
   the selection of transforms to optimize may have changed */
  if (this->GetMTime() > this->m_PreviousTransformsToOptimizeUpdateTime)
  {
    this->m_TransformsToOptimizeQueue.clear();
    for (size_t n = 0; n < this->m_TransformQueue.size(); ++n)
    {
      /* Return them in the same order as they're found in the main list */
      if (this->GetNthTransformToOptimize(static_cast<SizeValueType>(n)))
      {
        this->m_TransformsToOptimizeQueue.push_back(
          this->GetNthTransformModifiablePointer(static_cast<SizeValueType>(n)));
      }
    }
    this->m_PreviousTransformsToOptimizeUpdateTime = this->GetMTime();
  }
  return this->m_TransformsToOptimizeQueue;
}


template <typename TParametersValueType, unsigned int VDimension>
void
CompositeTransform<TParametersValueType, VDimension>::FlattenTransformQueue()
{
  TransformQueueType            transformQueue;
  TransformQueueType            transformsToOptimizeQueue;
  TransformsToOptimizeFlagsType transformsToOptimizeFlags;

  for (SizeValueType m = 0; m < this->GetNumberOfTransforms(); ++m)
  {
    auto * nestedCompositeTransform = dynamic_cast<Self *>(this->m_TransformQueue[m].GetPointer());
    if (nestedCompositeTransform)
    {
      nestedCompositeTransform->FlattenTransformQueue();
      for (SizeValueType n = 0; n < nestedCompositeTransform->GetNumberOfTransforms(); ++n)
      {
        transformQueue.push_back(nestedCompositeTransform->GetNthTransformModifiablePointer(n));
        if (nestedCompositeTransform->GetNthTransformToOptimize(n))
        {
          transformsToOptimizeFlags.push_back(true);
          transformsToOptimizeQueue.push_back(nestedCompositeTransform->GetNthTransformModifiablePointer(n));
        }
        else
        {
          transformsToOptimizeFlags.push_back(false);
        }
      }
    }
    else
    {
      transformQueue.push_back(this->m_TransformQueue[m]);
      if (this->m_TransformsToOptimizeFlags[m])
      {
        transformsToOptimizeFlags.push_back(true);
        transformsToOptimizeQueue.push_back(this->m_TransformQueue[m]);
      }
      else
      {
        transformsToOptimizeFlags.push_back(false);
      }
    }
  }

  this->m_TransformQueue = transformQueue;
  this->m_TransformsToOptimizeQueue = transformsToOptimizeQueue;
  this->m_TransformsToOptimizeFlags = transformsToOptimizeFlags;
}


template <typename TParametersValueType, unsigned int VDimension>
void
CompositeTransform<TParametersValueType, VDimension>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "TransformsToOptimizeFlags: " << std::endl << indent << indent;
  for (const bool m_TransformsToOptimizeFlag : m_TransformsToOptimizeFlags)
  {
    os << indent.GetNextIndent() << m_TransformsToOptimizeFlag << ' ';
  }
  os << std::endl;

  os << indent << "TransformsToOptimizeQueue: " << std::endl;
  for (auto cit = m_TransformsToOptimizeQueue.begin(); cit != m_TransformsToOptimizeQueue.end(); ++cit)
  {
    (*cit)->Print(os, indent.GetNextIndent());
    os << std::endl;
  }

  os << indent << "PreviousTransformsToOptimizeUpdateTime: "
     << static_cast<typename NumericTraits<ModifiedTimeType>::PrintType>(m_PreviousTransformsToOptimizeUpdateTime)
     << std::endl;
}


template <typename TParametersValueType, unsigned int VDimension>
typename LightObject::Pointer
CompositeTransform<TParametersValueType, VDimension>::InternalClone() const
{
  // This class doesn't use its superclass implementation
  // TODO: is it really the right behavior?
  // LightObject::Pointer loPtr = Superclass::InternalClone();

  LightObject::Pointer         loPtr = CreateAnother();
  const typename Self::Pointer clone = dynamic_cast<Self *>(loPtr.GetPointer());
  if (clone.IsNull())
  {
    itkExceptionMacro("downcast to type " << this->GetNameOfClass() << " failed.");
  }

  auto tqIt = this->m_TransformQueue.begin();

  auto tfIt = this->m_TransformsToOptimizeFlags.begin();

  for (int i = 0; tqIt != this->m_TransformQueue.end() && tfIt != this->m_TransformsToOptimizeFlags.end();
       ++tqIt, ++tfIt, ++i)
  {
    clone->AddTransform((*tqIt)->Clone().GetPointer());
    clone->SetNthTransformToOptimize(i, (*tfIt));
  }
  return loPtr;
}

} // end namespace itk

#endif
