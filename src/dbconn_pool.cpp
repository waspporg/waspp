/*
Copyright (c) 2014-present WASPP (waspp.org at gmail dot com)

Distributed under the Boost Software License, Version 1.0.
http://www.boost.org/LICENSE_1_0.txt
*/

#include "dbconn_pool.hpp"
#include "logger.hpp"

namespace waspp
{

	dbconn_pool::dbconn_pool()
		: port(0), pool_size(0), wait_timeout_sec(0), pool(0)
	{
		
	}

	dbconn_pool::~dbconn_pool()
	{
		for (auto& p : pool)
		{
			delete p;
		}
	}

	bool dbconn_pool::init_pool(std::unordered_map<std::string, std::string>& cfg)
	{
		std::vector<std::string> keys =  { "host", "userid", "passwd", "database", "port", "charset", "pool_size", "wait_timeout_sec" };
		for (auto& key : keys)
		{
			auto found = cfg.find(key);
			if (found == cfg.end())
			{
				return false;
			}

			if (key == "host")
			{
				host = found->second;
			}
			else if (key == "userid")
			{
				userid = found->second;
			}
			else if (key == "passwd")
			{
				passwd = found->second;
			}
			else if (key == "database")
			{
				database = found->second;
			}
			else if (key == "port")
			{
				port = strtoul(found->second.c_str(), nullptr, 0);
			}
			else if (key == "charset")
			{
				charset = found->second;
			}
			else if (key == "pool_size")
			{
				pool_size = strtoull(found->second.c_str(), nullptr, 0);
			}
			else if (key == "wait_timeout_sec")
			{
				wait_timeout_sec = strtod(found->second.c_str(), nullptr);
			}
		}

		return true;
	}

	bool dbconn_pool::fill_pool()
	{
		if (pool_size == 0)
		{
			return false;
		}

		for (std::size_t i = 0; i < pool_size; ++i)
		{
			auto dbconn = connect();
			if (!dbconn->ping())
			{
				delete dbconn;
				return false;
			}

			pool.push_back(dbconn);
		}

		return true;
	}

	mysqlpp::connection* dbconn_pool::get_dbconn()
	{
		spinlock_.lock();
		//
			if (pool.empty())
			{
				spinlock_.unlock();

				log(warn) << "dbconn_pool is empty";
				return connect(false);
			}

			auto dbconn = pool.back();
			pool.pop_back();
		//
		spinlock_.unlock();

		auto diff = std::difftime(std::time(nullptr), mktime(dbconn->last_released()));
		if (diff >= wait_timeout_sec && !dbconn->ping())
		{
			delete dbconn;
			return connect();
		}

		return dbconn;
	}

	void dbconn_pool::release_dbconn(mysqlpp::connection* dbconn)
	{
		if (!dbconn->is_pooled())
		{
			delete dbconn;
			return;
		}

		auto time_ = std::time(nullptr);
		dbconn->set_released(*std::localtime(&time_));

		spinlock_.lock();
		//
			pool.push_back(dbconn);
		//
		spinlock_.unlock();
	}

	mysqlpp::connection* dbconn_pool::connect(bool pooled_)
	{
		return new mysqlpp::connection(host.c_str(), userid.c_str(), passwd.c_str(), database.c_str(), port, charset.c_str(), pooled_);
	}

} // namespace waspp
