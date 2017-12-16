/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkSpatialObjectDuplicator_h
#define itkSpatialObjectDuplicator_h

#include "itkObject.h"
#include "itkSpatialObject.h"

namespace itk
{
/** \class SpatialObjectDuplicator
 *  This helper class create an SpatialObject which is perfect
 *  copy of the input SpatialObject
 * \ingroup ITKSpatialObjects
 */
template< typename TInputSpatialObject >
class ITK_TEMPLATE_EXPORT SpatialObjectDuplicator:public Object
{
public:
  /** Standard class typedefs. */
  typedef SpatialObjectDuplicator    Self;
  typedef Object                     Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(SpatialObjectDuplicator, Object);

  /** Type definitions for the input SpatialObject. */
  typedef TInputSpatialObject                        SpatialObjectType;
  typedef typename TInputSpatialObject::Pointer      SpatialObjectPointer;
  typedef typename TInputSpatialObject::ConstPointer SpatialObjectConstPointer;

  itkStaticConstMacro(ObjectDimension, unsigned int,
                      SpatialObjectType::ObjectDimension);

  typedef SpatialObject< itkGetStaticConstMacro(ObjectDimension) >
  InternalSpatialObjectType;

  /** Get/Set the input SpatialObject. */
  itkSetConstObjectMacro(Input, SpatialObjectType);

  /**
    * Provide an interface to match that
    * of other ProcessObjects
    * for this source generation object
    * by returning a non-const pointer
    * for the generated Object.
    */
  //NOTE:  The m_GeneratedImageSource is only
  //       exposed via the Source generation interface
  //       by the GetOutput() method that mimics
  //       a process object.
  virtual const SpatialObjectType * GetOutput () const { return this->m_DuplicateSpatialObject.GetPointer(); }
  virtual SpatialObjectType * GetOutput() { return this->m_DuplicateSpatialObject.GetPointer(); }

#if !defined(ITK_LEGACY_REMOVE)
  // This interface was exposed in ITKv4 when the itkGetModifiableObjectMacro was used
  virtual SpatialObjectType * GetModifiedOutput() { return this->m_DuplicateSpatialObject.GetPointer(); }
#endif


  /** Compute of the input SpatialObject. */
  void Update();

protected:
  SpatialObjectDuplicator();
  ~SpatialObjectDuplicator() ITK_OVERRIDE {}
  void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;

  /** Recursive function to copy the objects */
  void CopyObject(const InternalSpatialObjectType *source,
                  InternalSpatialObjectType *destination);

  /** If the pointer is null, prints classes in SpatialObjectFactoryBase */
  void WarnAndPrintFactories(const std::string& spatialObjectType) const;

private:
  ITK_DISALLOW_COPY_AND_ASSIGN(SpatialObjectDuplicator);

  SpatialObjectConstPointer m_Input;
  SpatialObjectPointer      m_DuplicateSpatialObject;
  ModifiedTimeType          m_InternalSpatialObjectTime;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSpatialObjectDuplicator.hxx"
#endif

#endif /* itkSpatialObjectDuplicator_h */
