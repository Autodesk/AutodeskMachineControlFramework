/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


package main

import (
	"fmt"
	"io/ioutil"
	"log"	
	"os"
	"errors"
	"net/http"
	"path/filepath"
	"encoding/xml"
	"./LibMC"	
	"./LibMCData"	
	
	// Handler for gzip handling of the REST end point.
	"github.com/gorilla/handlers" 
)


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Handles
//////////////////////////////////////////////////////////////////////////////////////////////////////
var GlobalContext libmc.MCContext;

const XMLNS_SERVERCONFIG = "http://schemas.autodesk.com/amc/2020/06"
const XMLNS_PACKAGECONFIG = "http://schemas.autodesk.com/amcpackage/2020/06"

type ServerPackageXMLLibrary struct {
	XMLName xml.Name `xml:"library"`	
	Name string `xml:"name,attr"`
	Import string `xml:"import,attr"`
	Resources string `xml:"resources,attr"`
}


type ServerPackageXMLBuild struct {
	XMLName xml.Name `xml:"build"`	
	Configuration string `xml:"configuration,attr"`
	Name string `xml:"name,attr"`
	CoreClient string `xml:"coreclient,attr"`
	Libraries []ServerPackageXMLLibrary `xml:"library"`
}


type ServerPackageXMLRoot struct {
	XMLName xml.Name `xml:"amcpackage"`	
	NameSpace string `xml:"xmlns,attr"`
	Build ServerPackageXMLBuild `xml:"build"`
}



type ServerConfigXMLServer struct {
	XMLName xml.Name `xml:"server"`	
	HostName string `xml:"hostname,attr"`
	Port uint32 `xml:"port,attr"`
}


type ServerConfigXMLData struct {
	XMLName xml.Name `xml:"data"`	
	Directory string `xml:"directory,attr"`
	DataBaseType string `xml:"database,attr"`
	SqLiteDB string `xml:"sqlitedb,attr"`
}

type ServerConfigXMLDefaultPackage struct {
	XMLName xml.Name `xml:"defaultpackage"`	
	Name string `xml:"name,attr"`
	GitHash string `xml:"githash,attr"`
	XMLSHA2 string `xml:"xmlsha2,attr"`
}


type ServerConfigXMLRoot struct {
	XMLName xml.Name `xml:"amc"`	
	NameSpace string `xml:"xmlns,attr"`
	Server ServerConfigXMLServer `xml:"server"`
	Data ServerConfigXMLData `xml:"data"`
	DefaultPackage ServerConfigXMLDefaultPackage `xml:"defaultpackage"`
}

type ServerConfigPackageLibraryInfo struct {
	Path string
	ResourcePath string
}


