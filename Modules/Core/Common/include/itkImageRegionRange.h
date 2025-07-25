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

#ifndef itkImageRegionRange_h
#define itkImageRegionRange_h

#include <algorithm> // For any_of and copy_n.
#include <cassert>
#include <cstddef>     // For ptrdiff_t.
#include <iterator>    // For bidirectional_iterator_tag.
#include <functional>  // For multiplies.
#include <numeric>     // For accumulate.
#include <type_traits> // For conditional, enable_if, and is_const.

#include "itkImageHelper.h"
#include "itkImageRegion.h"
#include "itkImageBufferRange.h"

namespace itk
{

/**
 * \class ImageRegionRange
 * Modern C++11 range to iterate over the pixels of an image region.
 * Designed to conform to Standard C++ Iterator requirements,
 * so that it can be used in range-based for loop, and passed to
 * Standard C++ algorithms.
 *
 * The following example adds 42 to each pixel, using a range-based for loop:
   \code
   ImageRegionRange range{ *image, imageRegion };

   for (auto&& pixel : range)
   {
     pixel = pixel + 42;
   }
   \endcode
 *
 * The following example prints the values of the pixels:
   \code
   for (const auto pixel : range)
   {
     std::cout << pixel << std::endl;
   }
   \endcode
 *
 * \author Niels Dekker, LKEB, Leiden University Medical Center
 *
 * \see ImageRegionIterator
 * \see ImageRegionIndexRange
 * \see ImageBufferRange
 * \see ShapedImageNeighborhoodRange
 * \ingroup ImageIterators
 * \ingroup ITKCommon
 */
template <typename TImage>
class ImageRegionRange final
{
private:
  using Self = ImageRegionRange;
  using ImageDimensionType = typename TImage::ImageDimensionType;
  using PixelType = typename TImage::PixelType;

  static constexpr bool               IsImageTypeConst = std::is_const_v<TImage>;
  static constexpr ImageDimensionType ImageDimension = TImage::ImageDimension;

  using BufferIteratorType = typename ImageBufferRange<TImage>::iterator;
  using RegionType = typename TImage::RegionType;
  using SizeType = typename TImage::SizeType;
  using OffsetType = typename TImage::OffsetType;
  using OffsetTableType = Offset<ImageDimension + 1>;
  using IndexType = typename RegionType::IndexType;


  /**
   * \class QualifiedIterator
   * Iterator class that is either 'const' or non-const qualified.
   * A non-const qualified instantiation of this template allows the pixel that
   * it points to, to be modified. A const qualified instantiation does not.
   *
   * \note The definition of this class is private. Please use its type alias
   * ImageRegionRange::iterator, or ImageRegionRange::const_iterator!
   * \see ImageRegionRange
   * \ingroup ImageIterators
   * \ingroup ITKCommon
   */
  template <bool VIsConst>
  class QualifiedIterator final
  {
  private:
    // Const and non-const iterators are friends, in order to implement the
    // constructor that allow conversion from non-const to const iterator.
    friend class QualifiedIterator<!VIsConst>;

    // ImageRegionRange is a friend, as it should be the only one that can
    // directly use the private constructor of the iterator.
    friend class ImageRegionRange;

    // Use either a const or a non-const qualified image buffer iterator.
    using QualifiedBufferIteratorType = std::conditional_t<VIsConst,
                                                           typename ImageBufferRange<TImage>::const_iterator,
                                                           typename ImageBufferRange<TImage>::iterator>;

    // QualifiedIterator data members (strictly private):

    // Iterator to the current pixel.
    QualifiedBufferIteratorType m_BufferIterator{};

    // A copy of the offset table of the image.
    OffsetTableType m_OffsetTable{ {} };

    // N-Dimensional offset relative to the index of the iteration region.
    OffsetType m_IterationOffset{ {} };

    // Size of the iteration region.
    SizeType m_IterationRegionSize{ {} };

    // Private constructor, used to create the begin and the end iterator of a range.
    // Only used by its friend class ImageRegionRange.
    QualifiedIterator(const QualifiedBufferIteratorType & bufferIterator,
                      const OffsetTableType &             offsetTable,
                      const OffsetType &                  iterationOffset,
                      const SizeType &                    regionSize) noexcept
      : m_BufferIterator(bufferIterator)
      ,
      // Note: Use parentheses instead of curly braces to initialize data members,
      // to avoid AppleClang 6.0.0.6000056 compilation error, "no viable conversion..."
      m_OffsetTable(offsetTable)
      , m_IterationOffset(iterationOffset)
      , m_IterationRegionSize(regionSize)
    {}

    template <size_t VIndex>
    void
    Increment() noexcept
    {
      m_BufferIterator += m_OffsetTable[VIndex];

      if constexpr (VIndex < (ImageDimension - 1))
      {
        if (static_cast<SizeValueType>(++m_IterationOffset[VIndex]) >= m_IterationRegionSize[VIndex])
        {
          m_IterationOffset[VIndex] = 0;
          m_BufferIterator -= m_OffsetTable[VIndex] * m_IterationRegionSize[VIndex];
          this->Increment<VIndex + 1>();
        }
      }
      else
      {
        static_assert(VIndex == (ImageDimension - 1));

        ++m_IterationOffset[VIndex];
      }
    }


