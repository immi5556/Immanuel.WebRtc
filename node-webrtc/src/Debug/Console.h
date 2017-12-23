#ifndef _H_CONSOLE_
#define _H_CONSOLE_

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

class Console
{
private:
	struct LogHelper
	{
	public:
		LogHelper(char* func, char* file, int line) :
				func(func), file(file), line(line)
		{
			std::size_t backslash = this->file.find_last_of('\\');
			std::size_t slash = this->file.find_last_of('/');

			this->file = ".../" + this->file.substr(std::min(backslash, slash) + 1);
			this->func = this->func.substr(this->func.find_last_of("::") + 1);

			std::cout << "[" << this->func << "() @ " << this->file << ":" << this->line << "] => ";
		}
		~LogHelper(void)
		{
			std::cout << std::endl;
		}

	public:
		template<class T>
		LogHelper& operator <<(T val)
		{
			std::cout << val;
			return (*this);
		}

		template<class T>
		LogHelper& operator <<(T* val)
		{
			std::cout << val;
			return (*this);
		}

	private:
		std::string func;
		std::string file;
		int line;
	};

public:
	static Console& getInstance() {
		static Console instance;
		return instance;
	}

public:
	LogHelper log(char* func, char* file, int line) {
		return LogHelper(func, file, line);
	}
};

#define LOG() Console::getInstance().log(__FUNCTION__, __FILE__, __LINE__)

#endif