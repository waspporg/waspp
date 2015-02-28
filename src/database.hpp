/*
Copyright (c) 2014-2015 WASPP (waspp.org at gmail dot com)

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef WASPP_DATABASE_HPP
#define WASPP_DATABASE_HPP

#include <map>
#include <vector>
#include <string>

#include "utility.hpp"
#include "config.hpp"
#include "dbconn_pool.hpp"

namespace waspp
{

	typedef boost::shared_ptr<dbconn_pool> dbpool_ptr;

	class database
		: public utility::singleton<database>
	{
	public:
		database();
		~database();

		bool init(config* cfg, const std::vector<std::string>& dbkeys);

		/*
		template<typename T>
		dbconn_ptr get(T dbkey)
		{
			dbpool_ptr dbpool = get_dbpool(dbkey);
			return dbpool->get_dbconn();
		}
		*/

		dbconn_ptr get(const std::string& dbkey)
		{
			dbpool_ptr dbpool = get_dbpool(dbkey);
			return dbpool->get_dbconn();
		}

		dbconn_ptr get_shard(unsigned int shard_key)
		{
			dbpool_ptr dbpool = get_dbpool(shard_key);
			return dbpool->get_dbconn();
		}

		/*
		template<typename T>
		void free(T dbkey, dbconn_ptr dbconn)
		{
			dbpool_ptr dbpool = get_dbpool(dbkey);
			dbpool->free_dbconn(dbconn);
		}
		*/

		void free(const std::string& dbkey, dbconn_ptr dbconn)
		{
			dbpool_ptr dbpool = get_dbpool(dbkey);
			dbpool->free_dbconn(dbconn);
		}

		void free_shard(unsigned int shard_key, dbconn_ptr dbconn)
		{
			dbpool_ptr dbpool = get_dbpool(shard_key);
			dbpool->free_dbconn(dbconn);
		}

	private:
		dbpool_ptr get_dbpool(const std::string& dbkey);
		dbpool_ptr get_dbpool(unsigned int dbkey);

		unsigned int shard_count;
		std::string shard_format;

		std::map<std::string, dbpool_ptr> db_;

	};

} // namespace waspp

#endif // WASPP_DATABASE_HPP
