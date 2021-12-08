#include "LoadJSON.h"
#include <map>
using namespace std;

bool loadToLog(Logger*& lg, string jspath)
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
		int p;
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
						lg->SetNext(bufp, mp[bufs]);
						bufp = "";
						bufs = "";
					}
				}
				
				
			}
		}
		return true;
	}
	catch (JSONException exc)
	{
		return false;
	}
}
