#include "QuickEncrypt.h"

QuickEncrypt::QuickEncrypt(void)
{
}

string QuickEncrypt::decode(string src,string key)
{
	if(src.length() <= 0){
		return src;
	}

	const regex pattern("(\\d+)");
	smatch matchResult;
	vector<int> list;
	
	const sregex_token_iterator end;
	for(sregex_token_iterator i(src.begin(),src.end(),pattern);i != end; ++i){
		int n = atoi(i->str().c_str());
		list.push_back(n);
	}

	string data;
	for(size_t i = 0; i < list.size(); i++){
		char c = list[i] - (0xff & key[i % key.size()]);
		data.append(1,c);
	}

	return data;
}

string QuickEncrypt::encode(string src,string key){

	string sb;
	for(size_t i = 0 ; i < src.size() ;i++){
		int n = (0xff & src[i]) + (0xff & key[i % key.length()]);
		stringstream thisGroup;
		thisGroup << "@" << n;
		string thisGroupString;
		thisGroup >> thisGroupString;
		sb.append(thisGroupString);
	}
	return sb;
}


QuickEncrypt::~QuickEncrypt(void)
{
}
