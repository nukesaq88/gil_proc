#ifndef _GIL_PROC_PREWITT_HPP_
#define _GIL_PROC_PREWITT_HPP_

#include <cmath>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/algorithm.hpp>

#include "filter_8_neighbor.hpp"

namespace gil_proc
{

namespace prewitt_detail
{
template <typename T>
inline T square( const T &x ) { return x * x; }
}

struct prewitt_functor
{
  template <typename Pixel>
  Pixel operator()( const Pixel &top_left,    const Pixel &top,        const Pixel &top_right,
                    const Pixel &left,        const Pixel &/*center*/, const Pixel &right,
                    const Pixel &bottom_left, const Pixel &bottom,     const Pixel &bottom_right ) const
  {
    using namespace boost;
    // pixel concept check
    gil::gil_function_requires< gil::PixelConcept<Pixel> >();

    typedef typename gil::channel_type<Pixel>::type channel_t;
    const channel_t min_val = gil::channel_traits<channel_t>::min_value();
    const channel_t max_val = gil::channel_traits<channel_t>::max_value();

    Pixel dst;
    for ( int c = 0; c < gil::num_channels<Pixel>::value; ++c )
    {
      const float sum_square = sobel_detail::square(
            ( top_left[c]     * -1 ) +
            ( top[c]          * -1 ) +
            ( top_right[c]    * -1 ) +
            ( bottom_left[c]  *  1 ) +
            ( bottom[c]       *  1 ) +
            ( bottom_right[c] *  1 )
            )
          +
          sobel_detail::square(
            ( top_left[c]     * -1 ) +
            ( left[c]         * -1 ) +
            ( bottom_left[c]  * -1 ) +
            ( top_right[c]    *  1 ) +
            ( right[c]        *  1 ) +
            ( bottom_right[c] *  1 )
            );

      dst[c] = boost::algorithm::clamp( std::sqrt( sum_square ), min_val, max_val );

    }
    return dst;
  }
};


template <typename SrcView, typename DstView>
void prewitt( const SrcView& src, const DstView& dst )
{
  using namespace boost;
  //image view concept check
  gil::gil_function_requires< gil::ImageViewConcept<SrcView> >();
  gil::gil_function_requires< gil::MutableImageViewConcept<DstView> >();

  filer_8_neighbor( src, dst, prewitt_functor() );
}



}

#endif // _GIL_PROC_PREWITT_HPP_
