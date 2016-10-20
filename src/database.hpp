/*
Copyright (c) 2014-present WASPP (waspp.org at gmail dot com)

Distributed under the Boost Software License, Version 1.0.
http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef database_hpp
#define database_hpp

#include <cstdint>

#include "dbconn_pool.hpp"
#include "locator.hpp"
#include "config.hpp"

namespace waspp
{
	
	enum dbname_type : int
	{
		db_etc = 0,
	};

	class database
		: public locator<database>
	{
	public:
		database();
		~database();

		bool init(config& cfg, const std::vector<std::string>& dbnames);

		dbconn_pool& get_dbpool(dbname_type dbname);
		dbconn_pool& get_dbpool(uint64_t shard_key);
		dbconn_pool& get_dbpool(const std::string& shard_key);

	private:
		unsigned int db_shard_count;
		std::string db_shard_format;

		std::vector<dbconn_pool*> db_;

	};

	class scoped_db
	{
	public:
		scoped_db(dbname_type dbname);
		scoped_db(uint64_t shard_key);
		scoped_db(const std::string& shard_key);

		~scoped_db();

	private:
		dbconn_pool& dbpool;
		
	public:
		mysqlpp::connection* conn;

	};

} // namespace waspp

#endif // database_hpp
