/*
Copyright (c) 2014-present WASPP (waspp.org at gmail dot com)

Distributed under the Boost Software License, Version 1.0.
http://www.boost.org/LICENSE_1_0.txt
*/

#include <cstdint>

#include "router.hpp"
#include "database.hpp"
#include "utility.hpp"
#include "session.hpp"
#include "performance_checker.hpp"

namespace waspp
{
	
	namespace dir_user
	{

		void signin_html(config& cfg, request& req, response& res)
		{
			router::get_file(cfg, res, "dir_include_header.html");
			router::get_file(cfg, res, "dir_user_signin.html");
			router::get_file(cfg, res, "dir_include_footer.html");
		}

		void auth(config& cfg, request& req, response& res)
		{
			performance_checker c(50, __FILE__, __LINE__);

			if (req.param("username").empty())
			{
				router::err_msg(cfg, res, err_username_required);
				return;
			}

			if (req.param("passwd").empty())
			{
				router::err_msg(cfg, res, err_passwd_required);
				return;
			}
			std::string passwd(md5_digest(req.param("passwd")));

			scoped_db db_shard(req.param("userid"));
			stmt_ptr stmt(db_shard.conn->prepare("SELECT userid FROM users WHERE username = ?"));
			//
				stmt->param(req.param("username"));
			//

			rs_ptr rs(stmt->query());
			if (rs->num_rows() == 0)
			{
				router::err_msg(cfg, res, err_username_not_found);
				return;
			}

			uint64_t userid = 0;
			if (rs->fetch())
			{
				userid = rs->get<uint64_t>("userid");
			}

			stmt.reset(db_shard.conn->prepare("SELECT userid FROM users WHERE userid = ? AND passwd = ?"));
			//
				stmt->param(userid);
				stmt->param(passwd);
			//

			rs.reset(stmt->query());
			if (rs->num_rows() == 0)
			{
				router::err_msg(cfg, res, err_auth_failed);
				return;
			}

			waspp::session sess(cfg, req, res);
			//
				sess.put("userid", std::to_string(userid));
				sess.put("username", req.param("username"));
			//

			res.redirect_to("/dir/forum/index");
		}

		void signup_html(config& cfg, request& req, response& res)
		{
			router::get_file(cfg, res, "dir_include_header.html");
			router::get_file(cfg, res, "dir_user_signup.html");
			router::get_file(cfg, res, "dir_include_footer.html");
		}

		void post(config& cfg, request& req, response& res)
		{
			performance_checker c(50, __FILE__, __LINE__);

			if (req.param("platformid").empty())
			{
				router::err_msg(cfg, res, err_platformid_required);
				return;
			}

			if (req.param("platformtype").empty())
			{
				router::err_msg(cfg, res, err_platformtype_required);
				return;
			}

			if (req.param("username").empty())
			{
				router::err_msg(cfg, res, err_username_required);
				return;
			}

			if (req.param("passwd").empty())
			{
				router::err_msg(cfg, res, err_passwd_required);
				return;
			}

			if (req.param("passwd") != req.param("retypepasswd"))
			{
				router::err_msg(cfg, res, err_passwds_not_identical);
				return;
			}
			std::string md5_passwd = md5_digest(req.param("passwd"));

			scoped_db db_shard(req.param("username"));
			stmt_ptr stmt(db_shard.conn->prepare("SELECT userid FROM users WHERE username = ?"));
			//
				stmt->param(req.param("username"));
			//

			rs_ptr rs(stmt->query());
			if (rs->num_rows() != 0)
			{
				router::err_msg(cfg, res, err_username_not_available);
				return;
			}

			stmt.reset(db_shard.conn->prepare("CALL USP_GET_UNIQUE_KEYS('users', ?)"));
			//
				stmt->param(1);
			//
			rs.reset(stmt->query());

			uint64_t userid = 0;
			if (rs->fetch(true))
			{
				userid = rs->get<uint64_t>("last_key");
			}
			std::string platformid = std::to_string(userid);

			stmt.reset(db_shard.conn->prepare("INSERT INTO users(userid, platformid, username, passwd, platformtype, inserttime, updatetime) VALUES(?, ?, ?, ?, ?, NOW(), NOW())"));
			//
				stmt->param(userid);
				stmt->param(platformid);
				stmt->param(req.param("username"));
				stmt->param(md5_passwd);
				stmt->param(strtol(req.param("platformtype").c_str(), nullptr, 0));
			//

			uint64_t affected_rows = stmt->execute();
			if (affected_rows == 0)
			{
				router::err_msg(cfg, res, err_users_insert_failed);
				return;
			}

			res.redirect_to("/dir/user/signin");
		}

		void signout(config& cfg, request& req, response& res)
		{
			res.delete_cookie(cfg.sess_cookie());
			res.redirect_to("/dir/user/signin");
		}

	} // namespace dir_user

} // namespace waspp
