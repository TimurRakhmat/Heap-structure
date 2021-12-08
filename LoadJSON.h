#pragma once
#include "Logger.h"
#include <fstream>
#include <iostream>
#include <exception>

bool loadToLog(Logger*& lg, string jspath);

class JSONException : public exception
{
private:
	string error_text;

public:
	JSONException(const string& _error_text) {
		error_text = _error_text;
	}
	const char* what() const noexcept {
		return error_text.c_str();
	}
};