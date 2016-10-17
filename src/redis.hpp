/*
Copyright (c) 2014-present WASPP (waspp.org at gmail dot com)

Distributed under the Boost Software License, Version 1.0.
http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef redis_hpp
#define redis_hpp

#include "redis_pool.hpp"
#include "singleton.hpp"
#include "config.hpp"

namespace waspp
{

	class redis
		: public singleton<redis>
	{
	public:
		redis();
		~redis();

		bool init(config* cfg, const std::vector<std::string>& rdnames);

		redis_pool* get_rdpool(const std::string& rdname);
		redis_pool* get_rdpool(unsigned long long int shard_key);

	private:
		unsigned int rd_shard_count;
		std::string rd_shard_format;

		std::unordered_map<std::string, redis_pool*> rd_;

	};

	class scoped_rd
	{
	public:
		scoped_rd(redis* rd, const std::string& rdname);
		scoped_rd(redis* rd, unsigned long long int shard_key);

		~scoped_rd();

		redis3m::reply run(const std::vector<std::string>& args);

	private:
		redis_pool* rdpool;
		redis3m::connection* rdconn;

	};

} // namespace waspp

#endif // redis_hpp
