//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

//#include <algorithm>

#include "request.hpp"

namespace waspp
{

	void request::parse_remote_endpoint(const std::string& remote_endpoint)
	{
		std::size_t pos = remote_endpoint.find(":");
		if (pos == std::string::npos)
		{
			return;
		}

		remote_addr = remote_endpoint.substr(0, pos);
		remote_port = remote_endpoint.substr(pos + 1);
	}

	std::string& request::header(const std::string& name)
	{
		std::vector<name_value>::iterator found;
		found = std::find_if(headers.begin(), headers.end(), boost::bind(&name_value::compare_name, _1, name));

		if (found == headers.end())
		{
			headers.push_back(name_value(name, std::string()));
			found = headers.end() - 1;
		}

		return found->value;
	}

	std::string& request::cookie(const std::string& name)
	{
		std::vector<name_value>::iterator found;
		found = std::find_if(cookies.begin(), cookies.end(), boost::bind(&name_value::compare_name, _1, name));

		if (found == cookies.end())
		{
			cookies.push_back(name_value(name, std::string()));
			found = cookies.end() - 1;
		}

		return found->value;
	}

	std::string& request::param(const std::string& name)
	{
		std::vector<name_value>::iterator found;
		found = std::find_if(params.begin(), params.end(), boost::bind(&name_value::compare_name, _1, name));

		if (found == params.end())
		{
			params.push_back(name_value(name, std::string()));
			found = params.end() - 1;
		}

		return found->value;
	}

	void request::reset()
	{
		remote_addr.clear();
		remote_port.clear();
		connection_option = 'K';

		method.clear();
		uri.clear();
		http_version_major = 0;
		http_version_minor = 0;
		headers.clear();

		content.clear();
		content_length = 0;

		cookies.clear();
		params.clear();
		rest_params.clear();
		uploads.clear();
	}

} // namespace waspp

