
package main

import (
	"log"
	"os"
	"fmt"
	"path/filepath"
	"encoding/xml"
)


type DistXMLEntry struct {
	XMLName xml.Name `xml:"entry"`
	Name string `xml:"name,attr"`
	FileName string `xml:"filename,attr"`
	Size uint32 `xml:"size,attr"`
	ContentType string `xml:"contenttype,attr"`
}


type DistXMLRoot struct {
	XMLName xml.Name `xml:"package"`
	XMLNs string `xml:"xmlns,attr"`
	Entries []DistXMLEntry `xml:"entry"`
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
//////////////////////////////////////////////////////////////////////////////////////////////////////


func createMCServerTemplate (outputDir string, packageName string, clientName string, libraryName string, configName string, gitHash string, dllExtension string, coreResourcesName string) (error) {

	pkgfile, err := os.Create(outputDir + gitHash + "_package.xml");
	if (err != nil) {
		return err
	}

	defer pkgfile.Close()	

	fmt.Fprintf(pkgfile, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
	fmt.Fprintf(pkgfile, "<amcpackage xmlns=\"http://schemas.autodesk.com/amcpackage/2020/06\">\n");
	fmt.Fprintf(pkgfile, "  <build name=\"%s\" configuration=\"%s\" coreclient=\"%s\">\n", packageName, configName, clientName);
	
	fmt.Fprintf(pkgfile, "    <library name=\"core\" import=\"%s\" resources=\"%s\" />\n", libraryName, coreResourcesName);
	fmt.Fprintf(pkgfile, "    <library name=\"datamodel\" import=\"%s_core_libmcdata.%s\" />\n", gitHash, dllExtension);
	fmt.Fprintf(pkgfile, "    <library name=\"lib3mf\" import=\"%s_core_lib3mf.%s\" />\n", gitHash, dllExtension);
	fmt.Fprintf(pkgfile, "    <library name=\"plugin_main\" import=\"%s_plugin_main.%s\" />\n", gitHash, dllExtension);
	fmt.Fprintf(pkgfile, "    <library name=\"plugin_demo\" import=\"%s_plugin_demo.%s\" />\n", gitHash, dllExtension);
	fmt.Fprintf(pkgfile, "    <library name=\"plugin_laser\" import=\"%s_plugin_laser.%s\" />\n", gitHash, dllExtension);
	fmt.Fprintf(pkgfile, "    <library name=\"plugin_mechanics\" import=\"%s_plugin_mechanics.%s\" />\n", gitHash, dllExtension);
	fmt.Fprintf(pkgfile, "    <library name=\"plugin_plc\" import=\"%s_plugin_plc.%s\" />\n", gitHash, dllExtension);
	fmt.Fprintf(pkgfile, "    <library name=\"plugin_buildprocess\" import=\"%s_plugin_buildprocess.%s\" />\n", gitHash, dllExtension);
	fmt.Fprintf(pkgfile, "    <library name=\"plugin_pidcontrol\" import=\"%s_plugin_pidcontrol.%s\" />\n", gitHash, dllExtension);
	fmt.Fprintf(pkgfile, "    <library name=\"plugin_printerconnection\" import=\"%s_plugin_printerconnection.%s\" />\n", gitHash, dllExtension);
	fmt.Fprintf(pkgfile, "    <library name=\"plugin_userinterface\" import=\"%s_plugin_userinterface.%s\" />\n", gitHash, dllExtension);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_marlin\" import=\"%s_driver_marlin.%s\" resources=\"%s_driver_marlin.data\"  />\n", gitHash, dllExtension, gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_scanlab\" import=\"%s_driver_scanlab.%s\" resources=\"%s_driver_scanlab.data\" />\n", gitHash, dllExtension, gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_camera\" import=\"%s_driver_camera.%s\" resources=\"%s_driver_camera.data\" />\n", gitHash, dllExtension, gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_scanlaboie\" import=\"%s_driver_scanlaboie.%s\" resources=\"%s_driver_scanlaboie.data\" />\n", gitHash, dllExtension, gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_s7net\" import=\"%s_driver_s7net.%s\" resources=\"%s_driver_s7net.data\"  />\n", gitHash, dllExtension, gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_mqtt\" import=\"%s_driver_mqtt.%s\" resources=\"%s_driver_mqtt.data\"  />\n", gitHash, dllExtension, gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_raylase\" import=\"%s_driver_raylase.%s\" resources=\"%s_driver_raylase.data\"  />\n", gitHash, dllExtension, gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_ximc\" import=\"%s_driver_ximc.%s\" resources=\"%s_driver_ximc.data\"  />\n", gitHash, dllExtension, gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_bur\" import=\"%s_driver_bur.%s\" resources=\"%s_driver_bur.data\"  />\n", gitHash, dllExtension, gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_ads\" import=\"%s_driver_ads.%s\" resources=\"%s_driver_ads.data\"  />\n", gitHash, dllExtension, gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_opcua\" import=\"%s_driver_opcua.%s\" resources=\"%s_driver_opcua.data\"  />\n", gitHash, dllExtension, gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_tcpip\" import=\"%s_driver_tcpip.%s\" resources=\"%s_driver_tcpip.data\"  />\n", gitHash, dllExtension, gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_uart\" import=\"%s_driver_uart.%s\" resources=\"%s_driver_uart.data\"  />\n", gitHash, dllExtension, gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_rasterizer\" import=\"%s_driver_rasterizer.%s\" resources=\"%s_driver_rasterizer.data\"  />\n", gitHash, dllExtension, gitHash);
	fmt.Fprintf(pkgfile, "  </build>\n");
	fmt.Fprintf(pkgfile, "</amcpackage>\n");
	
	pkgfile.Close()	
	

	file, err := os.Create(outputDir + "amc_server.xml");
	if (err != nil) {
		return err
	}
	
	defer file.Close()	
	
	fmt.Fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
	fmt.Fprintf(file, "<amc xmlns=\"http://schemas.autodesk.com/amc/2020/06\">\n");
	fmt.Fprintf(file, "  <server hostname=\"0.0.0.0\" port=\"8869\" />\n");
	fmt.Fprintf(file, "  <data directory=\"data/\" database=\"sqlite\" sqlitedb=\"storage.db\" />\n");
	fmt.Fprintf(file, "  <defaultpackage name=\"%s_package.xml\" githash=\"%s\" sha256=\"%s\" />\n", gitHash, gitHash, "");	
	fmt.Fprintf(file, "</amc>\n");


	return nil;

}

func main() {

	var Root DistXMLRoot;
	Root.XMLNs = "http://schemas.autodesk.com/amc/resourcepackage/2020/07";
	
	var dllExtension string
	var err error
		
	argsWithProg := os.Args;
	if (len (argsWithProg) < 4) {
		log.Fatal ("Please start with createDist <outputpath> <githash> <systemprefix>");
	}
	
	OutputDir := filepath.Clean (argsWithProg[1]) + "/";
	hexSum := argsWithProg[2];
	systemprefix := argsWithProg[3];
			
	if (systemprefix == "win64") {
	
		dllExtension = "dll";
		
	} else if (systemprefix == "linux64") {

		dllExtension = "so";
		
	} else if (systemprefix == "rpi") {
	
		dllExtension = "so";
		
	} else {
		log.Fatal("Invalid system prefix: " + systemprefix);
	}
		
		
	ClientZIPName := hexSum + "_core.client";
	CoreResourcesName := hexSum + "_core.data";
	LibraryName := hexSum + "_core_libmc." + dllExtension;
	ConfigName := hexSum + "_config.xml";
		
	packageName := "Build " + hexSum;	

	fmt.Printf("creating server config in %s\n", OutputDir);
	
	err = createMCServerTemplate (OutputDir, packageName, ClientZIPName, LibraryName, ConfigName, hexSum, dllExtension, CoreResourcesName);
	if err != nil {
		log.Fatal(err)
	}
	
	
	

}