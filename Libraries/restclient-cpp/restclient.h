/**
 * @file restclient.h
 * @brief libcurl wrapper for REST calls
 * @author Daniel Schauenberg <d@unwiredcouch.com>
 * @version
 * @date 2010-10-11
 */

#ifndef INCLUDE_RESTCLIENT_CPP_RESTCLIENT_H_
#define INCLUDE_RESTCLIENT_CPP_RESTCLIENT_H_

#include "../curl/curl.h"
#include <string>
#include <vector>
#include <map>
#include <cstdlib>

#include "../restclient-cpp/version.h"

/**
 * @brief namespace for all RestClient definitions
 */
namespace RestClient {

	typedef int (*ConnectionProgressCallback) (curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow, void * pUserData);

/**
  * public data definitions
  */
typedef std::map<std::string, std::string> HeaderFields;

/** @struct Response
  *  @brief This structure represents the HTTP response data
  *  @var Response::code
  *  Member 'code' contains the HTTP response code
  *  @var Response::body
  *  Member 'body' contains the HTTP response body
  *  @var Response::headers
  *  Member 'headers' contains the HTTP response headers
  */
typedef struct {
  int code;
  std::string body;
	std::vector<char> rawbody;
  HeaderFields headers;
} Response;

// init and disable functions
int init();
void disable();

/**
  * public methods for the simple API. These don't allow a lot of
  * configuration but are meant for simple HTTP calls.
  *
  */
Response get(const std::string& url, 
	HeaderFields &headers, int nTimeOut, bool bVerifyPeer);
Response post(const std::string& url,
              const std::string& content_type,
              const std::string& data,
			  HeaderFields &headers, int nTimeOut, bool bVerifyPeer);

Response put(const std::string& url,
              const std::string& content_type,
              const std::string& data,
			HeaderFields &headers, int nTimeOut, bool bVerifyPeer);
Response putbinary(const std::string& url,
	const std::string& ctype,
	unsigned char * data,
	unsigned int datasize,
	HeaderFields &headers,
	ConnectionProgressCallback pProgressCallback, 
	void * pProgressUserData, int nTimeOut, bool bVerifyPeer);
Response del(const std::string& url, bool bVerifyPeer);
Response delwithauth(const std::string& url, HeaderFields &headers, bool bVerifyPeer);
Response patch(const std::string &url, const std::string &content_type, const std::string &data, HeaderFields &headers, bool bVerifyPeer);

};  // namespace RestClient

#endif  // INCLUDE_RESTCLIENT_CPP_RESTCLIENT_H_
