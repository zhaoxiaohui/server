//
// connection.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER3_CONNECTION_HPP
#define HTTP_SERVER3_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"
#include "log.hpp"
#include "file_cache.hpp"

using namespace boost;
using boost::system::error_code;
namespace http {
	namespace server3 {

		/// Represents a single connection from a client.
		class connection
  			: public boost::enable_shared_from_this<connection>,
    		private boost::noncopyable
		{
			public:
  			/// Construct a connection with the given io_service.
  			explicit connection(asio::io_service& io_service,
      			request_handler& handler, std::string& log_fullname);

  			/// Get the socket associated with the connection.
  			asio::ip::tcp::socket& socket();

 			 /// Start the first asynchronous operation for the connection.
  			void start();

			private:
  			/// Handle completion of a read operation.
  			void handle_read(const error_code& e,
      			std::size_t bytes_transferred);

  			/// Handle completion of a write operation.
  			void handle_write(const error_code& e, reply *rep);

            ///检查连接是否超时
            void check_connection_end(const error_code& e);

            ///start to read
            void start_read();

            ///check is the socket is already closed
            const bool stopped();

            ///timeout then stop socket
            void stop();

  			/// Strand to ensure the connection's handlers are not called concurrently.
  			asio::io_service::strand strand_;

  			/// Socket for the connection.
  			asio::ip::tcp::socket socket_;

  			/// The handler used to process the incoming request.
  			request_handler& request_handler_;

  			/// Buffer for incoming data.
  			boost::array<char, 8192> buffer_;

  			/// The incoming request.
  			request request_;

  			/// The parser for the incoming request.
  			request_parser request_parser_;

  			/// The reply to be sent back to the client.
  			reply reply_;

            ///远端连接地址
            std::string address_;
            std::string port_;

			///日志记录
			Log* log;

			///缓存
			file_cache *file_c;

            ///设定连接时间
            asio::deadline_timer connection_timer;

			///连接个数
			//int connection_num;

		};

		typedef boost::shared_ptr<connection> connection_ptr;

	} // namespace server3
} // namespace http

#endif // HTTP_SERVER3_CONNECTION_HPP
