//
// Copyright (c) 2014-2015 WASPP (waspp.org at gmail dot com)
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <string>

#include "logger.hpp"
#include "config.hpp"
#include "database.hpp"
#include "server.hpp"

int main(int argc, char* argv[])
{
	waspp::logger* log = waspp::logger::instance();
	waspp::config* cfg = waspp::config::instance();
	waspp::database* db = waspp::database::instance();

	try
	{
		if (argc != 3)
		{
			std::cerr << "Usage: ./waspp develop 00 &\n";
			return 1;

			argv[1] = "develop";
			argv[2] = "00";
		}

		std::string phase(argv[1]), server_seq(argv[2]);
		std::string log_file, cfg_file, server_id;
		{
			log_file.append("../log/");
			log_file.append(phase);
			log_file.append(server_seq);
			log_file.append(".csv");

			cfg_file.append("../cfg/");
			cfg_file.append(phase);
			cfg_file.append(".json");

			server_id.append("server");
			server_id.append(server_seq);
		}

		log->file(log_file);

		if (!cfg->init(cfg_file, server_id))
		{
			log->fatal("config::init failed");
			return 1;
		}

		if (!log->init(phase, cfg->level, cfg->rotation))
		{
			log->fatal("logger::init failed");
			return 1;
		}

		std::vector<std::string> dbnames;
		{
			dbnames.push_back("db_index");
			dbnames.push_back("db_000");
			dbnames.push_back("db_001");
			dbnames.push_back("db_etc");
		}

		if (!db->init(cfg, dbnames))
		{
			log->fatal("database::init failed");
			return 1;
		}

		log->info("waspp starting..");

		waspp::server s(cfg->address, cfg->port, cfg->doc_root, cfg->num_threads);
		s.run();

		log->info("waspp stopped");
	}
	catch (std::exception& e)
	{
		log->fatal(e.what());
		return 1;
	}

	return 0;
}
