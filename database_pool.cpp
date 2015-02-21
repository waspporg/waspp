//
// Copyright (c) 2014-2015 WASPP (waspp.org@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/lexical_cast.hpp>

#include "database_pool.hpp"

namespace waspp
{

	database_pool::database_pool()
		: port(0), pool_size(0), timeout_sec(0), pool(0)
	{
	}

	database_pool::~database_pool()
	{
	}

	bool database_pool::init_pool(std::map<std::string, std::string>* c)
	{
		if (c == 0)
		{
			return false;
		}

		std::vector<std::string> keys;
		{
			keys.push_back("host");
			keys.push_back("userid");
			keys.push_back("passwd");
			keys.push_back("database");
			keys.push_back("port");
			keys.push_back("pool_size");
			keys.push_back("timeout_sec");
		}

		for (std::size_t i = 0; i < keys.size(); ++i)
		{
			if (c->find(keys[i]) == c->end())
			{
				return false;
			}
		}

		host = c->at("host");
		userid = c->at("userid");
		passwd = c->at("passwd");
		database = c->at("database");

		port = boost::lexical_cast<unsigned int>(c->at("port"));
		pool_size = boost::lexical_cast<std::size_t>(c->at("pool_size"));
		timeout_sec = boost::lexical_cast<double>(c->at("timeout_sec"));

		return true;
	}

	bool database_pool::fill_pool()
	{
		if (pool_size == 0)
		{
			return false;
		}

		lock.set();
		{
			for (std::size_t i = 0; i < pool_size; ++i)
			{
				database_ptr db = connect();
				if (!validate(db))
				{
					lock.clear();
					return false;
				}

				pool.push_back(db);
			}
		}
		lock.clear();

		return true;
	}

	database_ptr database_pool::acquire_connection()
	{
		lock.set();
		//{
			if (pool.empty())
			{
				lock.clear();
				return connect(false);
			}

			database_ptr db = *(pool.end() - 1);
			pool.pop_back();
		//}
		lock.clear();

		std::time_t time_ = std::time(0);
		double diff = std::difftime(time_, mktime(db->last_released()));

		if (diff > timeout_sec && !validate(db))
		{
			return connect();
		}

		return db;
	}

	void database_pool::release_connection(database_ptr db)
	{
		if (!db->is_pooled())
		{
			return;
		}

		std::time_t time_ = std::time(0);
		db->set_released(*std::localtime(&time_));

		lock.set();
		{
			pool.push_back(db);
		}
		lock.clear();
	}

	database_ptr database_pool::connect(bool pooled_)
	{
		return database_ptr(new mysqlpp::connection(host.c_str(), userid.c_str(), passwd.c_str(), database.c_str(), port, pooled_));
	}

	bool database_pool::validate(database_ptr db)
	{
		try
		{
			boost::scoped_ptr<mysqlpp::statement> stmt(db->prepare("DO 0"));
			stmt->execute();

			return true;
		}
		catch (...)
		{

		}

		return false;
	}

} // namespace waspp