type ServerConfig struct {
	HostName string
	Port uint32
	DataDirectory string
	DataBaseType libmcdata.DataBaseType
	ConnectionString string
	PackageName string
	PackageCoreClient string
	PackageConfig string
	PackageLibraries  map[string]ServerConfigPackageLibraryInfo
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility function
//////////////////////////////////////////////////////////////////////////////////////////////////////


func makeHandler(fn func(http.ResponseWriter, *http.Request)) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		fn(w, r);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Server package configuration
//////////////////////////////////////////////////////////////////////////////////////////////////////
func loadServerPackageXML (FileName string, SHA2 string) (ServerPackageXMLBuild, error) {

	var root ServerPackageXMLRoot;
	var build ServerPackageXMLBuild;
	
	file, err := os.Open(FileName);
	if (err != nil) {
		return build, err
	}
	
	defer file.Close();

	bytes, err := ioutil.ReadAll (file);
	if (err != nil) {
		return build, err
	}
	
	err = xml.Unmarshal(bytes, &root)
	if (err != nil) {
		return build, err
	}	
	
	if (root.NameSpace != XMLNS_PACKAGECONFIG) {
		err = errors.New ("Invalid server package xml!");
		return build, err;
	}
	
	build = root.Build;
	
	return build, nil;

}



//////////////////////////////////////////////////////////////////////////////////////////////////////
// Server configuration
//////////////////////////////////////////////////////////////////////////////////////////////////////
func LoadServerConfigXML (FileName string) (ServerConfig, error) {

	var root ServerConfigXMLRoot;
	var config ServerConfig;
	
	file, err := os.Open(FileName);
	if (err != nil) {
		return config, err
	}
	
	defer file.Close();

	bytes, err := ioutil.ReadAll (file);
	if (err != nil) {
		return config, err
	}
	
	err = xml.Unmarshal(bytes, &root)
	if (err != nil) {
		return config, err
	}	
	
	if (root.NameSpace != XMLNS_SERVERCONFIG) {
		err = errors.New ("Invalid server config xml!");
		return config, err;
	}

	if (root.Server.HostName == "") {
		err = errors.New ("Invalid server host name");
		return config, err;
	}
	
	if ((root.Server.Port < 0x0400) || (root.Server.Port > 0xBFFF)) {
		err = errors.New ("Invalid server port");
		return config, err;
	}
	

	if (root.Data.Directory == "") {
		err = errors.New ("Invalid data directory!");
		return config, err;
	}

	
	config.HostName = root.Server.HostName;
	config.Port = root.Server.Port;
	
	config.DataDirectory, err = filepath.Abs (root.Data.Directory);
	if (err != nil) {
		return config, err;
	}
	
	_, err = os.Stat (config.DataDirectory);
	if (err != nil) {
		err = errors.New ("Data directory does not exist: " + config.DataDirectory);
		return config, err
	}	

	if (root.Data.DataBaseType == "sqlite") {
		if (root.Data.SqLiteDB == "") {
			err = errors.New ("Invalid sqlite db file.");
			return config, err;
		}
		
		config.DataBaseType = libmcdata.DataBaseType_SqLite;
		config.ConnectionString = filepath.Join (config.DataDirectory, root.Data.SqLiteDB);
	} else {
		err = errors.New ("Invalid data base type: " + root.Data.DataBaseType);
		return config, err;
	}


	if (len (root.DefaultPackage.Name) == 0) {
		err = errors.New ("Invalid default package information");
		return config, err;
	}
	
	packageToUse, err := loadServerPackageXML (root.DefaultPackage.Name, root.DefaultPackage.XMLSHA2);
	if (err != nil) {
		return config, err
	}	

	if (packageToUse.Name == "") {
		err = errors.New ("Invalid package name");
		return config, err;
	}

	if (packageToUse.Configuration == "") {
		err = errors.New ("Invalid package config");
		return config, err;
	}

	if (packageToUse.CoreClient == "") {
		err = errors.New ("Invalid core client");
		return config, err;
	}

	config.PackageName = packageToUse.Name;
	if (err != nil) {
		return config, err
	}

	config.PackageConfig, err = filepath.Abs (packageToUse.Configuration);
	if (err != nil) {
		return config, err
	}

	_, err = os.Stat (config.PackageConfig);
	if (err != nil) {
		err = errors.New ("Package config does not exist: " + config.PackageConfig);
		return config, err
	}

	config.PackageCoreClient, err = filepath.Abs (packageToUse.CoreClient);
	if (err != nil) {
		return config, err
	}

	_, err = os.Stat (config.PackageCoreClient);
	if (err != nil) {
		err = errors.New ("Core client does not exist: " + config.PackageCoreClient);
		return config, err
	}
	
	
	config.PackageLibraries = make(map[string]ServerConfigPackageLibraryInfo);
	for _, library := range packageToUse.Libraries {
		if (len (library.Name) == 0) {
			err = errors.New ("Empty library name!");
			return config, err
		}
		
		if (len (library.Import) == 0) {
			err = errors.New ("Empty library import path!");
			return config, err
		}
		
		importPath, err := filepath.Abs (library.Import);
		if (err != nil) {
			return config, err
		}	
				
		importResourcePath := "";
		if (len (library.Resources) != 0) {
			importResourcePath, err = filepath.Abs (library.Resources);
			if (err != nil) {
				return config, err
			}	
			
		
		}
		
		
		/* for development, we are in lazy mode and do not care if the dlls exist.
				
		_, err = os.Stat (importPath);
		if (err != nil) {
			err = errors.New ("library \"" + library.Name + "\" does not exist: " + importPath);
			return config, err
		}		 */
		
		var libraryInfo ServerConfigPackageLibraryInfo;
		libraryInfo.Path = importPath;
		libraryInfo.ResourcePath = importResourcePath;
		
		config.PackageLibraries [library.Name] = libraryInfo;
		
	}
	
	
	if (len (config.PackageLibraries ["core"].Path) == 0) {
		err = errors.New ("Package core library not given!");
		return config, err
	}	

	if (len (config.PackageLibraries ["datamodel"].Path) == 0) {
		err = errors.New ("Package datamodel library not given!");
		return config, err
	}	

	return config, nil 
	
}




//////////////////////////////////////////////////////////////////////////////////////////////////////
// REST handler
//////////////////////////////////////////////////////////////////////////////////////////////////////

func RESTHandler (w http.ResponseWriter, r *http.Request) {

	var err error = nil;
	var dataBytes []byte;
	bodyBytes := make ([]byte, 1);
	
	w.Header().Set("Access-Control-Allow-Headers", "*");	
	w.Header().Set("Access-Control-Allow-Origin", "*");	
	w.Header().Set("Cache-Control", "no-cache");
	
	if (r.Method == "OPTIONS") { // CORS handler	
		return;
	}
	
	authHeader := r.Header.Get("Authorization");	
				
	requestHandler, err := GlobalContext.CreateAPIRequestHandler (r.URL.Path, r.Method, authHeader);
	if (err == nil) {
	
		var expectsRawBody bool = false;
		var expectsFormData bool = false;
		var fieldCount uint32 = 0;
	
		expectsRawBody, err = requestHandler.ExpectsRawBody ();	
		if ((expectsRawBody) && (err == nil)) {				
			bodyBytes, err = ioutil.ReadAll (r.Body);		
		}
		
		if (err == nil) {
			fieldCount, expectsFormData, err = requestHandler.ExpectsFormData ();
		}
				
		if (expectsFormData) {		
			err = r.ParseMultipartForm (32 * 1024 * 1024);
			
			if (err == nil) {
				var fieldIndex uint32;
				for fieldIndex = 0; fieldIndex < fieldCount; fieldIndex++ {
				
					fieldName, isFile, isMandatory, err := requestHandler.GetFormDataDetails (fieldIndex);
					if (err == nil) {
				
						if (isFile) {
		
							formFile, _, err := r.FormFile(fieldName);
						
							if (err == nil) {

							
								defer formFile.Close ();
					
								byteArray, err := ioutil.ReadAll(formFile);
								if (err == nil) {									
									err = requestHandler.SetFormDataField (fieldName, byteArray);							
								}													
								
							} else {
							
								if (!isMandatory) {
									err = nil;
								}
							
							}
						
						} else {
						
						
								formValue := r.FormValue (fieldName);
								if (formValue != "") {																
									err = requestHandler.SetFormStringField (fieldName, formValue);							
								}
								
						
						
						}
					
					}
										
					if (err != nil) {
						break;
					}
														
				}
				
			}
			
		}
		
		if (err == nil) {
		
			contentType, httpCode, err := requestHandler.Handle (bodyBytes);
				
			if (err == nil) {
				dataBytes, err = requestHandler.GetResultData (dataBytes);
			} else {
				dataBytes = make ([]byte, 1);
			}


			if (err == nil) {

				if (httpCode == 200) {		
					w.Header().Set("Content-Type", contentType);	
					w.Write (dataBytes);
				} else {		
					http.Error (w, string (dataBytes), int (httpCode));
				}
				
			}
		}
	
	}


	
	if (err != nil) {
		GlobalContext.Log (fmt.Sprintf ("Fatal error on %s: %s", r.URL.Path, err.Error()), libmc.LogSubSystem_Network, libmc.LogLevel_Message);
		http.Error(w, "Internal Server Error: " + err.Error (), http.StatusInternalServerError);
	}
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// App server
//////////////////////////////////////////////////////////////////////////////////////////////////////
func startAppServer (host string, port uint32, context libmc.MCContext) (error) {	
	
	GlobalContext = context;

	context.Log (fmt.Sprintf ("Listening on %s:%d", host, port), libmc.LogSubSystem_Network, libmc.LogLevel_Message);			   
	
	mux := http.NewServeMux();	
	mux.HandleFunc("/", RESTHandler);
	return http.ListenAndServe(fmt.Sprintf("%s:%d", host, port), handlers.CompressHandler(mux));	
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
//////////////////////////////////////////////////////////////////////////////////////////////////////

func main() {

	var err error;
	
	serverConfigFileName, err := filepath.Abs ("amc_server.xml");
	if (err != nil) {
		log.Fatal(err)
	}
	
	fmt.Println("Loading configuration: " + serverConfigFileName);
	serverConfig, err := LoadServerConfigXML (serverConfigFileName);
	if (err != nil) {
		log.Fatal(err)
	}

	//
	// Creating data model
	//
	fmt.Println("Executing " + serverConfig.PackageLibraries["datamodel"].Path);
	datawrapper, err := libmcdata.LoadLibrary (serverConfig.PackageLibraries["datamodel"].Path);
	if (err != nil) {
		log.Fatal(err)
	}
	
	nMajor, nMinor, nMicro, err := datawrapper.GetVersion()
	if (err != nil) {
		log.Fatal(err)
	}
	fmt.Println(fmt.Sprintf("Found libmcdata.version %d.%d.%d", nMajor, nMinor, nMicro))

	datamodel, err := datawrapper.CreateDataModelInstance ();
	if (err != nil) {
		log.Fatal(err)
	}
		
	datasymbollookup, err := datawrapper.GetSymbolLookupMethod ();
	if (err != nil) {
		log.Fatal(err)
	}

	err = datamodel.InitialiseDatabase (serverConfig.DataDirectory, serverConfig.DataBaseType, serverConfig.ConnectionString);
    if err != nil {
        log.Fatal(err)
    }	


	//
	// Creating Context
	//
	fmt.Println("Executing " + serverConfig.PackageLibraries["core"].Path);
	wrapper, err := libmc.LoadLibrary(serverConfig.PackageLibraries["core"].Path);
	if (err != nil) {
		log.Fatal(err)
	}
	
	nMajor, nMinor, nMicro, err = wrapper.GetVersion()
	if (err != nil) {
		log.Fatal(err)
	}
	fmt.Println(fmt.Sprintf("Found libmc.version %d.%d.%d", nMajor, nMinor, nMicro))
		
	err = wrapper.InjectComponent ("LibMCData", datasymbollookup);
	if (err != nil) {
		log.Fatal(err)
	}
		
		
	fmt.Println("Creating Context..");
	context, err := wrapper.CreateMCContext (datamodel);
	if (err != nil) {
		log.Fatal(err)
	}
		
	for libraryName, libraryInfo := range serverConfig.PackageLibraries {
		context.RegisterLibraryPath (libraryName, libraryInfo.Path, libraryInfo.ResourcePath);
	}	
	
				
	context.Log ("Loading " + serverConfig.PackageName + " (" + serverConfig.PackageConfig + ")", libmc.LogSubSystem_System, libmc.LogLevel_Message);
    content, err := ioutil.ReadFile(serverConfig.PackageConfig)
    if err != nil {
        log.Fatal(err)
    }	

	context.Log ("Parsing configuration", libmc.LogSubSystem_System, libmc.LogLevel_Message);
	err = context.ParseConfiguration (string (content));
    if err != nil {
        log.Fatal(err)
    }	

	context.Log ("Loading " + serverConfig.PackageCoreClient + "..", libmc.LogSubSystem_System, libmc.LogLevel_Message);
			
	err = context.LoadClientPackage (serverConfig.PackageCoreClient);
    if err != nil {
        log.Fatal(err)
    }	

		
	err = context.StartAllThreads ();
    if err != nil {
        log.Fatal(err)
    }	
	
	
	err = startAppServer (serverConfig.HostName, serverConfig.Port, context);
    if err != nil {
        log.Fatal(err)
    }	
	

	fmt.Println("terminating thread!");
	err = context.TerminateAllThreads ();
    if err != nil {
        log.Fatal(err)
    }	
	

	fmt.Println("done!");
		
}