    template <size_t VIndex>
    void
    Decrement() noexcept
    {
      m_BufferIterator -= m_OffsetTable[VIndex];

      if constexpr (VIndex < (ImageDimension - 1))
      {
        if (--m_IterationOffset[VIndex] < 0)
        {
          m_IterationOffset[VIndex] = m_IterationRegionSize[VIndex] - 1;
          m_BufferIterator += m_OffsetTable[VIndex] * m_IterationRegionSize[VIndex];
          this->Decrement<VIndex + 1>();
        }
      }
      else
      {
        static_assert(VIndex == (ImageDimension - 1));

        --m_IterationOffset[VIndex];
      }
    }


  public:
    // Types conforming the iterator requirements of the C++ standard library:
    using difference_type = ptrdiff_t;
    using value_type = typename std::iterator_traits<QualifiedBufferIteratorType>::value_type;
    using reference = typename std::iterator_traits<QualifiedBufferIteratorType>::reference;
    using pointer = typename std::iterator_traits<QualifiedBufferIteratorType>::pointer;
    using iterator_category = std::bidirectional_iterator_tag;

    /** Default-constructor, as required for any C++11 Forward Iterator. Offers
     * the guarantee added to the C++14 Standard: "value-initialized iterators
     * may be compared and shall compare equal to other value-initialized
     * iterators of the same type."
     *
     * \note The other five "special member functions" are defaulted implicitly,
     * following the C++ "Rule of Zero".
     */
    QualifiedIterator() = default;

    /** Constructor for implicit conversion from non-const to const iterator.  */
    template <bool VIsArgumentConst, typename = std::enable_if_t<VIsConst && !VIsArgumentConst>>
    QualifiedIterator(const QualifiedIterator<VIsArgumentConst> & arg) noexcept
      : m_BufferIterator(arg.m_BufferIterator)
      ,
      // Note: Use parentheses instead of curly braces to initialize data members,
      // to avoid AppleClang 6.0.0.6000056 compilation error, "no viable conversion..."
      m_OffsetTable(arg.m_OffsetTable)
      , m_IterationOffset(arg.m_IterationOffset)
      , m_IterationRegionSize(arg.m_IterationRegionSize)
    {}


    /**  Returns a reference to the current pixel. */
    reference
    operator*() const noexcept
    {
      return *m_BufferIterator;
    }

    /** Prefix increment ('++it'). */
    QualifiedIterator &
    operator++() noexcept
    {
      this->Increment<0>();
      return *this;
    }


    /** Postfix increment ('it++').
     * \note Usually prefix increment ('++it') is preferable. */
    QualifiedIterator
    operator++(int) noexcept
    {
      auto result = *this;
      ++(*this);
      return result;
    }


    /** Prefix decrement ('--it'). */
    QualifiedIterator &
    operator--() noexcept
    {
      this->Decrement<0>();
      return *this;
    }


    /** Postfix increment ('it--').
     * \note  Usually prefix increment ('--it') is preferable. */
    QualifiedIterator
    operator--(int) noexcept
    {
      auto result = *this;
      --(*this);
      return result;
    }


    /** Returns (it1 == it2) for iterators it1 and it2. Note that these iterators
     * should be from the same range. This operator does not support comparing iterators
     * from different ranges. */
    friend bool
    operator==(const QualifiedIterator & lhs, const QualifiedIterator & rhs) noexcept
    {
      return lhs.m_BufferIterator == rhs.m_BufferIterator;
    }


    /** Returns (it1 != it2) for iterators it1 and it2. */
    friend bool
    operator!=(const QualifiedIterator & lhs, const QualifiedIterator & rhs) noexcept
    {
      // Implemented just like the corresponding std::rel_ops operator.
      return !(lhs == rhs);
    }
  };

  // Inspired by, and originally copied from ImageBase::FastComputeOffset(ind)).
  static OffsetValueType
  ComputeOffset(const OffsetTableType & offsetTable, const IndexType & bufferedRegionIndex, const IndexType & index)
  {
    OffsetValueType offsetValue = 0;
    ImageHelper<ImageDimension, ImageDimension>::ComputeOffset(
      bufferedRegionIndex, index, offsetTable.data(), offsetValue);
    return offsetValue;
  }

  // ImageRegionRange data members (strictly private):

  BufferIteratorType m_BufferBegin{};

  IndexType m_BufferedRegionIndex{ {} };

  IndexType m_IterationRegionIndex{ {} };

  SizeType m_IterationRegionSize{ {} };

  // A copy of the offset table of the image.
  OffsetTableType m_OffsetTable{ {} };

public:
  using const_iterator = QualifiedIterator<true>;
  using iterator = QualifiedIterator<IsImageTypeConst>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;


