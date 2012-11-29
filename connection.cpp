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
#include "file_cache.hpp"

namespace http {
namespace server3 {

connection::connection(asio::io_service& io_service,
    request_handler& handler)
  : strand_(io_service),
    socket_(io_service),
    request_handler_(handler),
    connection_timer(io_service)
{
    log = Log::getInstance();
	file_c = file_cache::getInstance();
    connection_timer.expires_at(boost::posix_time::pos_infin);//不作为
}

asio::ip::tcp::socket& connection::socket()
{
  return socket_;
}

 std::string toString(short unsigned int num){
     std::ostringstream oss;
     oss<<num;
     return oss.str();
 }

void connection::start()
{

    address_ = socket_.remote_endpoint().address().to_string();
    port_    = toString(socket_.remote_endpoint().port());
    std::cerr << "connection start at client[" + address_  + ":" + port_  +"]\n";
    connection::start_read();
    //设置连接时间
    //connection_timer.expires_from_now(boost::posix_time::seconds(60));
  
    //socket_.async_read_some(asio::buffer(buffer_),
      //strand_.wrap(
        //boost::bind(&connection::handle_read, shared_from_this(),
          //asio::placeholders::error,
          //asio::placeholders::bytes_transferred)));

    connection_timer.async_wait(strand_.wrap(boost::bind(&connection::check_connection_end, shared_from_this(), asio::placeholders::error)));
}

void connection::check_connection_end(const error_code& e)
{
    if(!e){
        if (connection::stopped())
            return;

        // Check whether the deadline has passed. We compare the deadline against
        // the current time since a new asynchronous operation may have moved the
        // deadline before this actor had a chance to run.
        if (connection_timer.expires_at() <= boost::asio::deadline_timer::traits_type::now()){
            // The deadline has passed. Stop the session. The other actors will
            // terminate as soon as possible.
            connection::stop();
        }
        else{
            // Put the actor back to sleep.
            connection_timer.async_wait(
                strand_.wrap(
                    boost::bind(&connection::check_connection_end,
                    shared_from_this(), asio::placeholders::error)));
        }
    }else{
        std::cerr << "check connection error:" << e.message() <<"\n";
    }
}


const bool connection::stopped(){
    return !socket_.is_open();
}

void connection::stop(){
    error_code ignored_ec;
    std::cerr << "connection end at client[" + address_ + ":" + port_ +"]\n";
    //std::cerr << "one connection:end\n";
    socket_.close(ignored_ec);
    connection_timer.cancel();
}

void connection::start_read(){
    //设置连接时间
    connection_timer.expires_from_now(boost::posix_time::seconds(60));
    
    socket_.async_read_some(asio::buffer(buffer_),
        strand_.wrap(
            boost::bind(&connection::handle_read, shared_from_this(),
            asio::placeholders::error,
            asio::placeholders::bytes_transferred)));

}

void connection::handle_read(const error_code& e,
    std::size_t bytes_transferred)
{
    if(connection::stopped()){
        return;
    }
    //std::cout<<"read:"<<buffer_.data();
    if (!e)
    {
        boost::tribool result;
        boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
            request_, buffer_.data(), buffer_.data() + bytes_transferred);
        //std::cout<<"request:"<<request_.uri<<"\n";
	    //string mess_r = "Client[" + socket_.remote_endpoint().address().to_string() + ":" + toString(socket_.remote_endpoint().port()) +\
					  "] request file " + request_.uri;
   	    //记录 请求信息
	    //log->record(mess_r);
        //std::cout<<"record"<<"\n";
        //std::string address = socket_.remote_endpoint().address().to_string();
        //std::string port = toString(socket_.remote_endpoint().port());
        if (result)
        {
			
		    //std::cout<<"Requst: "<<request_.uri<<"\n";
		    reply *rep = file_c->hasKey(request_.uri);
		    if(rep){
				asio::async_write(socket_, rep->to_buffers(),
					strand_.wrap(
						boost::bind(&connection::handle_write, shared_from_this(),
						asio::placeholders::error, rep)));
		    }else{
      		    request_handler_.handle_request(request_, reply_);
			    //reply *rep = (reply *)malloc(sizeof(reply));
			    if(reply_.status == reply::ok){
				    reply *rep = new reply();
				    if(!rep){
					    std::cerr<<"read failed for failed to malloc reply\n";
					    return;
				    }
				    //std::cout<<"content\n";
				    rep->content = reply_.content;
				    //std::cout<<"header\n";
				    rep->headers = reply_.headers;
				    //std::cout<<"status\n";
				    rep->status = reply_.status;
				    file_c->insert(request_.uri, rep);
			    }
      		    asio::async_write(socket_, reply_.to_buffers(),
          		strand_.wrap(
            		boost::bind(&connection::handle_write, shared_from_this(),
              		asio::placeholders::error, &reply_)));
		    }
        }
        else if (!result){
            reply_ = reply::stock_reply(reply::bad_request);
            asio::async_write(socket_, reply_.to_buffers(),
                strand_.wrap(
                    boost::bind(&connection::handle_write, shared_from_this(),
                    asio::placeholders::error, &reply_)));
        }else{
            //socket_.async_read_some(asio::buffer(buffer_),
            //strand_.wrap(
            //boost::bind(&connection::handle_read, shared_from_this(),
              //asio::placeholders::error,
              //asio::placeholders::bytes_transferred)));
            connection::start_read();
        }
    }else{
        stop();//stop the connection
    }
    // If an error occurs then no new asynchronous operations are started. This
    // means that all shared_ptr references to the connection object will
    // disappear and the object will be destroyed automatically after this
    // handler returns. The connection class's destructor closes the socket.
}

void connection::handle_write(const error_code& e, reply *rep)
{
    //如果连接已经断开 则放弃写回
    if(stopped())return;
    string mess_r = "Client[" + address_ + ":" + port_ + "] request file " + request_.uri;
    //记录 请求信息
    log->record(mess_r);
    if (!e)
    {
        // Initiate graceful connection closure.
        error_code ignored_ec;
        socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
	
	    string mess_s;
	    if(rep->status == reply::ok){
		    mess_s = "Successfully send file " + request_.uri + " with " + boost::lexical_cast<std::string>(rep->content.size())+ " bytes to clien[" +\
					 address_ + ":" + port_ + "]";
	    }else{
		    mess_s = "Failed to send file " + request_.uri + " to client[" + address_ + ":" + port_ + "] due to " + rep->to_string(rep->status);
	    }
	    log->record(mess_s);
    }else{
  	    string mess_f = "Failed to send file " + request_.uri + " to client[" + address_ + ":" + port_  + "] due to error " + e.message() ;
	    log->record(mess_f);
    }

    // No new asynchronous operations are started. This means that all shared_ptr
    // references to the connection object will disappear and the object will be
    // destroyed automatically after this handler returns. The connection class's
    // destructor closes the socket.
}

} // namespace server3
} // namespace http
