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
#ifndef itkSphereMeshSource_hxx
#define itkSphereMeshSource_hxx

#include "itkIntTypes.h"

namespace itk
{
/**
 *
 */
template <typename TOutputMesh>
SphereMeshSource<TOutputMesh>::SphereMeshSource()
  : m_ResolutionX(4)
  , m_ResolutionY(4)
  , m_Squareness1(1.0)
  , m_Squareness2(1.0)
{
  /**
   * Create the output
   */
  auto output = TOutputMesh::New();
  this->ProcessObject::SetNumberOfRequiredOutputs(1);
  this->ProcessObject::SetNthOutput(0, output.GetPointer());
  m_Center.Fill(0);
  m_Scale.Fill(1);
}

/*
 *
 */
template <typename TOutputMesh>
void
SphereMeshSource<TOutputMesh>::GenerateData()
{
  // calculate the number os cells and points
  const IdentifierType numpts = m_ResolutionX * m_ResolutionY + 2;

  // calculate the steps using resolution
  const double ustep = itk::Math::pi / (m_ResolutionX + 1);
  const double vstep = 2.0 * itk::Math::pi / m_ResolutionY;
  const double ubeg = (-itk::Math::pi / 2.0) + ustep;
  const double vbeg = -itk::Math::pi;

  ///////////////////////////////////////////////////////////////////////////
  // nodes allocation

  // the temporary container of nodes' connectedness
  typename OutputMeshType::PointIdentifier tripoints[3] = { 0, 1, 2 };

  // memory allocation for nodes
  const typename OutputMeshType::Pointer outputMesh = this->GetOutput();

  outputMesh->GetPoints()->Reserve(numpts);

  outputMesh->SetCellsAllocationMethod(MeshEnums::MeshClassCellsAllocationMethod::CellsAllocatedDynamicallyCellByCell);

  const PointsContainerPointer       myPoints = outputMesh->GetPoints();
  typename PointsContainer::Iterator point = myPoints->Begin();

  OPointType p1;

  // calculate all regular nodes
  while (point != myPoints->End())
  {
    double u = ubeg;
    for (IdentifierType i = 0; i < m_ResolutionX; u += ustep, i++)
    {
      double v = vbeg;
      for (IdentifierType j = 0; j < m_ResolutionY; v += vstep, j++)
      {
        int signu = (std::cos(u) > 0) ? 1 : -1;
        int signv = (std::cos(v) > 0) ? 1 : -1;

        p1[0] = m_Scale[0] * signu *
                  (std::pow(static_cast<float>(itk::Math::abs(std::cos(u))), static_cast<float>(m_Squareness1)))*signv *
                  (std::pow(static_cast<float>(itk::Math::abs(std::cos(v))), static_cast<float>(m_Squareness2))) +
                m_Center[0];

        signv = (std::sin(v) > 0) ? 1 : -1;

        p1[1] = m_Scale[1] * signu *
                  (std::pow(static_cast<float>(itk::Math::abs(std::cos(u))), static_cast<float>(m_Squareness1)))*signv *
                  (std::pow(static_cast<float>(itk::Math::abs(std::sin(v))), static_cast<float>(m_Squareness2))) +
                m_Center[1];

        signu = (std::sin(u) > 0) ? 1 : -1;
        p1[2] = m_Scale[2] * signu *
                  (std::pow(static_cast<float>(itk::Math::abs(std::sin(u))), static_cast<float>(m_Squareness1))) +
                m_Center[2];

        point.Value() = p1;
        ++point;
      }
    }

    // calculate the south pole node
    p1[0] = (m_Scale[0] * (std::pow(static_cast<float>(itk::Math::abs(std::cos(-itk::Math::pi / 2))), 1.0f)) *
               (std::pow(static_cast<float>(itk::Math::abs(std::cos(0.0))), 1.0f)) +
             m_Center[0]);
    p1[1] = (m_Scale[1] * (std::pow(static_cast<float>(itk::Math::abs(std::cos(-itk::Math::pi / 2))), 1.0f)) *
               (std::pow(static_cast<float>(itk::Math::abs(std::sin(0.0))), 1.0f)) +
             m_Center[1]);
    p1[2] = (m_Scale[2] * -1 * (std::pow(static_cast<float>(itk::Math::abs(std::sin(-itk::Math::pi / 2))), 1.0f)) +
             m_Center[2]);
    point.Value() = p1;
    ++point;

    // calculate the north pole node
    p1[0] = (m_Scale[0] * (std::pow(static_cast<float>(itk::Math::abs(std::cos(itk::Math::pi / 2))), 1.0f)) *
               (std::pow(itk::Math::abs(std::cos(0.0)), 1.0)) +
             m_Center[0]);
    p1[1] = (m_Scale[1] * (std::pow(static_cast<float>(itk::Math::abs(std::cos(itk::Math::pi / 2))), 1.0f)) *
               (std::pow(itk::Math::abs(std::sin(0.0)), 1.0)) +
             m_Center[1]);
    p1[2] =
      (m_Scale[2] * (std::pow(static_cast<float>(itk::Math::abs(std::sin(itk::Math::pi / 2))), 1.0f)) + m_Center[2]);
    point.Value() = p1;
    ++point;
  }

  ///////////////////////////////////////////////////////////////////////////
  // cells allocation

  IdentifierType p = 0;

  // store all regular cells
  CellAutoPointer testCell;
  for (unsigned int ii = 0; ii + 1 < m_ResolutionX; ++ii)
  {
    for (unsigned int jj = 0; jj < m_ResolutionY; ++jj)
    {
      const IdentifierType jn = (jj + 1) % m_ResolutionY;
      tripoints[0] = ii * m_ResolutionY + jj;
      tripoints[1] = tripoints[0] - jj + jn;
      tripoints[2] = tripoints[0] + m_ResolutionY;
      testCell.TakeOwnership(new TriCellType);
      testCell->SetPointIds(tripoints);
      outputMesh->SetCell(p, testCell);
      outputMesh->SetCellData(p, (OPixelType)3.0);
      ++p;
      testCell.TakeOwnership(new TriCellType);
      tripoints[0] = tripoints[1];
      tripoints[1] = tripoints[0] + m_ResolutionY;
      testCell->SetPointIds(tripoints);
      outputMesh->SetCell(p, testCell);
      outputMesh->SetCellData(p, (OPixelType)3.0);
      ++p;
    }
  }

  // store cells containing the south pole nodes
  for (unsigned int jj = 0; jj < m_ResolutionY; ++jj)
  {
    const IdentifierType jn = (jj + 1) % m_ResolutionY;
    tripoints[0] = numpts - 2;
    tripoints[1] = jn;
    tripoints[2] = jj;
    testCell.TakeOwnership(new TriCellType);
    testCell->SetPointIds(tripoints);
    outputMesh->SetCell(p, testCell);
    outputMesh->SetCellData(p, (OPixelType)1.0);
    ++p;
  }

  // store cells containing the north pole nodes
  for (unsigned int jj = 0; jj < m_ResolutionY; ++jj)
  {
    const IdentifierType jn = (jj + 1) % m_ResolutionY;
    tripoints[2] = (m_ResolutionX - 1) * m_ResolutionY + jj;
    tripoints[1] = numpts - 1;
    tripoints[0] = tripoints[2] - jj + jn;
    testCell.TakeOwnership(new TriCellType);
    testCell->SetPointIds(tripoints);
    outputMesh->SetCell(p, testCell);
    outputMesh->SetCellData(p, (OPixelType)2.0);
    ++p;
  }
}

template <typename TOutputMesh>
void
SphereMeshSource<TOutputMesh>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Center: " << m_Center << std::endl;
  os << indent << "Scale: " << m_Scale << std::endl;
  os << indent << "ResolutionX: " << m_ResolutionX << std::endl;
  os << indent << "ResolutionX: " << m_ResolutionY << std::endl;
  os << indent << "Squareness1: " << m_Squareness1 << std::endl;
  os << indent << "Squareness2: " << m_Squareness2 << std::endl;
}
} // end namespace itk

#endif
