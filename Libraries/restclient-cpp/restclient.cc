/**
 * @file restclient.cpp
 * @brief implementation of the restclient class
 *
 * This just provides static wrappers around the Connection class REST
 * methods. However since I didn't want to have to pull in a whole URI parsing
 * library just now, the Connection constructor is passed an empty string and
 * the full URL is passed to the REST methods. All those methods to is
 * concatenate them anyways. So this should do for now.
 *
 * @author Daniel Schauenberg <d@unwiredcouch.com>
 */

#include "restclient-cpp/restclient.h"

#include <curl/curl.h>
#include <sstream>

#include "restclient-cpp/version.h"
#include "restclient-cpp/connection.h"


/**
 * @brief global init function. Call this before you start any threads.
 */
int RestClient::init() {
  CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
  if (res == CURLE_OK) {
    return 0;
  } else {
    return 1;
  }
}

/**
 * @brief global disable function. Call this before you terminate your
 * program.
 */
void RestClient::disable() {
  curl_global_cleanup();
}

/**
 * @brief HTTP GET method
 *
 * @param url to query
 *
 * @return response struct
 */
RestClient::Response RestClient::get(const std::string& url,
	HeaderFields &headers, int nTimeOut, bool bVerifyPeer) {
  RestClient::Response ret;
  RestClient::Connection *conn = new RestClient::Connection("");
  conn->SetTimeout(nTimeOut);
  conn->setVerifyPeer(bVerifyPeer);

  RestClient::HeaderFields::iterator iIterator = headers.begin();
  while (iIterator != headers.end()) {
	  conn->AppendHeader(iIterator->first, iIterator->second);

	  iIterator++;
  }

  ret = conn->get(url);
  delete conn;
  return ret;
}

/**
 * @brief HTTP POST method
 *
 * @param url to query
 * @param ctype content type as string
 * @param data HTTP POST body
 *
 * @return response struct
 */
RestClient::Response RestClient::post(const std::string& url,
                                      const std::string& ctype,
                                      const std::string& data,
									  HeaderFields &headers, int nTimeOut, bool bVerifyPeer)
{
	RestClient::Response ret;
  RestClient::Connection *conn = new RestClient::Connection("");
  conn->AppendHeader("Content-Type", ctype);
  conn->SetTimeout(nTimeOut);
  conn->setVerifyPeer(bVerifyPeer);

  RestClient::HeaderFields::iterator iIterator = headers.begin ();
  while (iIterator != headers.end()) {
	  conn->AppendHeader(iIterator->first, iIterator->second);

	  iIterator++;
  }


  ret = conn->post(url, data);
  delete conn;
  return ret;
}


RestClient::Response RestClient::putbinary(const std::string& url,
	const std::string& ctype,
	unsigned char * data,
	unsigned int datasize,
	HeaderFields &headers,
	RestClient::ConnectionProgressCallback pProgressCallback,
	void * pProgressUserData, int nTimeOut, bool bVerifyPeer)
{
	RestClient::Response ret;
	RestClient::Connection *conn = new RestClient::Connection("");
	conn->AppendHeader("Content-Type", ctype);
	conn->setProgressCallback(pProgressCallback, pProgressUserData);
	conn->SetTimeout(nTimeOut);
	conn->setVerifyPeer(bVerifyPeer);

	RestClient::HeaderFields::iterator iIterator = headers.begin();
	while (iIterator != headers.end()) {
		conn->AppendHeader(iIterator->first, iIterator->second);

		iIterator++;
	}


	ret = conn->putbinary(url, data, datasize);
	delete conn;
	return ret;
}


/**
 * @brief HTTP PUT method
 *
 * @param url to query
 * @param ctype content type as string
 * @param data HTTP PUT body
 *
 * @return response struct
 */
RestClient::Response RestClient::put(const std::string& url,
                                     const std::string& ctype,
                                     const std::string& data,
										HeaderFields &headers, int nTimeOut, bool bVerifyPeer) {
  RestClient::Response ret;
  RestClient::Connection *conn = new RestClient::Connection("");
  conn->AppendHeader("Content-Type", ctype);
  conn->SetTimeout(nTimeOut);
  conn->setVerifyPeer(bVerifyPeer);

  RestClient::HeaderFields::iterator iIterator = headers.begin();
  while (iIterator != headers.end()) {
	  conn->AppendHeader(iIterator->first, iIterator->second);

	  iIterator++;
  }

  ret = conn->put(url, data);
  delete conn;
  return ret;
}

/**
 * @brief HTTP DELETE method
 *
 * @param url to query
 *
 * @return response struct
 */
RestClient::Response RestClient::del(const std::string& url, bool bVerifyPeer) {
  RestClient::Response ret;
  RestClient::Connection *conn = new RestClient::Connection("");
  conn->setVerifyPeer(bVerifyPeer);
  ret = conn->del(url);
  delete conn;
  return ret;
}

RestClient::Response RestClient::delwithauth(const std::string& url, HeaderFields &headers, bool bVerifyPeer) {
	RestClient::Response ret;
	RestClient::Connection *conn = new RestClient::Connection("");

	RestClient::HeaderFields::iterator iIterator = headers.begin();
	while (iIterator != headers.end()) {
		conn->AppendHeader(iIterator->first, iIterator->second);

		iIterator++;
	}
	conn->setVerifyPeer(bVerifyPeer);
	ret = conn->del(url);
	delete conn;
	return ret;
}

RestClient::Response RestClient::patch(const std::string &url, const std::string &content_type, const std::string &data, HeaderFields &headers, bool bVerifyPeer) {
	RestClient::Response ret; 
	RestClient::Connection *conn = new RestClient::Connection("");

	conn->AppendHeader("Content-Type", content_type);

	RestClient::HeaderFields::iterator iIterator = headers.begin();
	while (iIterator != headers.end()) {
		conn->AppendHeader(iIterator->first, iIterator->second);

		iIterator++;
	}
	conn->setVerifyPeer(bVerifyPeer);
	ret = conn->patch(url, data);
	delete conn;
	return ret;
}
