
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

func addFileToZIP (path string, zipfilename string, zipWriter * zip.Writer) error {

	var header zip.FileHeader;
	header.Name = zipfilename;
	header.Method = zip.Deflate;
	
	input, err := ioutil.ReadFile(path)
	if err != nil {
		return err
	}				
	
	filewriter, err := zipWriter.CreateHeader(&header)
	if err != nil {
		return err
	}
	
	_, err = filewriter.Write(input)
	return err
				
}


func main() {
		
	argsWithProg := os.Args;
	if (len (argsWithProg) < 3) {
		log.Fatal ("Please start with createClientSource <inputpath> <outputfile>");
	}

	var Root DistXMLRoot;
	Root.XMLNs = "http://schemas.autodesk.com/amc/resourcepackage/2020/07";
				
	inputPath := argsWithProg[1];
	outputFileName := argsWithProg[2];

    newZipFile, err := os.Create(outputFileName)
    if err != nil {
        log.Fatal(err)
    }
    defer newZipFile.Close()

    zipWriter := zip.NewWriter(newZipFile)
    defer zipWriter.Close()
	
	var prefix string;
		
	walkFunc := func(path string, info os.FileInfo, err error) error {		
			if err != nil {
				return err
			}
			
			if (!info.IsDir()) {
			
				slashpath := strings.ReplaceAll (path, "\\", "/");
							
				_, file := filepath.Split(slashpath);							
							
				fmt.Println ("adding " + prefix + "/" + file + "...");
				
				addFileToZIP (path, prefix + "/" + file, zipWriter);
				
			}
			
			return nil
	};


	prefix = "common";

	err = filepath.Walk(inputPath + "/src/" + prefix, walkFunc)
	
	if err != nil {
		log.Fatal(err)
	}
	
	prefix = "modules";

	err = filepath.Walk(inputPath + "/src/" + prefix, walkFunc)

	if err != nil {
		log.Fatal(err)
	}
	
	prefix = "dialogs";

	err = filepath.Walk(inputPath + "/src/" + prefix, walkFunc)

	if err != nil {
		log.Fatal(err)
	}
	
	err = addFileToZIP (inputPath + "/package.json", "packages/package.json", zipWriter);
	if err != nil {
		log.Fatal(err)
	}

	err = addFileToZIP (inputPath + "/package-lock.json", "packages/package-lock.json", zipWriter);
	if err != nil {
		log.Fatal(err)
	}

}