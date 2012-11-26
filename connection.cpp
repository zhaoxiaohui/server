//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include <vector>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "request_handler.hpp"

namespace http {
namespace server3 {

connection::connection(asio::io_service& io_service,
    request_handler& handler)
  : strand_(io_service),
    socket_(io_service),
    request_handler_(handler)
{
}

asio::ip::tcp::socket& connection::socket()
{
  return socket_;
}

void connection::start()
{
  socket_.async_read_some(asio::buffer(buffer_),
      strand_.wrap(
        boost::bind(&connection::handle_read, shared_from_this(),
          asio::placeholders::error,
          asio::placeholders::bytes_transferred)));
}

std::string toString(short unsigned int num){
	std::ostringstream oss;
	oss<<num;
	return oss.str();
}
std::string toString(int num){
	std::ostringstream oss;
	oss<<num;
	return oss.str();
}
void connection::handle_read(const error_code& e,
    std::size_t bytes_transferred)
{
    std::cout<<"read:"<<buffer_.data();
  if (!e)
  {
    boost::tribool result;
    boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
        request_, buffer_.data(), buffer_.data() + bytes_transferred);
    //std::cout<<"request:"<<request_.uri<<"\n";
	string mess_r = "Client[" + socket_.remote_endpoint().address().to_string() + ":" + toString(socket_.remote_endpoint().port()) +\
					  "] request file " + request_.uri;
   	//记录 请求信息
	log.record(mess_r);

    if (result)
    {
			
		std::cout<<"Requst: "<<request_.uri<<"\n";
      request_handler_.handle_request(request_, reply_);
      asio::async_write(socket_, reply_.to_buffers(),
          strand_.wrap(
            boost::bind(&connection::handle_write, shared_from_this(),
              asio::placeholders::error)));
    }
    else if (!result)
    {
      reply_ = reply::stock_reply(reply::bad_request);
      asio::async_write(socket_, reply_.to_buffers(),
          strand_.wrap(
            boost::bind(&connection::handle_write, shared_from_this(),
              asio::placeholders::error)));
    }
    else
    {
      socket_.async_read_some(asio::buffer(buffer_),
          strand_.wrap(
            boost::bind(&connection::handle_read, shared_from_this(),
              asio::placeholders::error,
              asio::placeholders::bytes_transferred)));
    }
  }

  // If an error occurs then no new asynchronous operations are started. This
  // means that all shared_ptr references to the connection object will
  // disappear and the object will be destroyed automatically after this
  // handler returns. The connection class's destructor closes the socket.
}

void connection::handle_write(const error_code& e)
{
  if (!e)
  {
    // Initiate graceful connection closure.
    error_code ignored_ec;
    socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
	string mess_s = "Successfully send file " + request_.uri + " with " + boost::lexical_cast<std::string>(reply_.content.size())+ " bytes to clien[" +\
					 socket_.remote_endpoint().address().to_string() + ":" + toString(socket_.remote_endpoint().port()) + "]";
	log.record(mess_s);
  }else{
  	string mess_f = "Failed to send file " + request_.uri + " to client[" + socket_.remote_endpoint().address().to_string() + ":"  +\
					 toString(socket_.remote_endpoint().port()) + "] due to error ";
	log.record(mess_f);
  }

  // No new asynchronous operations are started. This means that all shared_ptr
  // references to the connection object will disappear and the object will be
  // destroyed automatically after this handler returns. The connection class's
  // destructor closes the socket.
}

} // namespace server3
} // namespace http
