#pragma once
#include <string>
#include <iostream>
#include <map>
#include <fstream>
using namespace std;

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
                oss.open(stream, std::fstream::app);
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
    Logger* SetNext(string &stream, int sev_lvl_ = INFORMATION)  {
        Stream* tmp = new Stream(stream, sev_lvl_);
        if (sstream)
            sstream->SetNext(tmp);
        else
            sstream = tmp;
        return this;
    }
    string Handle(string request, int severity, string data) {
        if (sstream)
            return sstream->Handle(request, severity, data);

        return {};
    }

    Logger() {
    }

    ~Logger() {
        if (sstream)
            delete sstream;
    }
};
