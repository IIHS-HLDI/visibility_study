#pragma once

#include "Config.h"

class SessionManager
{
	SessionConfig* session_config;

public:
	SessionConfig get_session();

	SessionConfig load_session(const char* filepath);

	void save_session(const char* filepath);
};

