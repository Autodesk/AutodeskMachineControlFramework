
package main

import (
	"log"
	"os"
	"fmt"
	"io/ioutil"
	"strings"
	"path/filepath"
	"encoding/xml"
	"archive/zip"
	"mime"
)


type DistXMLEntry struct {
	XMLName xml.Name `xml:"entry"`
	URL string `xml:"url,attr"`
	FileName string `xml:"filename,attr"`
	Size uint32 `xml:"size,attr"`
	ContentType string `xml:"contenttype,attr"`
}


type DistXMLRoot struct {
	XMLName xml.Name `xml:"serve"`
	XMLNs string `xml:"xmlns,attr"`
	Entries []DistXMLEntry `xml:"entry"`
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
//////////////////////////////////////////////////////////////////////////////////////////////////////


func createMCServerTemplate (outputDir string, packageName string, clientName string, libraryName string, configName string, gitHash string) (error) {

	pkgfile, err := os.Create(outputDir + gitHash + "_package.xml");
	if (err != nil) {
		return err
	}

	defer pkgfile.Close()	

	fmt.Fprintf(pkgfile, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
	fmt.Fprintf(pkgfile, "<amcpackage xmlns=\"http://schemas.autodesk.com/amcpackage/2020/06\">\n");
	fmt.Fprintf(pkgfile, "  <build name=\"%s\" configuration=\"%s\" coreclient=\"%s\">\n", packageName, configName, clientName);

	fmt.Fprintf(pkgfile, "    <library name=\"core\" import=\"%s\" />\n", libraryName);
	fmt.Fprintf(pkgfile, "    <library name=\"datamodel\" import=\"%s_core_libmcdata.dll\" />\n", gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"lib3mf\" import=\"%s_core_lib3mf.dll\" />\n", gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"plugin_main\" import=\"%s_plugin_main.dll\" />\n", gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"plugin_pidcontrol\" import=\"%s_plugin_pidcontrol.dll\" />\n", gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"plugin_printerconnection\" import=\"%s_plugin_printerconnection.dll\" />\n", gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_marlin\" import=\"%s_driver_marlin.dll\" />\n", gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_scanlab\" import=\"%s_driver_scanlab.dll\" />\n", gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_camera\" import=\"%s_driver_camera.dll\" />\n", gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_scanlaboie\" import=\"%s_driver_scanlaboie.dll\" />\n", gitHash);
	fmt.Fprintf(pkgfile, "    <library name=\"driver_s7net\" import=\"%s_driver_s7net.dll\" />\n", gitHash);
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
	fmt.Fprintf(file, "  <server hostname=\"127.0.0.1\" port=\"8869\" />\n");
	fmt.Fprintf(file, "  <data directory=\"data/\" database=\"sqlite\" sqlitedb=\"storage.db\" />\n");
	fmt.Fprintf(file, "  <defaultpackage name=\"%s_package.xml\" githash=\"%s\" sha256=\"%s\" />\n", gitHash, gitHash, "");	
	fmt.Fprintf(file, "</amc>\n");


	return nil;

}

func main() {

	var Root DistXMLRoot;
	Root.XMLNs = "http://schemas.autodesk.com/amc/clientdistribution/2020/07";
		
	argsWithProg := os.Args;
	if (len (argsWithProg) < 3) {
		log.Fatal ("Please start with createDist <outputpath> <githash>");
	}
			
	
	OutputDir := filepath.Clean (argsWithProg[1]) + "/";
	hexSum := argsWithProg[2];
	
	
	ClientZIPName := hexSum + "_core_client.zip";
	LibraryName := hexSum + "_core_libmc.dll";
	ConfigName := hexSum + "_config.xml";
	
	DistXMLName := "dist.xml";
	
	packageName := "Build " + hexSum;
	
    newZipFile, err := os.Create(OutputDir + ClientZIPName)
    if err != nil {
        log.Fatal(err)
    }
    defer newZipFile.Close()

    zipWriter := zip.NewWriter(newZipFile)
    defer zipWriter.Close()	


	err = filepath.Walk("dist/",
		func(path string, info os.FileInfo, err error) error {		
			if err != nil {
				return err
			}
			
			if (!info.IsDir()) {
			
				slashpath := strings.ReplaceAll (path, "\\", "/");
				url := slashpath[5:];

				if (url == "index.html") {
					url = "";
				}				
				
				_, file := filepath.Split(slashpath);
				
				
				fileext := filepath.Ext(file);
				contenttype := mime.TypeByExtension(fileext);
				if (contenttype == "") {
				
					if (fileext==".ico") { contenttype="image/x-icon"; }
					if (fileext==".woff") { contenttype="font/woff"; }
					if (fileext==".woff2") { contenttype="font/woff2"; }
					if (fileext==".ttf") { contenttype="font/ttf"; }
				
				}
				
				
				if (fileext!=".map") {
				
					var entry DistXMLEntry;
					entry.URL = url;
					entry.FileName = file;
					entry.Size = uint32 (info.Size());
					entry.ContentType = contenttype;
					
					Root.Entries = append (Root.Entries, entry);
					
					fmt.Printf("Adding %s (%d bytes)\n", path, info.Size())
					
					input, err := ioutil.ReadFile(path)
					if err != nil {
						return err
					}
					
					
					var header zip.FileHeader;
					header.Name = file;
					header.Method = zip.Deflate;
					
					filewriter, err := zipWriter.CreateHeader(&header)
					if err != nil {
						return err
					}
					
					_, err = filewriter.Write(input)
					if err != nil {
						return err
					}
					
					/*err = ioutil.WriteFile(OutputDir + "/" + TargetDistDir + "/" + file, input, 0644)
					if err != nil {				
						return err;
					} */
				
				}
				
			}
			
			return nil
	})
	
	
	if err != nil {
		log.Fatal(err)
	}

	bytes, err := xml.Marshal (&Root);
	if err != nil {
		log.Fatal(err)
	}
	
	var header zip.FileHeader;
	header.Name = DistXMLName;
	header.Method = zip.Deflate;
				
	filewriter, err := zipWriter.CreateHeader(&header)
	if err != nil {
		log.Fatal(err)
	}
				
	_, err = filewriter.Write(bytes)
    if err != nil {
		log.Fatal(err)
	}
	
	_, err = os.Stat (OutputDir + "data");
	if (err != nil) {
		fmt.Printf("creating %s\n", OutputDir + "data");
		err = os.Mkdir(OutputDir + "data", 0755)
		if err != nil {
			log.Fatal(err)
		}
	}
	
	fmt.Printf("creating server config in %s\n", OutputDir);
	
	err = createMCServerTemplate (OutputDir, packageName, ClientZIPName, LibraryName, ConfigName, hexSum);
	if err != nil {
		log.Fatal(err)
	}
	
	
	

}