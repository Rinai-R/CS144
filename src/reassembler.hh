#pragma once

#include "byte_stream.hh"
#include <map>
#include <string>

// 单个字符行不通，还是用 string
// // 改了很久，这里直接用单个字符了。
// class pending_data_t {
// public:
//   pending_data_t() : _data(0), is_last_substring_(false) {}
//   pending_data_t(string data, bool is_last_substring)
//       : _data(data), is_last_substring_(is_last_substring) {}

//   std::string data() const { return _data; }
//   bool is_last_substring() const { return is_last_substring_; }

// private:
//   std::string _data;                 // 单个字节
//   bool is_last_substring_;  // 该字节是否流的最后一个
// };

class Reassembler
{
public:
  // 构造函数，将数据写入给定的 ByteStream。
  // `output` 是一个右值引用，表示目标输出流，数据将在此流中重组。
  explicit Reassembler( ByteStream&& output )
    : output_( std::move( output ) ), capacity_( writer().available_capacity() )
  {}
  /*
   * 将一个新的子字符串插入到 ByteStream 中进行重组。
   *   `first_index`: 子字符串的第一个字节的索引
   *   `data`: 子字符串内容
   *   `is_last_substring`: 该子字符串是否为流的最后一部分
   *   `output`: 一个可变引用，指向目标输出流（Writer）
   *
   * Reassembler 的任务是将索引的子字符串（可能是乱序和重叠的）重新组装回原始的 ByteStream。
   * 一旦 Reassembler 得到下一个字节，它应该将其写入到输出流中。
   *
   * 如果 Reassembler 获得的字节虽然适合写入流的可用容量，但由于前面的字节仍然未知，它应该
   * 将这些字节存储在内部，直到这些“间隙”被填充。
   *
   * Reassembler 应该丢弃那些超出流的可用容量的字节（即使前面的字节填补了空缺，这些字节
   * 也无法写入）。
   *
   * Reassembler 应该在写入最后一个字节后关闭流。
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring );

  // 获取 Reassembler 中当前存储的字节数。
  uint64_t bytes_pending() const;

  // 获取输出流的读取器
  Reader& reader() { return output_.reader(); }
  const Reader& reader() const { return output_.reader(); }

  // 获取输出流的写入器，但只能是只读访问
  const Writer& writer() const { return output_.writer(); }

  uint64_t available_capacity() const { return writer().available_capacity(); };
  uint64_t next_byte_index() const { return writer().bytes_pushed(); };
  uint64_t first_unpopped_index() const { return next_byte_index() + available_capacity() - capacity_; };

  void merge_substrings();

private:
  ByteStream output_;
  uint64_t capacity_;
  uint64_t last_byte_index_ = -1;
  std::multimap<uint64_t, std::string> pending_data_ {};
};
