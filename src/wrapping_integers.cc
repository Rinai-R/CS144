#include "wrapping_integers.hh"
#include <cmath>

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  return Wrap32( ( n + zero_point.raw_value_ + ( 1ULL << 32 ) ) % ( 1ULL << 32 ) );
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // 零点值和当前值的差值。
  uint64_t offset = ( ( 1ULL << 32 ) + ( raw_value_ - zero_point.raw_value_ ) ) % ( 1ULL << 32 );
  // 这里是计算去除低 32 位的偏移量然后只取前面的部分。
  uint64_t base = ( checkpoint / ( 1ULL << 32 ) ) * ( 1ULL << 32 );

  // 取值可能有三种情况
  uint64_t src[3] = { base + offset, base - ( 1ULL << 32 ) + offset, base + ( 1ULL << 32 ) + offset };

  uint64_t closest = src[0];
  uint64_t min_diff = ( src[0] > checkpoint ) ? ( src[0] - checkpoint ) : ( checkpoint - src[0] );
  for ( int i = 1; i < 3; i++ ) {
    uint64_t diff = ( src[i] > checkpoint ) ? ( src[i] - checkpoint ) : ( checkpoint - src[i] );
    // 如果差值更小，则更新最近值
    if ( diff < min_diff ) {
      min_diff = diff;
      closest = src[i];
    }
  }
  return closest;
}
