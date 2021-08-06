/**
 * @file connection.cpp
 * @brief implementation of the connection class
 * @author Daniel Schauenberg <d@unwiredcouch.com>
 */

#include "restclient-cpp/connection.h"

#include <curl/curl.h>

#include <cstring>
#include <string>
#include <iostream>
#include <map>
#include <stdexcept>

#include "restclient-cpp/restclient.h"
#include "restclient-cpp/helpers.h"
#include "restclient-cpp/version.h"



/**
 * @brief constructor for the Connection object
 *
 * @param baseUrl - base URL for the connection to use
 *
 */
RestClient::Connection::Connection(const std::string baseUrl)
                               : lastRequest(), headerFields() {
  this->curlHandle = curl_easy_init();
  if (!this->curlHandle) {
    throw std::runtime_error("Couldn't initialize curl handle");
  }

	curl_easy_setopt(this->curlHandle, CURLOPT_SSL_ENABLE_ALPN, 0);

  this->baseUrl = baseUrl;
  this->timeout = 0;
  this->followRedirects = false;

  this->m_pProgressCallback = 0;
  this->m_pProgressUserData = 0;
}

RestClient::Connection::~Connection() {
  if (this->curlHandle) {
    curl_easy_cleanup(this->curlHandle);
  }
}


void RestClient::Connection::setProgressCallback(ConnectionProgressCallback pCallback, void * pUserData)
{
	m_pProgressCallback = pCallback;
	m_pProgressUserData = pUserData;
}

RestClient::ConnectionProgressCallback RestClient::Connection::getProgressCallback()
{
	return m_pProgressCallback;
}

void * RestClient::Connection::getProgressUserData()
{
	return m_pProgressUserData;
}



// getters/setters

/**
 * @brief get diagnostic information about the connection object
 *
 * @return RestClient::Connection::Info struct
 */
RestClient::Connection::Info
RestClient::Connection::GetInfo() {
  RestClient::Connection::Info ret;
  ret.baseUrl = this->baseUrl;
  ret.headers = this->GetHeaders();
  ret.timeout = this->timeout;
  ret.basicAuth.username = this->basicAuth.username;
  ret.basicAuth.password = this->basicAuth.password;
  ret.customUserAgent = this->customUserAgent;
  ret.lastRequest = this->lastRequest;

  return ret;
}

/**
 * @brief append a header to the internal map
 *
 * @param key for the header field
 * @param value for the header field
 *
 */
void
RestClient::Connection::AppendHeader(const std::string& key,
                                     const std::string& value) {
  this->headerFields[key] = value;
}

/**
 * @brief set the custom headers map. This will replace the currently
 * configured headers with the provided ones. If you want to add additional
 * headers, use AppendHeader()
 *
 * @param headers to set
 */
void
RestClient::Connection::SetHeaders(RestClient::HeaderFields headers) {
  this->headerFields = headers;
}

/**
 * @brief get all custom headers set on the connection
 *
 * @returns a RestClient::HeaderFields map containing the custom headers
 */
RestClient::HeaderFields
RestClient::Connection::GetHeaders() {
  return this->headerFields;
}

/**
 * @brief configure whether to follow redirects on this connection
 *
 * @param follow - boolean whether to follow redirects
 */
void
RestClient::Connection::FollowRedirects(bool follow) {
  this->followRedirects = follow;
}

/**
 * @brief set custom user agent for connection. This gets prepended to the
 * default restclient-cpp/RESTCLIENT_VERSION string
 *
 * @param userAgent - custom userAgent prefix
 *
 */
void
RestClient::Connection::SetUserAgent(const std::string& userAgent) {
  this->customUserAgent = userAgent;
}

/**
 * @brief set custom Certificate Authority (CA) path
 *
 * @param caInfoFilePath - The path to a file holding the certificates used to
 * verify the peer with. See CURLOPT_CAINFO
 *
 */
