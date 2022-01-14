/**
 * @file opcuaclient.c
 * @author Sven Scheres (sven.scheres@lm-innovations.com)
 * @brief Simple OPC/UA client to evaluate Open62541
 * @version 0.1
 * @date 2021-12-01
 *
 * @copyright Copyright (c) 2021
 *
 */
 
#include <stdio.h>
#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/plugin/securitypolicy.h>
#include <open62541/plugin/securitypolicy_default.h>
 
#include <open62541/plugin/pki_default.h>
#include <open62541/server_config_default.h>
 
#include <unistd.h>
#include <limits.h>
#include <filesystem>
#include <string>
#include <variant>
#include <map>
 
class OpcUA {
 public:
  UA_ByteString client_certificate_;
  UA_ByteString client_privatekey_;
 
  std::string server_url = "opc.tcp://localhost:48050";  // default URL of the OPC/UA server to connect to
  int ns_index = 7;                                      // default OPC/UA namespace index
  bool secure_connect = false;                           // set to true to use a encrypted connection.
 
  UA_Client* client;
 
  OpcUA();
 
  UA_StatusCode connect();
  bool write_variant_to_server(
      std::string node_id,
      std::variant<int, float, bool, std::string, uint16_t> value);  // can write various data types
  bool writeBooleanToServer(std::string node_id, bool value);
  float readFloatFromServer(std::string node_id);
  double readDoubleFromServer(std::string node_id);
  int readIntFromServer(std::string node_id);
  uint32_t readUintFromServer(std::string node_id);
  bool readBoolFromServer(std::string node_id);
  bool callVariantMethod(std::string object_id, std::string method_id,
                         std::variant<int, float, bool, std::string, std::monostate> value);
  UA_INLINE UA_ByteString loadFile(const char* const path);
  std::string translateStatuscode(UA_StatusCode status);
 
  ~OpcUA();
};
