//
// Copyright (c) 2014-2015 WASPP (waspp.org@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef WASPP_dbconn_pool_HPP
#define WASPP_dbconn_pool_HPP

#include <ctime>

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "spinlock.hpp"
#include "mysqlpp.hpp"

namespace waspp
{
	typedef boost::shared_ptr<mysqlpp::connection> dbconn_ptr;

	typedef boost::scoped_ptr<mysqlpp::statement> statement_ptr;
	typedef boost::scoped_ptr<mysqlpp::result> result_ptr;

	class dbconn_pool
		: private boost::noncopyable
	{
	public:
		dbconn_pool();
		~dbconn_pool();

		bool init_pool(const std::map<std::string, std::string>& cfg);
		bool fill_pool();

		dbconn_ptr get_dbconn();
		void free_dbconn(dbconn_ptr dbconn);

	private:
		dbconn_ptr connect(bool pooled_ = true);
		bool validate(dbconn_ptr dbconn);

		std::string host, userid, passwd, database;
		unsigned int port;

		std::size_t pool_size;
		double timeout_sec;

		std::vector<dbconn_ptr> pool;
		spinlock lock;

	};

} // namespace waspp

#endif // WASPP_dbconn_pool_HPP