void
RestClient::Connection::SetCAInfoFilePath(const std::string& caInfoFilePath) {
  this->caInfoFilePath = caInfoFilePath;
}

/**
 * @brief get the user agent to add to the request
 *
 * @return user agent as std::string
 */
std::string
RestClient::Connection::GetUserAgent() {
  std::string prefix;
  if (this->customUserAgent.length() > 0) {
    prefix = this->customUserAgent + " ";
  }
  return std::string(prefix + "restclient-cpp/" + RESTCLIENT_VERSION);
}

/**
 * @brief set timeout for connection
 *
 * @param seconds - timeout in seconds
 *
 */
void
RestClient::Connection::SetTimeout(int seconds) {
  this->timeout = seconds;
}


static int curl_xferinfoCallback(void *connP,
	curl_off_t dltotal, curl_off_t dlnow,
	curl_off_t ultotal, curl_off_t ulnow)
{
	RestClient::Connection * connection = (RestClient::Connection *) connP;
	RestClient::ConnectionProgressCallback pCallback = connection->getProgressCallback();
	void * pUserData = connection->getProgressUserData();

	if (pCallback != 0) {
		return pCallback((curl_off_t)dltotal, (curl_off_t)dlnow, (curl_off_t)ultotal, (curl_off_t)ulnow, pUserData);
	}
	
	return 0;
}


/**
 * @brief set username and password for basic auth
 *
 * @param username
 * @param password
 *
 */
void
RestClient::Connection::SetBasicAuth(const std::string& username,
                                     const std::string& password) {
  this->basicAuth.username = username;
  this->basicAuth.password = password;
}

/**
 * @brief helper function to get called from the actual request methods to
 * prepare the curlHandle for transfer with generic options, perform the
 * request and record some stats from the last request and then reset the
 * handle with curl_easy_reset to its default state. This will keep things
 * like connections and session ID intact but makes sure you can change
 * parameters on the object for another request.
 *
 * @param uri URI to query
 * @param ret Reference to the Response struct that should be filled
 *
 * @return 0 on success and 1 on error
 */
