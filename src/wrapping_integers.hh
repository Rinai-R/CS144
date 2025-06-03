#pragma once

#include <cstdint>

/*
 * Wrap32 类型表示一个 32 位无符号整数，它具有：
 *    - 一个任意的“零点”（初始值），
 *    - 当它的值达到 2^32 - 1 时会回绕回零。
 */

class Wrap32
{
public:
  // 构造函数：接受一个原始的 32 位无符号整数值并初始化
  explicit Wrap32( uint32_t raw_value ) : raw_value_( raw_value ) {}

  /*
   * wrap 方法：通过给定的绝对序列号 n 和零点（Wrap32 对象）来构造一个 Wrap32 对象。
   * 这是一个静态方法，返回的 Wrap32 对象根据给定的序列号进行包装。
   */
  static Wrap32 wrap( uint64_t n, Wrap32 zero_point );

  /*
   * unwrap 方法：返回一个绝对序列号，它是与当前 Wrap32 对象对应的绝对序列号。
   * unwrap 方法会参考给定的零点（Wrap32）和一个“检查点”（checkpoint），
   * 返回最接近的绝对序列号。对于同一个 Wrap32，可能存在多个对应的绝对序列号，
   * unwrap 方法会返回最接近检查点的一个。
   */
  uint64_t unwrap( Wrap32 zero_point, uint64_t checkpoint ) const;

  // 运算符重载：对 Wrap32 对象进行加法运算，返回一个新的 Wrap32 对象
  Wrap32 operator+( uint32_t n ) const { return Wrap32 { raw_value_ + n }; }

  // 运算符重载：判断两个 Wrap32 对象是否相等
  bool operator==( const Wrap32& other ) const { return raw_value_ == other.raw_value_; }

protected:
  uint32_t raw_value_ {}; // 存储包装后的原始值
};
