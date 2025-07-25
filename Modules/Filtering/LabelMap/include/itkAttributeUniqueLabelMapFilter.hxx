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
#ifndef itkAttributeUniqueLabelMapFilter_hxx
#define itkAttributeUniqueLabelMapFilter_hxx

#include "itkProgressReporter.h"
#include <queue>

namespace itk
{

template <typename TImage, typename TAttributeAccessor>
AttributeUniqueLabelMapFilter<TImage, TAttributeAccessor>::AttributeUniqueLabelMapFilter() = default;


template <typename TImage, typename TAttributeAccessor>
void
AttributeUniqueLabelMapFilter<TImage, TAttributeAccessor>::GenerateData()
{
  // Allocate the output
  this->AllocateOutputs();

  // the priority queue to store all the lines of all the objects sorted
  using PriorityQueueType =
    typename std::priority_queue<LineOfLabelObject, std::vector<LineOfLabelObject>, LineOfLabelObjectComparator>;
  PriorityQueueType pq;

  auto           labelMap = this->GetLabelMap();
  IdentifierType numberOfLines = 0;

  ProgressReporter progress(this, 0, labelMap->GetNumberOfLabelObjects(), 100, 0.0f, 0.3f);

  for (typename ImageType::Iterator it2(labelMap); !it2.IsAtEnd(); ++it2)
  {
    LabelObjectType * lo = it2.GetLabelObject();

    // may reduce the number of lines to proceed
    lo->Optimize();

    typename LabelObjectType::ConstLineIterator lit(lo);
    while (!lit.IsAtEnd())
    {
      pq.push(LineOfLabelObject(lit.GetLine(), lo));
      ++numberOfLines;
      ++lit;
    }

    // clear the lines to read them later
    lo->Clear();

    // go to the next label
    progress.CompletedPixel();
  }

  if (pq.empty())
  {
    // nothing to do
    return;
  }

  ProgressReporter progress2(this, 0, numberOfLines, 100, 0.3f, 0.60f);

  using LinesType = typename std::deque<LineOfLabelObject>;
  LinesType lines;

  lines.push_back(pq.top());
  LineOfLabelObject prev = lines.back();
  IndexType         prevIdx = prev.line.GetIndex();
  pq.pop();

  const AttributeAccessorType accessor;

  while (!pq.empty())
  {
    LineOfLabelObject l = pq.top();
    IndexType         idx = l.line.GetIndex();
    // NOTE: VS 7,8,9 seem to contain a bug where if the next line is
    // accessed with l.labelObject, the results will be erroneous. I
    // have not been able to find any reason for this.
    //
    // EXERCISE EXTREME CAUTION WHEN EDITING THE NEXT 2 LINES
    const typename LabelObjectType::LabelType             lLabel = pq.top().labelObject->GetLabel();
    const typename TAttributeAccessor::AttributeValueType attr = accessor(l.labelObject);
    pq.pop();

    bool newMainLine = false;
    // don't check dim 0!
    for (unsigned int i = 1; i < ImageDimension; ++i)
    {
      if (idx[i] != prevIdx[i])
      {
        newMainLine = true;
      }
    }

    assert(newMainLine || (idx[0] >= prevIdx[0]));

    if (newMainLine)
    {
      // just push the line
      lines.push_back(l);
    }
    else
    {
      OffsetValueType       prevLength = prev.line.GetLength();
      const OffsetValueType length = l.line.GetLength();

      if (prevIdx[0] + prevLength > idx[0])
      {
        // the lines are overlapping. We need to choose which line to keep.
        // the label, the only "attribute" to be guaranteed to be unique, is
        // used to choose
        // which line to keep. This is necessary to avoid the case where a
        // part of a label is over
        // a second label, and below in another part of the image.
        bool                                                  keepCurrent = false;
        const typename TAttributeAccessor::AttributeValueType prevAttr = accessor(prev.labelObject);
        const typename LabelObjectType::LabelType             prevLabel = prev.labelObject->GetLabel();
        // this may be changed to a single boolean expression, but may become
        // quite difficult to read
        if (attr == prevAttr)
        {
          if (lLabel > prevLabel)
          {
            keepCurrent = !m_ReverseOrdering;
          }
          else
          {
            keepCurrent = m_ReverseOrdering;
          }
        }
        else
        {
          if (attr > prevAttr)
          {
            keepCurrent = !m_ReverseOrdering;
          }
          else
          {
            keepCurrent = m_ReverseOrdering;
          }
        }

        if (keepCurrent)
        {
          // keep the current one. We must truncate the previous one to remove
          // the
          // overlap, and take care of the end of the previous line if it
          // extends
          // after the current one.
          if (prevIdx[0] + prevLength > idx[0] + length)
          {
            // the previous line is longer than the current one. Lets take its
            // tail and
            // add it to the priority queue
            IndexType newIdx = idx;
            newIdx[0] = idx[0] + length;
            const OffsetValueType newLength = prevIdx[0] + prevLength - newIdx[0];
            pq.push(LineOfLabelObject(LineType(newIdx, newLength), prev.labelObject));
          }
          // truncate the previous line to let some place for the current one
          prevLength = idx[0] - prevIdx[0];
          if (prevLength != 0)
          {
            lines.back().line.SetLength(idx[0] - prevIdx[0]);
          }
          else
          {
            // length is 0 - no need to keep that line
            lines.pop_back();
          }
          // and push the current one
          lines.push_back(l);
        }
        else
        {
          // keep the previous one. If the previous line fully overlap the
          // current one,
          // the current one is fully discarded.
          if (prevIdx[0] + prevLength >= idx[0] + length)
          {
            // discarding the current line - just do nothing
          }
          else
          {
            IndexType newIdx = idx;
            newIdx[0] = prevIdx[0] + prevLength;
            const OffsetValueType newLength = idx[0] + length - newIdx[0];

            if (newLength > 0)
            {
              l.line.SetIndex(newIdx);
              l.line.SetLength(newLength);
              // The front of this line is trimmed, it may occur after a line in the queue
              // so the queue is used for the proper ordering.
              pq.push(l);
            }
          }
        }
      }
      else
      {
        // no overlap - things are just fine already
        lines.push_back(l);
      }
    }

    // store the current line as the previous one, and go to the next one.
    prev = lines.back();
    prevIdx = prev.line.GetIndex();

    --numberOfLines;
    progress2.CompletedPixel();
  }

  // put the lines in their object
  for (unsigned int i = 0; i < lines.size(); ++i)
  {
    LineOfLabelObject & l = lines[i];
    l.labelObject->AddLine(l.line);
  }

  this->UpdateProgress(0.95f);

  // remove objects without lines
  typename ImageType::Iterator it(this->GetLabelMap());
  while (it.IsAtEnd())
  {
    const typename LabelObjectType::LabelType label = it.GetLabel();
    LabelObjectType *                         labelObject = it.GetLabelObject();

    if (labelObject->Empty())
    {
      // must increment the iterator before removing the object to avoid
      // invalidating the iterator
      ++it;
      this->GetLabelMap()->RemoveLabel(label);
    }
    else
    {
      ++it;
    }
  }

  this->UpdateProgress(1.0f);
}


template <typename TImage, typename TAttributeAccessor>
void
AttributeUniqueLabelMapFilter<TImage, TAttributeAccessor>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "ReverseOrdering: " << m_ReverseOrdering << std::endl;
}

} // end namespace itk
#endif
