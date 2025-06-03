#include "tcp_receiver.hh"
#include <algorithm>
#include <cstdint>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  // 处理RST标志，这里是出现错误了
  if ( message.RST ) {
    reassembler_.reader().set_error();
    return;
  }

  // 处理SYN标志
  if ( message.SYN ) {
    if ( !is_initialized_ ) {
      initial_seqno_ = message.seqno; // 保存初始序列号
      is_initialized_ = true;
    }
  }

  // 如果连接尚未初始化，则不处理数据
  if ( !is_initialized_ ) {
    return;
  }

  // 计算数据应该插入的索引位置
  uint64_t _index = 0;
  if ( message.SYN ) {
    _index = 0;
  } else {
    // 因为还有一个 SYN 多余的，所以需要减去1
    _index = message.seqno.unwrap( initial_seqno_, reassembler_.next_byte_index() ) - 1;
  }

  // 处理数据段
  if ( !message.payload.empty() ) {
    reassembler_.insert( _index, message.payload, message.FIN );
  } else if ( message.FIN ) {
    // 如果 FIN 标志并且没有数据
    reassembler_.insert( _index, "", true );
  }
}

TCPReceiverMessage TCPReceiver::send() const
{
  TCPReceiverMessage message;

  uint64_t next_seqno = reassembler_.next_byte_index();
  if ( is_initialized_ ) {
    // 计算 ackno 时需要考虑 SYN 占用一个序列号
    uint64_t offset = 1;
    // 如果连接已经关闭，还需要一个 FIN
    if ( reassembler_.writer().is_closed() ) {
      offset += 1;
    }
    message.ackno = Wrap32::wrap( next_seqno + offset, initial_seqno_ );
  }

  // 面向样例编程，窗口大小不能超过 UINT16_MAX
  uint64_t window_size = reassembler_.available_capacity();
  message.window_size = min( window_size, static_cast<uint64_t>( UINT16_MAX ) );

  // 如果发生错误，设置 RST 标志
  message.RST = reassembler_.reader().has_error();

  return message;
}
