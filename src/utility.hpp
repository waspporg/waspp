/*
Copyright (c) 2014-present WASPP (waspp.org at gmail dot com)

Distributed under the Boost Software License, Version 1.0.
http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef WASPP_UTILITY_HPP
#define WASPP_UTILITY_HPP

#include <cstdlib>

#include <openssl/md5.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <atomic>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "name_value.hpp"

namespace waspp
{

	template<typename T>
	class singleton
	{
	public:
		static T* instance()
		{
			if (instance_ == nullptr)
			{
				instance_ = new T();

				// avoid memory leak
				atexit(destroy);
			}

			return instance_;
		}

	protected:
		singleton() {}
		virtual ~singleton() {}

	private:
		static void destroy()
		{
			delete instance_;
		}

		static T* instance_;

	};

	template<typename T> T* singleton<T>::instance_ = nullptr;

	class spinlock
	{
	public:
		spinlock() : state_(unlocked) {}

		void acquire()
		{
			while (state_.exchange(locked, std::memory_order_acquire) == locked)
			{
				/* busy-wait */
			}
		}

		void release()
		{
			state_.store(unlocked, std::memory_order_release);
		}

	private:
		enum state { locked, unlocked };
		std::atomic<state> state_;

	};

	enum class uri_type
	{
		tcp = 1,
		http_get = 2,
		http_post = 3,

		ssl = 4,
		https_get = 5,
		https_post = 6
	};

	class uri_conn
	{
	public:
		uri_conn(uri_type type_, const std::string& host_, const std::string& port_ = "");
		~uri_conn();

		void set_http_headers(const std::vector<name_value>& req_headers_) { req_headers = req_headers_; }
		bool query(const std::string& path = "/", const std::string& data = "");

		const std::string& res_headers() { return res_headers_; }
		const std::string& res_content() { return res_content_; }

	private:
		void set_http_get(std::ostream& req_stream, const std::string& path);
		void set_http_post(std::ostream& req_stream, const std::string& path, const std::string& data);

		template<typename T>
		bool tcp_query(T& socket_, std::ostream& req_stream, const std::string& data)
		{
			try
			{
				if (!data.empty())
				{
					req_stream << data;

					// Send the request.
					boost::asio::write(socket_, req_buf);
				}

				boost::system::error_code error;
				boost::asio::read(socket_, res_buf, boost::asio::transfer_at_least(1), error);

				if (error)
				{
					return false;
				}

				std::ostringstream oss;
				oss << &res_buf;

				res_content_ = oss.str();
				return true;
			}
			catch (...)
			{

			}

			return false;
		}

		template<typename T>
		bool http_query(T& socket_)
		{
			try
			{
				// Send the request.
				boost::asio::write(socket_, req_buf);

				// Read the response status line. The response streambuf will automatically
				// grow to accommodate the entire line. The growth may be limited by passing
				// a maximum size to the streambuf constructor.
				boost::asio::read_until(socket_, res_buf, "\r\n");

				// Check that response is OK.
				std::istream res_stream(&res_buf);
				if (!is_200(res_stream))
				{
					return false;
				}

				// Read the response headers, which are terminated by a blank line.
				boost::asio::read_until(socket_, res_buf, "\r\n\r\n");

				// Process the response headers.
				while (std::getline(res_stream, res_headers_) && res_headers_ != "\r")
				{
				}

				// Write whatever content we already have to output.
				std::ostringstream oss;
				if (res_buf.size() > 0)
				{
					oss << &res_buf;
				}

				// Read until EOF, writing data to output as we go.
				boost::system::error_code error;
				while (boost::asio::read(socket_, res_buf,
					boost::asio::transfer_at_least(1), error))
				{
					oss << &res_buf;
				}

				if (error != boost::asio::error::eof)
				{
					throw boost::system::system_error(error);
				}

				res_content_ = oss.str();
				return true;
			}
			catch (...)
			{

			}

			return false;
		}

		bool is_200(std::istream& res_stream);

		boost::asio::io_service io_service_;

		uri_type uri_type_;
		std::string host;
		std::string port;

		std::vector<name_value> req_headers;
		boost::asio::streambuf req_buf;

		boost::asio::streambuf res_buf;
		std::string res_headers_;
		std::string res_content_;

	};

	std::string get_extension(const std::string& path);
	std::string md5_digest(const std::string& str);

	void gzip_str(std::string& str);

	/* -*-mode:c++; c-file-style: "gnu";-*- */
	/*
	*  $Id: CgiUtils.h,v 1.17 2014/04/23 20:55:03 sebdiaz Exp $
	*
	*  Copyright (C) 1996 - 2004 Stephen F. Booth <sbooth@gnu.org>
	*                       2007 Sebastien DIAZ <sebastien.diaz@gmail.com>
	*  Part of the GNU cgicc library, http://www.gnu.org/software/cgicc
	*
	*  This library is free software; you can redistribute it and/or
	*  modify it under the terms of the GNU Lesser General Public
	*  License as published by the Free Software Foundation; either
	*  version 3 of the License, or (at your option) any later version.
	*
	*  This library is distributed in the hope that it will be useful,
	*  but WITHOUT ANY WARRANTY; without even the implied warranty of
	*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	*  Lesser General Public License for more details.
	*
	*  You should have received a copy of the GNU Lesser General Public
	*  License along with this library; if not, write to the Free Software
	*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
	*/

	bool strings_are_equal(const std::string& s1, const std::string& s2);
	bool strings_are_equal(const std::string& s1, const std::string& s2, std::size_t n);

	std::string extract_between(const std::string& data, const std::string& separator1, const std::string& separator2);
	std::string extract_between(const std::string& datas, const std::string& separators);

	std::string char2hex(char c);
	char hex2char(char first, char second);

	// also known as url_encode
	std::string percent_encode(const std::string& src);

	// also known as url_decode
	std::string percent_decode(const std::string& src);

	/*
	base64.cpp and base64.h

	Copyright (C) 2004-2008 Rene Nyffenegger

	This source code is provided 'as-is', without any express or implied
	warranty. In no event will the author be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this source code must not be misrepresented; you must not
	claim that you wrote the original source code. If you use this source code
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original source code.

	3. This notice may not be removed or altered from any source distribution.

	Rene Nyffenegger rene.nyffenegger@adp-gmbh.ch
	*/

	// this is NOT the original source code.
	// updated the function base64_encode to get string input.

	static const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	static inline bool is_base64(unsigned char c)
	{
		return (isalnum(c) || (c == '+') || (c == '/'));
	}

	std::string base64_encode(std::string const& string_to_encode);
	std::string base64_decode(std::string const& encoded_string);

} // namespace waspp

#endif // WASPP_UTILITY_HPP
