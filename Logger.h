#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <exception>

using namespace std;

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

class LogBuilder {
public:
    vector<string> fname;
    vector<int> severity;

    LogBuilder* add_next(string& stream, int sev_lvl_ = 2)
    {
        fname.push_back(stream);
        severity.push_back(sev_lvl_);
        return this;
    }

    LogBuilder* load_from_json(string jspath)
    {
		try {
			ifstream js(jspath);
			map<string, int> mp;
			mp["trace"] = 0;
			mp["debug"] = 1;
			mp["information"] = 2;
			mp["warning"] = 3;
			mp["error"] = 4;
			mp["fatal"] = 5;

			int br_count = 0;
			char tmp;
			string buf = "", bufp = "", bufs = "";
			while (js)
			{
				buf = "";
				js.get(tmp);
				if (js)
				{
					if (tmp == '"')
					{
						js.get(tmp);
						while (js and tmp != '"')
						{
							buf += tmp;
							js.get(tmp);
						}
						if (!js)
							throw JSONException("end");
						js.get(tmp);
						if (br_count == 2)
						{
							if (buf == "path")
							{
								if (bufp != "")
									throw JSONException("repeat");
								while (js and tmp != ':')
								{
									if (!isspace(tmp))
										throw JSONException("end");
									js.get(tmp);
								}
								js.get(tmp);
								while (js and tmp != '"')
								{
									buf += tmp;
									js.get(tmp);
								}
								if (!js)
									throw JSONException("end");
								if (tmp == '"')
								{
									js.get(tmp);
									while (js and tmp != '"')
									{
										bufp += tmp;
										js.get(tmp);
									}
									if (!js)
										throw JSONException("end");
									js.get(tmp);
								}
							}
							else if (buf == "severity")
							{
								if (bufs != "")
									throw JSONException("repeat");
								while (js and tmp != ':')
								{
									if (!isspace(tmp))
										throw JSONException("end");
									js.get(tmp);
								}
								js.get(tmp);
								while (js and tmp != '"')
								{
									buf += tmp;
									js.get(tmp);
								}
								if (!js)
									throw JSONException("end");
								if (tmp == '"')
								{
									js.get(tmp);
									while (js and tmp != '"')
									{
										bufs += tmp;
										js.get(tmp);
									}
									if (mp.find(bufs) == mp.end())
										throw JSONException("badSeverity");
									if (!js)
										throw JSONException("end");
									js.get(tmp);
								}
							}
							else
								throw JSONException("badChoice");
						}

					}
					if (tmp == '{') {
						br_count++;
						if (br_count > 2)
							throw JSONException("overInsert");
					}
					if (tmp == '}')
					{
						br_count--;
						if (br_count < 0)
							throw JSONException("overRemove");
						if (br_count == 1)
						{
							if (bufp == "")
								throw JSONException("noPath");
							int sss = 2;
							if (bufs != "")
								sss = mp[bufs];
							fname.push_back(bufp);
							severity.push_back(mp[bufs]);
							bufp = "";
							bufs = "";
						}
					}


				}
			}
			return this;
		}
		catch (JSONException exc)
		{
			cout << exc.what();
			exit(-1);
			return nullptr;
		}
    }
};


class Logger {
public:
    static enum severity
    {
        TRACE,
        DEBUG,
        INFORMATION,
        WARNING,
        ERROR,
        FATAL
    };
private:
    class Stream {
    public:
        string list[6] = { "trace", "debug", "information", "warning", "error", "fatal" };
        Stream* next_stream;
        string oss_name;
        ofstream oss;
        int sev_lvl;
        void SetNext(Stream* stream) {
            if (next_stream != nullptr)
                next_stream->SetNext(stream);
            else
                next_stream = stream;
        }
        string Handle(string request, int sev, string data) {
            if (sev >= sev_lvl)
                if (oss_name != "stdout")
                    oss << "[" << data << "]" << "[" << list[sev] << "]" << ":" << request << endl;
                else
                    cout << "[" << data << "]" << "[" << list[sev] << "]" << ":" << request << endl;
            if (this->next_stream) {
                return this->next_stream->Handle(request, sev, data);
            }

            return {};
        }
        Stream(string &stream, int sev_lvl_ = INFORMATION): sev_lvl(sev_lvl_), oss_name(stream),next_stream(nullptr){
            if (stream != "stdout")
                oss.open(stream, fstream::app);
        }
        ~Stream(){
            if (next_stream)
                delete next_stream;
            if (oss_name != "stdout")
                oss.close();
        }
    };

    Stream* sstream = nullptr;
    
public:
    string Handle(string request, int severity, string data) {
        if (sstream)
            return sstream->Handle(request, severity, data);

        return {};
    }

    Logger(LogBuilder* builder) {
		for (unsigned int i = 0; i < builder->fname.size(); i++)
		{
			auto tmp = new Stream(builder->fname[i], builder->severity[i]);
			if (sstream)
				sstream->SetNext(tmp);
			else
				sstream = tmp;
		}
    }

    ~Logger() {
        if (sstream)
            delete sstream;
    }
};
