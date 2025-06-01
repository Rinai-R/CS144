#include "reassembler.hh"

using namespace std;

// 向 Reassembler 插入数据：根据 first_index（字节偏移）和数据（data）将子串放入容器
void Reassembler::insert( uint64_t first_index, std::string data, bool is_last_substring )
{
  if ( data.empty() && is_last_substring && first_index == next_byte_index() ) {
    output_.writer().close();
    return;
  }

  // 如果是最后一个子串，更新最后一个字节的位置
  if ( is_last_substring ) {
    last_byte_index_ = first_index + data.length() - 1;
  }

  // 如果当前子串可以插入
  if ( first_index < next_byte_index() + available_capacity() && first_index + data.length() > next_byte_index() ) {
    uint64_t insert_key = std::max( first_index, next_byte_index() );

    pending_data_.insert( std::make_pair(
      insert_key,
      data.substr( insert_key - first_index,
                   std::min( data.length(), next_byte_index() + available_capacity() - insert_key ) ) ) );
    // 每次插入都需要进行合并
    merge_substrings();

    if ( pending_data_.begin()->first == next_byte_index() ) {
      output_.writer().push( pending_data_.begin()->second );

      if ( pending_data_.begin()->first + pending_data_.begin()->second.length() - 1 == last_byte_index_ ) {
        output_.writer().close();
      }

      // 删除已经处理完的子串
      pending_data_.erase( pending_data_.begin() );
    }
  }
}

uint64_t Reassembler::bytes_pending() const
{
  uint64_t count = 0;
  for ( auto it = pending_data_.begin(); it != pending_data_.end(); it++ ) {
    count += it->second.length();
  }
  return count;
}

// 合并重叠的子串
void Reassembler::merge_substrings()
{
  if ( pending_data_.size() <= 1 )
    return;

  auto it = pending_data_.begin();
  auto next = std::next( it );

  // 遍历合并。
  while ( next != pending_data_.end() ) {
    // 如果当前子串和下一个子串有重叠
    if ( it->first + it->second.length() >= next->first ) {

      // 如果当前子串完全包含下一个子串，直接删除下一个子串
      if ( it->first + it->second.length() >= next->first + next->second.length() ) {
        next = pending_data_.erase( next );
        continue;
      }

      // 计算重叠部分的字节索引，将重叠部分的子串合并到当前子串中
      uint64_t overlap_index = it->first + it->second.length() - next->first;
      it->second += next->second.substr( overlap_index );

      // 删除下一个子串
      next = pending_data_.erase( next );
    } else {
      it++;
      next++;
    }
  }
}
