//
// reply.hpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REPLY_HPP
#define HTTP_REPLY_HPP

#include <string>
#include <vector>
#include <memory>
#include <boost/asio.hpp>
#include "http/server/header.hpp"


namespace http {
namespace server {

using namespace boost::asio::local;

/// A reply to be sent to a client.
struct reply
{
  reply()
    : ready_to_go(true)
    {}
  /// The status of the reply.
  enum status_type
  {
    ok = 200,
    created = 201,
    accepted = 202,
    no_content = 204,
    multiple_choices = 300,
    moved_permanently = 301,
    moved_temporarily = 302,
    not_modified = 304,
    bad_request = 400,
    unauthorized = 401,
    forbidden = 403,
    not_found = 404,
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503
  } status;

  /// The headers to be included in the reply.
  std::vector<header> headers;

  /// The content to be sent in the reply.
  std::string content;

  /* if the reply is ready to send to client */
  bool ready_to_go;

  /* connected pair to signal the content is ready for delivery */
  std::shared_ptr<stream_protocol::socket> read_side;
  std::shared_ptr<stream_protocol::socket> write_side;

  /// Convert the reply into a vector of buffers. The buffers do not own the
  /// underlying memory blocks, therefore the reply object must remain valid and
  /// not be changed until the write operation has completed.
  std::vector<boost::asio::const_buffer> to_buffers();

  void handle_write(const boost::system::error_code& error, // Result of operation.
                    std::size_t bytes_transferred           // Number of bytes written.
  );

  /* tell the connection to deleivery the content */
  void ready();

  /// Get a stock reply.
  static reply stock_reply(status_type status);
};

} // namespace server
} // namespace http

#endif // HTTP_REPLY_HPP
