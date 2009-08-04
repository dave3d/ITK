/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkShapeKeepNObjectsLabelMapFilter.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkShapeKeepNObjectsLabelMapFilter_txx
#define __itkShapeKeepNObjectsLabelMapFilter_txx

#include "itkShapeKeepNObjectsLabelMapFilter.h"
#include "itkLabelObjectAccessors.h"
#include "itkProgressReporter.h"
#include "itkShapeLabelObject.h"
#include "itkShapeLabelObjectAccessors.h"

namespace itk {

template <class TImage>
ShapeKeepNObjectsLabelMapFilter<TImage>
::ShapeKeepNObjectsLabelMapFilter()
{
  m_ReverseOrdering = false;
  m_NumberOfObjects = 1;
  m_Attribute = LabelObjectType::SIZE;

  // create the output image for the removed objects
  this->SetNumberOfRequiredOutputs(2);
  this->SetNthOutput(1, static_cast<TImage*>(this->MakeOutput(1).GetPointer()));
}


template <class TImage>
void
ShapeKeepNObjectsLabelMapFilter<TImage>
::GenerateData()
{
  switch( m_Attribute )
    {
    case LabelObjectType::LABEL:
      {
      typedef typename Functor::LabelLabelObjectAccessor< LabelObjectType >   FunctorType;
      FunctorType functor;
      this->TemplatedGenerateData( functor );
      break;
      }
    case LabelObjectType::SIZE:
      {
      typedef typename Functor::SizeLabelObjectAccessor< LabelObjectType > FunctorType;
      FunctorType functor;
      this->TemplatedGenerateData( functor );
      break;
      }
    case LabelObjectType::PHYSICAL_SIZE:
      {
      typedef typename Functor::PhysicalSizeLabelObjectAccessor< LabelObjectType > FunctorType;
      FunctorType functor;
      this->TemplatedGenerateData( functor );
      break;
      }
    case LabelObjectType::SIZE_REGION_RATIO:
      {
      typedef typename Functor::SizeRegionRatioLabelObjectAccessor< LabelObjectType > FunctorType;
      FunctorType functor;
      this->TemplatedGenerateData( functor );
      break;
      }
    case LabelObjectType::REGION_ELONGATION:
      {
      typedef typename Functor::RegionElongationLabelObjectAccessor< LabelObjectType > FunctorType;
      FunctorType functor;
      this->TemplatedGenerateData( functor );
      break;
      }
    case LabelObjectType::SIZE_ON_BORDER:
      {
      typedef typename Functor::SizeOnBorderLabelObjectAccessor< LabelObjectType > FunctorType;
      FunctorType functor;
      this->TemplatedGenerateData( functor );
      break;
      }
    case LabelObjectType::PHYSICAL_SIZE_ON_BORDER:
      {
      typedef typename Functor::PhysicalSizeOnBorderLabelObjectAccessor< LabelObjectType > FunctorType;
      FunctorType functor;
      this->TemplatedGenerateData( functor );
      break;
      }
    case LabelObjectType::FERET_DIAMETER:
      {
      typedef typename Functor::FeretDiameterLabelObjectAccessor< LabelObjectType > FunctorType;
      FunctorType functor;
      this->TemplatedGenerateData( functor );
      break;
      }
    case LabelObjectType::BINARY_ELONGATION:
      {
      typedef typename Functor::BinaryElongationLabelObjectAccessor< LabelObjectType > FunctorType;
      FunctorType functor;
      this->TemplatedGenerateData( functor );
      break;
      }
    case LabelObjectType::PERIMETER:
      {
      typedef typename Functor::PerimeterLabelObjectAccessor< LabelObjectType > FunctorType;
      FunctorType functor;
      this->TemplatedGenerateData( functor );
      break;
      }
    case LabelObjectType::ROUNDNESS:
      {
      typedef typename Functor::RoundnessLabelObjectAccessor< LabelObjectType > FunctorType;
      FunctorType functor;
      this->TemplatedGenerateData( functor );
      break;
      }
    case LabelObjectType::EQUIVALENT_RADIUS:
      {
      typedef typename Functor::EquivalentRadiusLabelObjectAccessor< LabelObjectType > FunctorType;
      FunctorType functor;
      this->TemplatedGenerateData( functor );
      break;
      }
    case LabelObjectType::EQUIVALENT_PERIMETER:
      {
      typedef typename Functor::EquivalentPerimeterLabelObjectAccessor< LabelObjectType > FunctorType;
      FunctorType functor;
      this->TemplatedGenerateData( functor );
      break;
      }
    case LabelObjectType::BINARY_FLATNESS:
      {
      typedef typename Functor::BinaryFlatnessLabelObjectAccessor< LabelObjectType > FunctorType;
      FunctorType functor;
      this->TemplatedGenerateData( functor );
      break;
      }
    default:
      itkExceptionMacro(<< "Unknown attribute type");
      break;
    }
}


template <class TImage>
template <class TAttributeAccessor>
void
ShapeKeepNObjectsLabelMapFilter<TImage>
::TemplatedGenerateData( const TAttributeAccessor & itkNotUsed( accessor ) )
{
  // Allocate the output
  this->AllocateOutputs();

  ImageType * output = this->GetOutput();
  ImageType * output2 = this->GetOutput( 1 );

  // set the background value for the second output - this is not done in the superclasses
  output2->SetBackgroundValue( output->GetBackgroundValue() );

  typedef typename ImageType::LabelObjectContainerType LabelObjectContainerType;
  const LabelObjectContainerType & labelObjectContainer = output->GetLabelObjectContainer();
  typedef typename std::vector< typename LabelObjectType::Pointer > VectorType;

  ProgressReporter progress( this, 0, 2 * labelObjectContainer.size() );

  // get the label objects in a vector, so they can be sorted
  VectorType labelObjects;
  labelObjects.reserve( labelObjectContainer.size() );
  typename LabelObjectContainerType::const_iterator it = labelObjectContainer.begin();
  while( it != labelObjectContainer.end() )
    {
    labelObjects.push_back( it->second );
    progress.CompletedPixel();
    it++;
    }

  // instantiate the comparator and sort the vector
  if( m_NumberOfObjects < labelObjectContainer.size() )
    {
    typename VectorType::iterator end = labelObjects.begin() + m_NumberOfObjects;
    if( m_ReverseOrdering )
      {
      Functor::LabelObjectReverseComparator< LabelObjectType, TAttributeAccessor > comparator;
      std::nth_element( labelObjects.begin(), end, labelObjects.end(), comparator );
      }
    else
      {
      Functor::LabelObjectComparator< LabelObjectType, TAttributeAccessor > comparator;
      std::nth_element( labelObjects.begin(), end, labelObjects.end(), comparator );
      }
    progress.CompletedPixel();
  
    // and remove the last objects of the map
    for( typename VectorType::const_iterator it = end;
      it != labelObjects.end();
      it++ )
      {
      output2->AddLabelObject( *it );
      output->RemoveLabelObject( *it );
      progress.CompletedPixel();
      }
    }
}


template <class TImage>
void
ShapeKeepNObjectsLabelMapFilter<TImage>
::PrintSelf( std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "ReverseOrdering: "  << m_ReverseOrdering << std::endl;
  os << indent << "NumberOfObjects: "  << m_NumberOfObjects << std::endl;
  os << indent << "Attribute: "  << LabelObjectType::GetNameFromAttribute( m_Attribute ) << " (" << m_Attribute << ")" << std::endl;
}

}// end namespace itk
#endif
