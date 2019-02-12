#include "httpservice.hpp"

using namespace automaton;
using namespace detail;

std::string HttpBase::get_canned_response (int code)
{
	try
	{
		return _canned.at (code);
	}
	catch(...)
	{}

	return _canned.at (500);
}

const std::unordered_map<int, std::string> HttpBase::_canned =
{
	{400, "HTTP/1.0 400 Bad Request\r\n\r\n"},
	{401, "HTTP/1.0 401 Unauthorized\r\n\r\n"},
	{402, "HTTP/1.0 402 Payment Required\r\n\r\n"},
	{403, "HTTP/1.0 403 Forbidden\r\n\r\n"},
	{404, "HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 49\r\n\r\n<html><body><h1>404 Not Found.</h1></body></html>"},
	{405, "HTTP/1.0 405 Method Not Allowed\r\n\r\n"},	

	{500, "HTTP/1.0 500 Internal Server Error\r\n\r\n"},
	{501, "HTTP/1.0 501 Not Implemented\r\n\r\n"},
	{502, "HTTP/1.0 502 Bad Gateway\r\n\r\n"},
	{503, "HTTP/1.0 503 Service Unavailable\r\n\r\n"},
	{504, "HTTP/1.0 504 Gateway Timeout\r\n\r\n"},
	{505, "HTTP/1.0 505 HTTP Version Not Supported\r\n\r\n"}

};
