/*
Copyright (c) 2014-2015 WASPP (waspp.org at gmail dot com)

Distributed under the Boost Software License, Version 1.0.
http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef WASPP_CONFIG_HPP
#define WASPP_CONFIG_HPP

#include <string>
#include <vector>

#include "name_value.hpp"
#include "app_status.hpp"
#include "utility.hpp"

namespace waspp
{

	struct cfgpair
	{
		cfgpair(const std::string& first_, std::vector<name_value> second_) : first(first_), second(second_)
		{
		}

		bool compare_first(const std::string& first_)
		{
			return first == first_;
		}

		std::string first;
		std::vector<name_value> second;
	};

	struct statuspair
	{
		statuspair(int first_, const std::string& second_) : first(first_), second(second_)
		{
		}

		bool compare_first(int first_)
		{
			return first == first_;
		}

		int first;
		std::string second;
	};

	class config
		: public singleton<config>
	{
	public:
		config();
		~config();

		bool init(const std::string& file, const std::string& server_id);
		std::vector<name_value>& get(const std::string& item);
		const std::string& msg(app_status_type status_code);

		// logger
		const std::string& level() { return level_; }
		const std::string& rotation() { return rotation_; }

		// session
		const std::string& encrypt_key() { return encrypt_key_; }
		const std::string& sess_cookie() { return sess_cookie_; }
		double expiry_sec() { return expiry_sec_; }
		double update_sec() { return update_sec_; }
		bool validate_ep() { return validate_ep_; }
		bool validate_ua() { return validate_ua_; }

		// server
		const std::string& address() { return address_; }
		const std::string& port() { return port_; }
		const std::string& doc_root() { return doc_root_; }
		std::size_t num_threads() { return num_threads_; }
		bool compress() { return compress_; }

		// ssl
		bool use_ssl() { return use_ssl_; }
		const std::string& ssl_passwd() { return ssl_passwd_; }
		const std::string& ssl_cert_chain() { return ssl_cert_chain_; }
		const std::string& ssl_priv_key() { return ssl_priv_key_; }
		const std::string& ssl_ca_cert() { return ssl_ca_cert_; }
		

	private:
		// logger
		std::string level_, rotation_;

		// session
		std::string encrypt_key_, sess_cookie_;
		double expiry_sec_, update_sec_;
		bool validate_ep_, validate_ua_;

		// server
		std::string address_, port_, doc_root_;
		std::size_t num_threads_;
		bool compress_;

		// ssl
		bool use_ssl_;
		std::string ssl_passwd_, ssl_cert_chain_, ssl_priv_key_, ssl_ca_cert_;

		std::vector<cfgpair> cfg_;

		// status messages
		std::string msg_locale;
		std::vector<statuspair> status_;

	};

} // namespace waspp

#endif // WASPP_CONFIG_HPP
