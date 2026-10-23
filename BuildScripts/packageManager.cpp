// packageManager.cpp
// C++17 translation using cpp-httplib + PicoSHA2 (header-only) for SHA-256.
// Also includes rapidjson and pugixml headers to respect the "only dependencies" constraint.

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>

#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "common_utils.hpp" 
#include "common_exportstream_native.hpp"
#include "common_importstream_native.hpp"

#include "Libraries/cpp-httplib/httplib.h"
#include "Libraries/PugiXML/pugixml.hpp"             
#include "Libraries/RapidJSON/document.h"  
#include "Libraries/PicoSHA2/picosha2.h"               // SHA-256 (header-only)


struct ParsedURL {
  std::string scheme; // "http" or "https"
  std::string host;
  int         port = -1;
  std::string path;   // starts with '/'
};

// Very small URL parser for forms like: http(s)://host[:port][/path...]
static bool parse_url(const std::string& url, ParsedURL& out) {
  auto pos_scheme = url.find("://");
  if (pos_scheme == std::string::npos) return false;
  out.scheme = url.substr(0, pos_scheme);
  auto rest = url.substr(pos_scheme + 3);

  auto pos_slash = rest.find('/');
  std::string hostport = (pos_slash == std::string::npos) ? rest : rest.substr(0, pos_slash);
  out.path = (pos_slash == std::string::npos) ? "/" : rest.substr(pos_slash);

  auto pos_colon = hostport.find(':');
  if (pos_colon == std::string::npos) {
    out.host = hostport;
    out.port = -1;
  } else {
    out.host = hostport.substr(0, pos_colon);
    out.port = std::stoi(hostport.substr(pos_colon + 1));
  }

  if (out.scheme != "http" && out.scheme != "https") 
      return false;
  if (out.path.empty()) out.path = "/";
  return !out.host.empty();
}

static std::string join_url_path(const std::string& base_path, const std::string& segment) {
  if (base_path.empty() || base_path.back() != '/') {
    if (!segment.empty() && segment.front() == '/') return base_path + segment;
    return base_path + "/" + segment;
  } else {
    if (!segment.empty() && segment.front() == '/') return base_path + segment.substr(1);
    return base_path + segment;
  }
}

template <typename ClientT> void download_with_client(ClientT& client, const std::string& path, const std::string& out_path) {

    client.set_follow_location (true);

	AMCCommon::CExportStream_Native ofs_stream (out_path);

    size_t nReceivedSize = 0;

    auto res = client.Get(path.c_str(), [&ofs_stream, &nReceivedSize](const char* data, size_t len) {

		size_t nOldReceivedMB = nReceivedSize / (1024 * 1024);

        ofs_stream.writeBuffer(data, len);

		nReceivedSize += len;

        size_t nNewReceivedMB = nReceivedSize / (1024 * 1024);

        if (nNewReceivedMB != nOldReceivedMB) {
			std::cout << "Received " << nNewReceivedMB << " MB...\n";
        }


        return true;
        });

    if (!res) {
        throw std::runtime_error ("HTTP request failed (network/connection error).");
    }

    if (res->status != 200) {
        throw std::runtime_error("HTTP error status: " + std::to_string(res->status));
    }

}


// Download to file using cpp-httplib.
void download_to_file(const std::string& full_url, const std::string& out_path) 
{
  ParsedURL u;
  if (!parse_url(full_url, u)) 
  { 
    throw std::runtime_error("Invalid URL: " + full_url);
  }

  std::vector<uint8_t> receivedBuffer;
  
  if (u.scheme == "http") {
      httplib::Client client(u.host.c_str(), (u.port > 0 ? u.port : 80));

	  download_with_client(client, u.path, out_path);
      
  } else {

      httplib::SSLClient sslclient(u.host.c_str(), (u.port > 0 ? u.port : 443));
                          
      sslclient.enable_server_certificate_verification(true);
      download_with_client(sslclient, u.path, out_path);

  }

  
}


