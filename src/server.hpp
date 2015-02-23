//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

//
// Copyright (c) 2014-2015 WASPP (waspp.org@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef WASPP_SERVER_HPP
#define WASPP_SERVER_HPP

#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "connection.hpp"
#include "request_handler.hpp"

namespace waspp
{

	/// The top-level class of the HTTP server.
	class server
		: private boost::noncopyable
	{
	public:
		/// Construct the server to listen on the specified TCP address and port, and
		/// serve up files from the given directory.
		explicit server(const std::string& address, const std::string& port,
			const std::string& doc_root, std::size_t thread_pool_size);

		/// Run the server's io_service loop.
		void run();

	private:
		/// Initiate an asynchronous accept operation.
		void start_accept();

		/// Handle completion of an asynchronous accept operation.
		void handle_accept(const boost::system::error_code& e);

		/// Handle a request to stop the server.
		void handle_stop();

		/// The number of threads that will call io_service::run().
		std::size_t thread_pool_size_;

		/// The io_service used to perform asynchronous operations.
		boost::asio::io_service io_service_;

		/// The signal_set is used to register for process termination notifications.
		boost::asio::signal_set signals_;

		/// Acceptor used to listen for incoming connections.
		boost::asio::ip::tcp::acceptor acceptor_;

		/// The next connection to be accepted.
		connection_ptr new_connection_;

		/// The handler for all incoming requests.
		request_handler request_handler_;

	};

} // namespace waspp

#endif // WASPP_SERVER_HPP