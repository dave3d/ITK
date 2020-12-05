/*=========================================================================
 *
 *  Copyright NumFOCUS
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
#include "itkMemoryProbe.h"

namespace itk
{
MemoryProbe::MemoryProbe()
  : ResourceProbe<MemoryProbe::MemoryLoadType, double>("Memory", "kB")
{}

MemoryProbe ::~MemoryProbe() = default;

MemoryProbe::MemoryLoadType
MemoryProbe::GetInstantValue() const
{
  return static_cast<MemoryProbe::MemoryLoadType>(m_MemoryObserver.GetMemoryUsage());
}
} // end namespace itk
