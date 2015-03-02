//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef WASPP_RESPONSE_HPP
#define WASPP_RESPONSE_HPP

#include <map>
#include <vector>

#include <boost/asio.hpp>

#include "name_value.hpp"

namespace waspp
{

	/// A response to be sent to a client.
	struct response
	{
		/// The status of the response.
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
		};

		void set_cookie(const std::string& name, const std::string& value)
		{
			cookies[name] = value;
		}

		void delete_cookie(const std::string& name)
		{
			set_cookie(name, std::string());
		}

		status_type status;

		/// The headers to be included in the response.
		std::vector<name_value> headers;

		/// The content to be sent in the response.
		std::string content;
		std::string content_extension;

		std::map<std::string, std::string> cookies;

		/// Convert the response into a vector of buffers. The buffers do not own the
		/// underlying memory blocks, therefore the response object must remain valid and
		/// not be changed until the write operation has completed.
		std::vector<boost::asio::const_buffer> to_buffers();

		/// Get a stock response.
		static response static_response(status_type status);
	};

} // namespace waspp

#endif // WASPP_RESPONSE_HPP
