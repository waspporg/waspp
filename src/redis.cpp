/*
Copyright (c) 2014-present WASPP (waspp.org at gmail dot com)

Distributed under the Boost Software License, Version 1.0.
http://www.boost.org/LICENSE_1_0.txt
*/

#include <boost/crc.hpp>

#include "redis.hpp"
#include "locator.hpp"

namespace waspp
{

	redis::redis()
	{
		
	}

	redis::~redis()
	{
		for (auto& r : rd_)
		{
			delete r;
		}
	}

	bool redis::init(const std::vector<std::string>& rdnames)
	{
		try
		{
			auto cfg = locator::cfg();
			auto cfg_rd_shard = cfg->get("rd_shard");

			std::vector<std::string> keys = { "rd_shard_count", "rd_shard_format" };
			for (auto& key : keys)
			{
				auto found = cfg_rd_shard.find(key);
				if (found == cfg_rd_shard.end())
				{
					return false;
				}

				if (key == "rd_shard_count")
				{
					rd_shard_count = strtoul(found->second.c_str(), nullptr, 0);
				}
				else if (key == "rd_shard_format")
				{
					rd_shard_format = found->second;
				}
			}

			for (auto& rdname : rdnames)
			{
				auto rdpool = new redis_pool();

				if (!rdpool->init_pool(cfg->get(rdname)) || !rdpool->fill_pool())
				{
					delete rdpool;
					return false;
				}

				rd_.push_back(rdpool);
			}

			return true;
		}
		catch (...)
		{
			throw;
		}

		return false;
	}

	redis_pool& redis::get_rdpool(rdname_type rdname)
	{
		return *rd_[(int)rdname];
	}

	redis_pool& redis::get_rdpool(uint64_t shard_key)
	{
		return *rd_[shard_key % rd_shard_count];
	}

	redis_pool& redis::get_rdpool(const std::string& shard_key)
	{
		boost::crc_32_type crc32;
		crc32.process_bytes(shard_key.c_str(), shard_key.size());
		
		return *rd_[crc32.checksum() % rd_shard_count];
	}

	scoped_rd::scoped_rd(rdname_type rdname)
		: rdpool(locator::rd()->get_rdpool(rdname)),
		conn(rdpool.get_rdconn())
	{
		
	}

	scoped_rd::scoped_rd(uint64_t shard_key)
		: rdpool(locator::rd()->get_rdpool(shard_key)),
		conn(rdpool.get_rdconn())
	{
		
	}

	scoped_rd::scoped_rd(const std::string& shard_key)
		: rdpool(locator::rd()->get_rdpool(shard_key)),
		conn(rdpool.get_rdconn())
	{
		
	}

	scoped_rd::~scoped_rd()
	{
		rdpool.release_rdconn(conn);
	}

} // namespace waspp
