/*
Copyright (c) 2014-present WASPP (waspp.org at gmail dot com)

Distributed under the Boost Software License, Version 1.0.
http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef redis_hpp
#define redis_hpp

#include "redis_pool.hpp"
#include "locator.hpp"
#include "config.hpp"

namespace waspp
{

	enum rdname_type
	{
		rd_rnk = 999,
	};

	class redis
		: public locator<redis>
	{
	public:
		redis();
		~redis();

		bool init(config& cfg, const std::unordered_map<int, std::string>& rdnames);

		redis_pool& get_rdpool(rdname_type rdname);
		redis_pool& get_rdpool(uint64_t shard_key);
		redis_pool& get_rdpool(const std::string& shard_key);

	private:
		unsigned int rd_shard_count;
		std::string rd_shard_format;

		std::unordered_map<int, redis_pool*> rd_;

	};

	class scoped_rd
	{
	public:
		scoped_rd(const std::string& shard_key);
		scoped_rd(uint64_t shard_key);

		~scoped_rd();

		redis3m::connection* conn;

	private:
		redis_pool& rdpool;

	};

} // namespace waspp

#endif // redis_hpp