RestClient::Response
RestClient::Connection::performCurlRequest(const std::string& uri) {
  // init return type
  RestClient::Response ret = {};
	ret.rawbody.clear();

  std::string url = std::string(this->baseUrl + uri);
  std::string headerString;
  CURLcode res = CURLE_OK;
  curl_slist* headerList = NULL;

  /** set query URL */
  curl_easy_setopt(this->curlHandle, CURLOPT_URL, url.c_str());
  /** set progress function */
  curl_easy_setopt(this->curlHandle, CURLOPT_XFERINFOFUNCTION, curl_xferinfoCallback);
  curl_easy_setopt(this->curlHandle, CURLOPT_XFERINFODATA, this);
  curl_easy_setopt(this->curlHandle, CURLOPT_NOPROGRESS, 0);

  /** set callback function */
  curl_easy_setopt(this->curlHandle, CURLOPT_WRITEFUNCTION,
                   Helpers::write_callback);
  /** set data object to pass to callback function */
  curl_easy_setopt(this->curlHandle, CURLOPT_WRITEDATA, &ret);
  /** set the header callback function */
  curl_easy_setopt(this->curlHandle, CURLOPT_HEADERFUNCTION,
                   Helpers::header_callback);
  /** callback object for headers */
  curl_easy_setopt(this->curlHandle, CURLOPT_HEADERDATA, &ret);
  /** set http headers */
  for (HeaderFields::const_iterator it = this->headerFields.begin();
      it != this->headerFields.end(); ++it) {
    headerString = it->first;
    headerString += ": ";
    headerString += it->second;
    headerList = curl_slist_append(headerList, headerString.c_str());
  }
  curl_easy_setopt(this->curlHandle, CURLOPT_HTTPHEADER,
      headerList);

  // set basic auth if configured
  if (this->basicAuth.username.length() > 0) {
    std::string authString = std::string(this->basicAuth.username + ":" +
                                         this->basicAuth.password);
    curl_easy_setopt(this->curlHandle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(this->curlHandle, CURLOPT_USERPWD, authString.c_str());
  }
  /** set user agent */
  curl_easy_setopt(this->curlHandle, CURLOPT_USERAGENT,
                   this->GetUserAgent().c_str());

  // set timeout
  if (this->timeout) {
    curl_easy_setopt(this->curlHandle, CURLOPT_TIMEOUT, this->timeout);
    // dont want to get a sig alarm on timeout
    curl_easy_setopt(this->curlHandle, CURLOPT_NOSIGNAL, 1);
  }
  // set follow redirect
  if (this->followRedirects == true) {
    curl_easy_setopt(this->curlHandle, CURLOPT_FOLLOWLOCATION, 1L);
  }
  // if provided, supply CA path
  if (!this->caInfoFilePath.empty()) {
    curl_easy_setopt(this->curlHandle, CURLOPT_CAINFO,
                     this->caInfoFilePath.c_str());
  }
  res = curl_easy_perform(this->curlHandle);
  if (res != CURLE_OK) {
    if (res == CURLE_OPERATION_TIMEDOUT) {
      ret.code = res;
      ret.body = "Operation Timeout.";
    } else {
      ret.body = "Failed to query.";
      ret.code = -1;
    }
  } else {
    int64_t http_code = 0;
    curl_easy_getinfo(this->curlHandle, CURLINFO_RESPONSE_CODE, &http_code);
    ret.code = static_cast<int>(http_code);
  }

  curl_easy_getinfo(this->curlHandle, CURLINFO_TOTAL_TIME,
                    &this->lastRequest.totalTime);
  curl_easy_getinfo(this->curlHandle, CURLINFO_NAMELOOKUP_TIME,
                    &this->lastRequest.nameLookupTime);
  curl_easy_getinfo(this->curlHandle, CURLINFO_CONNECT_TIME,
                    &this->lastRequest.connectTime);
  curl_easy_getinfo(this->curlHandle, CURLINFO_APPCONNECT_TIME,
                    &this->lastRequest.appConnectTime);
  curl_easy_getinfo(this->curlHandle, CURLINFO_PRETRANSFER_TIME,
                    &this->lastRequest.preTransferTime);
  curl_easy_getinfo(this->curlHandle, CURLINFO_STARTTRANSFER_TIME,
                    &this->lastRequest.startTransferTime);
  curl_easy_getinfo(this->curlHandle, CURLINFO_REDIRECT_TIME,
                    &this->lastRequest.redirectTime);
  curl_easy_getinfo(this->curlHandle, CURLINFO_REDIRECT_COUNT,
                    &this->lastRequest.redirectCount);
  // free header list
  curl_slist_free_all(headerList);
  // reset curl handle
  curl_easy_reset(this->curlHandle);
  return ret;
}

/**
 * @brief HTTP GET method
 *
 * @param url to query
 *
 * @return response struct
 */
RestClient::Response
RestClient::Connection::get(const std::string& url) {
  return this->performCurlRequest(url);
}
/**
 * @brief HTTP POST method
 *
 * @param url to query
 * @param data HTTP POST body
 *
 * @return response struct
 */
RestClient::Response
RestClient::Connection::post(const std::string& url,
                             const std::string& data) {
  /** Now specify we want to POST data */
  curl_easy_setopt(this->curlHandle, CURLOPT_POST, 1L);
  /** set post fields */
  curl_easy_setopt(this->curlHandle, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_setopt(this->curlHandle, CURLOPT_POSTFIELDSIZE, data.size());

  return this->performCurlRequest(url);
}

/**
* @brief HTTP POST method
*
* @param url to query
* @param data HTTP POST body
*
* @return response struct
*/
RestClient::Response
RestClient::Connection::postbinary(const std::string& url,
	const unsigned char * pData, const unsigned int size) {
	/** Now specify we want to POST data */
	curl_easy_setopt(this->curlHandle, CURLOPT_POST, 1L);
	/** set post fields */
	curl_easy_setopt(this->curlHandle, CURLOPT_POSTFIELDS, pData);
	curl_easy_setopt(this->curlHandle, CURLOPT_POSTFIELDSIZE, size);

	return this->performCurlRequest(url);
}

/**
 * @brief HTTP PUT method
 *
 * @param url to query
 * @param data HTTP PUT body
 *
 * @return response struct
 */
RestClient::Response
RestClient::Connection::put(const std::string& url,
                            const std::string& data) {
  /** initialize upload object */
  RestClient::Helpers::UploadObject up_obj;
  up_obj.data = data.c_str();
  up_obj.length = data.size();

  /** Now specify we want to PUT data */
  curl_easy_setopt(this->curlHandle, CURLOPT_PUT, 1L);
  curl_easy_setopt(this->curlHandle, CURLOPT_UPLOAD, 1L);
  /** set read callback function */
  curl_easy_setopt(this->curlHandle, CURLOPT_READFUNCTION,
                   RestClient::Helpers::read_callback);
  /** set data object to pass to callback function */
  curl_easy_setopt(this->curlHandle, CURLOPT_READDATA, &up_obj);
  /** set data size */
  curl_easy_setopt(this->curlHandle, CURLOPT_INFILESIZE,
                     static_cast<int64_t>(up_obj.length));

  return this->performCurlRequest(url);
}

/**
* @brief HTTP PUT method
*
* @param url to query
* @param data HTTP PUT body
*
* @return response struct
*/
RestClient::Response
RestClient::Connection::putbinary(const std::string& url,
	const unsigned char * pData, const unsigned int size) {
	/** initialize upload object */
	RestClient::Helpers::UploadObject up_obj;
	up_obj.data = (const char*) pData;
	up_obj.length = size;

	/** Now specify we want to PUT data */
	curl_easy_setopt(this->curlHandle, CURLOPT_PUT, 1L);
	curl_easy_setopt(this->curlHandle, CURLOPT_UPLOAD, 1L);
	/** set read callback function */
	curl_easy_setopt(this->curlHandle, CURLOPT_READFUNCTION,
		RestClient::Helpers::read_callback);
	/** set data object to pass to callback function */
	curl_easy_setopt(this->curlHandle, CURLOPT_READDATA, &up_obj);
	/** set data size */
	curl_easy_setopt(this->curlHandle, CURLOPT_INFILESIZE,
		static_cast<int64_t>(up_obj.length));

	return this->performCurlRequest(url);
}

/**
 * @brief HTTP DELETE method
 *
 * @param url to query
 *
 * @return response struct
 */
RestClient::Response
RestClient::Connection::del(const std::string& url) {
  /** we want HTTP DELETE */
  const char* http_delete = "DELETE";

  /** set HTTP DELETE METHOD */
  curl_easy_setopt(this->curlHandle, CURLOPT_CUSTOMREQUEST, http_delete);

  return this->performCurlRequest(url);
}

RestClient::Response RestClient::Connection::patch(const std::string& uri, const std::string &data) {
	char *http_patch = "PATCH";
	curl_easy_setopt(this->curlHandle, CURLOPT_CUSTOMREQUEST, http_patch);
	curl_easy_setopt(this->curlHandle, CURLOPT_POSTFIELDS, data.c_str());
	curl_easy_setopt(this->curlHandle, CURLOPT_POSTFIELDSIZE, data.size());
	return this->performCurlRequest(uri);
}


void RestClient::Connection::setVerifyPeer(bool bVerifyPeer)
{
	curl_easy_setopt(this->curlHandle, CURLOPT_SSL_VERIFYPEER, (long)bVerifyPeer);
}


