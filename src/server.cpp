//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <vector>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "server.hpp"

namespace waspp
{

	server::server(logger* log_, config* cfg_)
		: log(log_),
		cfg(cfg_),
		signals_(io_service_),
		acceptor_(io_service_),
		new_connection_(),
		request_handler_()
	{
		// Register to handle the signals that indicate when the server should exit.
		// It is safe to register for the same signal multiple times in a program,
		// provided all registration for the specified signal is made through Asio.
		signals_.add(SIGINT);
		signals_.add(SIGTERM);
#if defined(SIGQUIT)
		signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
		signals_.async_wait(boost::bind(&server::handle_stop, this));

		// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
		boost::asio::ip::tcp::resolver resolver(io_service_);
		boost::asio::ip::tcp::resolver::query query(cfg->address(), cfg->port());
		boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
		acceptor_.open(endpoint.protocol());
		acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		acceptor_.set_option(boost::asio::ip::tcp::acceptor::keep_alive(true));
		acceptor_.bind(endpoint);
		acceptor_.listen();

		start_accept();
	}

	void server::run()
	{
		log->info("server starting..");

		// Create a pool of threads to run all of the io_services.
		std::vector< boost::shared_ptr<boost::thread> > threads;
		for (std::size_t i = 0; i < cfg->num_threads(); ++i)
		{
			boost::shared_ptr<boost::thread> thread(new boost::thread(
				boost::bind(&boost::asio::io_service::run, &io_service_)));
			threads.push_back(thread);
		}

		// Wait for all threads in the pool to exit.
		for (std::size_t i = 0; i < threads.size(); ++i)
		{
			threads[i]->join();
		}
	}

	void server::start_accept()
	{
		new_connection_.reset(new connection(io_service_, request_handler_));
		acceptor_.async_accept(new_connection_->socket(),
			boost::bind(&server::handle_accept, this,
			boost::asio::placeholders::error));
	}

	void server::handle_accept(const boost::system::error_code& e)
	{
		if (!e)
		{
			new_connection_->start();
		}

		start_accept();
	}

	void server::handle_stop()
	{
		io_service_.stop();
		log->info("server stopped");
	}

} // namespace waspp