// ---------------------- Main ----------------------
int main(int argc, char* argv[]) {

    try {

        if (argc < 2) {
            std::cerr << "Please start with: packageManager <command> <parameter1> <parameter2> ....\n";            
            std::cerr << "for example: packageManager download <localfilename> <repositoryurl> <githash> <sha256checksum>\n";
            return 1;
        }

        std::string command = argv[1];

        if (command == "download") {
            if (argc < 6) {
                std::cerr << "Please start with: packageManager download <localfilename> <repositoryurl> <githash> <sha256checksum>\n";
                return 1;
            }

            const std::string localFileName = argv[2];
            const std::string downloadURLBase = argv[3];
            const std::string gitHash = argv[4];
            const std::string shaCheckSum = argv[5];

            std::cout << "---------------------------------------------------------------------------------\n";
            std::cout << "-- AMCF Package Manager                                                        --\n";
            std::cout << "---------------------------------------------------------------------------------\n";
            std::cout << "Repository URL: " << downloadURLBase << "\n";
            std::cout << "Local Filename: " << localFileName << "\n";
            std::cout << "GIT Hash: " << gitHash << "\n";
            std::cout << "SHA 256 Checksum: " << shaCheckSum << "\n";
            std::cout << "---------------------------------------------------------------------------------\n";

            std::cout << "Verifying " << localFileName << "...\n";

            std::string calculatedChecksum;

            if (!AMCCommon::CUtils::fileOrPathExistsOnDisk (localFileName)) {
                std::cout << "Local package does not exist...\n";
            }
            else {
                try {
					std::vector<uint8_t> fileBytes;
					AMCCommon::CImportStream_Native ifs_stream(localFileName);
                    ifs_stream.readIntoMemory(fileBytes);

                    calculatedChecksum = picosha2::hash256_hex_string(fileBytes); 
                    std::cout << "Calculated Checksum: " << calculatedChecksum << "\n";
                    if (calculatedChecksum != shaCheckSum) {
                        std::cout << "Checksum mismatch!\n";
                    }
                }
                catch (const std::exception& ex) {
                    std::cerr << "Error reading file: " << ex.what() << "\n";
                    return 1;
                }
            }

            if (calculatedChecksum != shaCheckSum) {
                std::cout << "Creating " << localFileName << "...\n";

                const std::string fileSegment = "amcf_" + gitHash + ".zip";

                ParsedURL baseParsed;
                if (!parse_url(downloadURLBase, baseParsed)) {
                    std::cerr << "Invalid repository URL: " << downloadURLBase << "\n";
                    return 1;
                }
                std::string finalPath = join_url_path(baseParsed.path, fileSegment);
                std::string finalURL = baseParsed.scheme + "://" + baseParsed.host;
                if (baseParsed.port > 0) finalURL += ":" + std::to_string(baseParsed.port);
                finalURL += finalPath;

                std::cout << "Downloading package from " << finalURL << "\n";
                std::string err;
                download_to_file(finalURL, localFileName);

                try {
                    std::vector<uint8_t> fileBytes;
                    AMCCommon::CImportStream_Native ifs_stream(localFileName);
                    ifs_stream.readIntoMemory(fileBytes);

                    auto finalChecksum = picosha2::hash256_hex_string(fileBytes);
                    std::cout << "Final Checksum: " << finalChecksum << "\n";
                    if (finalChecksum != shaCheckSum) {
                        std::cerr << "Checksum mismatch!\n";
                        return 1;
                    }
                }
                catch (const std::exception& ex) {
                    std::cerr << "Error reading downloaded file: " <<  ex.what() << "\n";
                    return 1;
                }
            }

            std::cout << "done..\n";
            return 0;

        }
        else {
            std::cerr << "Unknown command. Valid commands are download.\n";
            return 1;
        }
  }
  catch (const std::exception& ex) {
    std::cerr << "Fatal error: " << ex.what() << "\n";
    return 1;
  }

}
