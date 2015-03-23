//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <vector>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "server_http2.hpp"

#ifndef CHECK_MEMORY_LEAK_WITHOUT_SSL

namespace waspp
{

	server_http2::server_http2(config* cfg_)
		: cfg(cfg_),
		signals_(io_service_),
		acceptor_(io_service_),
		context_(boost::asio::ssl::context::sslv23),
		new_connection_ssl_(),
		request_handler_()
	{
		try
		{
			context_.set_default_verify_paths();
			context_.set_options(boost::asio::ssl::context::default_workarounds);

			context_.use_certificate_chain_file(cfg->ssl_crt());
			context_.use_private_key_file(cfg->ssl_key(), boost::asio::ssl::context::pem);
		}
		catch (...)
		{
			
		}

		// Register to handle the signals that indicate when the server should exit.
		// It is safe to register for the same signal multiple times in a program,
		// provided all registration for the specified signal is made through Asio.
		signals_.add(SIGINT);
		signals_.add(SIGTERM);
#if defined(SIGQUIT)
		signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
		signals_.async_wait(boost::bind(&server_http2::handle_stop, this));

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

	void server_http2::run()
	{
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

	void server_http2::start_accept()
	{
		new_connection_ssl_.reset(new connection_ssl(io_service_, context_, request_handler_));
		acceptor_.async_accept(new_connection_ssl_->socket().lowest_layer(),
			boost::bind(&server_http2::handle_accept, this,
			boost::asio::placeholders::error));
	}

	void server_http2::handle_accept(const boost::system::error_code& e)
	{
		if (!e)
		{
			new_connection_ssl_->start();
		}

		start_accept();
	}

	void server_http2::handle_stop()
	{
		io_service_.stop();
	}

} // namespace waspp

#endif // CHECK_MEMORY_LEAK_WIHTOUT_OPEN_SSL