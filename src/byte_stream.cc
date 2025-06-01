#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ), buffer_(), read_cnt_( 0 ), write_cnt_( 0 ) {}
void Writer::push( string data )
{
  if ( Writer::is_closed() )
    return;

  Writer::write_cnt_ += min( Writer::available_capacity(), data.size() );
  Writer::buffer_.append( data );
  Writer::buffer_.resize( min( Writer::buffer_.size(), capacity_ ) );

  return;
}

void Writer::close()
{
  Writer::closed_ = true;
}

bool Writer::is_closed() const
{
  return Writer::closed_;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - Writer::buffer_.size();
}

uint64_t Writer::bytes_pushed() const
{
  return Writer::write_cnt_;
}

string_view Reader::peek() const
{
  std::string_view view( Writer::ByteStream::buffer_.data(), Writer::ByteStream::buffer_.size() );
  return view;
}

void Reader::pop( uint64_t len )
{
  if ( Reader::is_finished() )
    return;

  if ( len > Reader::bytes_buffered() )
    return;
  printf( "before buffer size: %lu\n", Reader::buffer_.size() );
  Reader::buffer_.erase( 0, len );
  printf( "after buffer size: %lu\n", Reader::buffer_.size() );
  Reader::read_cnt_ += len;
  return;
}

bool Reader::is_finished() const
{
  return Reader::closed_ && Reader::buffer_.empty();
}

uint64_t Reader::bytes_buffered() const
{
  return Reader::buffer_.size();
}

uint64_t Reader::bytes_popped() const
{
  return Reader::read_cnt_;
}