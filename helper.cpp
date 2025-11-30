#include <iostream>
#include "header.h"
using namespace std;


// string cookie_str = req.get_header_value("Session
string getValueFromCookie(const string cookie_str, const string name) {
	string prefix = name + "=";

	size_t start_of_pair = cookie_str.find(name);

	if (start_of_pair == string::npos)
		return string{};

	size_t start_of_value = start_of_pair + prefix.size();
	size_t end_of_both = cookie_str.find(";", start_of_pair);
	return cookie_str.substr(start_of_value, end_of_both - start_of_value);
};


int main_() {
	string test_cookie{ "session_id=abcdef; preferences=null" };

	string id = getValueFromCookie(test_cookie, string{ "session_id" });
	string pref = getValueFromCookie(test_cookie, string{ "preferences" });

	cout << (id == "abcdef") << (pref == "null");
	return 0;
}
