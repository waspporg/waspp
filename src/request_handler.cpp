//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <string>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "request_handler.hpp"
#include "request.hpp"
#include "response.hpp"
#include "logger.hpp"
#include "config.hpp"
#include "database.hpp"
#include "router.hpp"
#include "mime_types.hpp"

namespace waspp
{

	request_handler::request_handler()
	{
	}

	void request_handler::handle_request(request& req, response& res)
	{
		logger* log = logger::instance();
		config* cfg = config::instance();
		database* db = database::instance();

		try
		{
			// Request path must be absolute and not contain "..".
			if (req.uri.empty() || req.uri[0] != '/'
				|| req.uri.find("..") != std::string::npos)
			{
				res = response::static_response(response::bad_request);
				return;
			}

			std::string full_path = cfg->doc_root + req.uri;
			if (boost::filesystem::exists(full_path))
			{
				router::res_file(res, full_path);
				return;
			}

			func_ptr func = router::get_func(req.uri);
			if (func == 0)
			{
				res = response::static_response(response::not_found);
				return;
			}

			func(log, cfg, db, req, res);
			if (res.finished)
			{
				return;
			}

			res.status = response::ok;

			res.headers.resize(2);
			res.headers[0].name = "Content-Length";
			res.headers[0].value = boost::lexical_cast<std::string>(res.content.size());
			res.headers[1].name = "Content-Type";
			res.headers[1].value = mime_types::extension_to_type(res.content_extension);

			std::string cookie;
			for (std::size_t i = 0; i < res.cookies.size(); ++i)
			{
				cookie.clear();
				cookie.append(res.cookies[i].name);
				cookie.append("=");
				cookie.append(res.cookies[i].value);
				cookie.append("; path=/");

				res.headers.push_back(name_value("Set-Cookie", cookie));
			}
		}
		catch (std::exception& e)
		{
			log->fatal(e.what());
			res = response::static_response(response::internal_server_error);
		}

		//res = response::static_response(response::not_found);
		//return;

		/*
			std::vector<std::string> request_params;
			boost::split(request_params, request_path, boost::is_any_of("/"));
			request_params.erase(request_params.begin());

			boost::property_tree::ptree pt, response, session, params, headers_;

			response.put("status", 2000);
			response.put("message", "OKAY");
			response.put("method", req.method);
			response.put("uri", req.uri);
			response.put("version1", req.http_version_major);
			response.put("version2", req.http_version_minor);

			std::vector<header> headers;

			session.put("userid", "steve");
			session.put("username", "steve yune");

			std::vector<std::string>::iterator i;
			for (i = request_params.begin(); i != request_params.end(); ++i)
			{
			params.push_back(std::make_pair("", *i));
			}

			pt.put_child("response", response);
			pt.put_child("session", session);
			pt.put_child("params", params);
			pt.put_child("headers_", headers_);

			std::stringstream ss;
			write_json(ss, pt, false);

			// Fill out the response to be sent to the client.
			res.status = response::ok;
			res.content = ss.str();

			res.headers.push_back(name_value("Content-Length", boost::lexical_cast<std::string>(res.content.size())));
			res.headers.push_back(name_value("Content-Type", mime_types::extension_to_type("json")));

			*/
	}

} // namespace waspp