  /** Constructs an empty range
   */
  ImageRegionRange() noexcept = default;


  /** Constructs an object, representing the range of pixels of the specified
   * region, within the specified image.
   * \note This constructor supports class template argument deduction (CTAD).
   */
  explicit ImageRegionRange(TImage & image, const RegionType & iterationRegion)
    : m_BufferBegin{ std::begin(ImageBufferRange{ image }) }
    ,
    // Note: Use parentheses instead of curly braces to initialize data members,
    // to avoid AppleClang 6.0.0.6000056 compile errors, "no viable conversion..."
    m_BufferedRegionIndex(image.TImage::GetBufferedRegion().GetIndex())
    , m_IterationRegionIndex(iterationRegion.GetIndex())
    , m_IterationRegionSize(iterationRegion.GetSize())
  {
    const OffsetValueType * const offsetTable = image.GetOffsetTable();
    assert(offsetTable != nullptr);

    if (iterationRegion.GetNumberOfPixels() > 0) // If region is non-empty
    {
      // Check if the iteration region is within the buffered region, similar
      // to checks in ImageConstIteratorWithIndex(const TImage*, const RegionType&)
      // and ImageConstIterator::SetRegion(const RegionType&).

      const auto & bufferedRegion = image.GetBufferedRegion();

      itkAssertOrThrowMacro((bufferedRegion.IsInside(iterationRegion)),
                            "Iteration region " << iterationRegion << " is outside of buffered region "
                                                << bufferedRegion);
    }

    std::copy_n(offsetTable, ImageDimension + 1, m_OffsetTable.data());
  }


  /** Constructs a range of the pixels of the requested region of an image.
   * \note This constructor supports class template argument deduction (CTAD).
   */
  explicit ImageRegionRange(TImage & image)
    : ImageRegionRange(image, image.GetRequestedRegion())
  {}


  /** Returns an iterator to the first pixel. */
  [[nodiscard]] iterator
  begin() const noexcept
  {
    return iterator{ m_BufferBegin + Self::ComputeOffset(m_OffsetTable, m_BufferedRegionIndex, m_IterationRegionIndex),
                     m_OffsetTable,
                     OffsetType(),
                     m_IterationRegionSize };
  }

  /** Returns an 'end iterator' for this range. */
  [[nodiscard]] iterator
  end() const noexcept
  {
    auto endRegionIndex = m_IterationRegionIndex;
    endRegionIndex.back() += m_IterationRegionSize.back();

    OffsetType iterationOffset{ {} }; // Initialize offsets to 0, by aggregate initializer
    *(iterationOffset.rbegin()) = m_IterationRegionSize.back();

    return iterator{ m_BufferBegin + Self::ComputeOffset(m_OffsetTable, m_BufferedRegionIndex, endRegionIndex),
                     m_OffsetTable,
                     iterationOffset,
                     m_IterationRegionSize };
  }

  /** Returns a const iterator to the first pixel.
   * Provides only read-only access to the pixel data. */
  [[nodiscard]] const_iterator
  cbegin() const noexcept
  {
    return this->begin();
  }

  /** Returns a const 'end iterator' for this range. */
  [[nodiscard]] const_iterator
  cend() const noexcept
  {
    return this->end();
  }

  /** Returns a reverse 'begin iterator' for this range. */
  [[nodiscard]] reverse_iterator
  rbegin() const noexcept
  {
    return reverse_iterator{ this->end() };
  }

  /** Returns a reverse 'end iterator' for this range. */
  [[nodiscard]] reverse_iterator
  rend() const noexcept
  {
    return reverse_iterator{ this->begin() };
  }

  /** Returns a const reverse 'begin iterator' for this range. */
  [[nodiscard]] const_reverse_iterator
  crbegin() const noexcept
  {
    return this->rbegin();
  }

  /** Returns a const reverse 'end iterator' for this range. */
  [[nodiscard]] const_reverse_iterator
  crend() const noexcept
  {
    return this->rend();
  }


  /** Returns the size of the range, that is the number of pixels in the region. */
  [[nodiscard]] size_t
  size() const noexcept
  {
    return std::accumulate(
      m_IterationRegionSize.begin(), m_IterationRegionSize.end(), size_t{ 1 }, std::multiplies<size_t>{});
  }


  /** Tells whether the range is empty. */
  [[nodiscard]] bool
  empty() const noexcept
  {
    return std::any_of(m_IterationRegionSize.begin(), m_IterationRegionSize.end(), [](const SizeValueType sizeValue) {
      return sizeValue == 0;
    });
  }


  /** Explicitly-defaulted destructor. */
  ~ImageRegionRange() = default;
};


// Deduction guide to avoid compiler warnings (-wctad-maybe-unsupported) when using class template argument deduction.
template <typename TImage>
ImageRegionRange(TImage &) -> ImageRegionRange<TImage>;

} // namespace itk

#endif
