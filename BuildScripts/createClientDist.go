
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


func main() {
		
	argsWithProg := os.Args;
	if (len (argsWithProg) < 3) {
		log.Fatal ("Please start with createClientDist <inputpath> <outputfile>");
	}

	var Root DistXMLRoot;
	Root.XMLNs = "http://schemas.autodesk.com/amc/resourcepackage/2020/07";
				
	inputPath := argsWithProg[1];
	outputFileName := argsWithProg[2];

	DistXMLName := "package.xml";

    newZipFile, err := os.Create(outputFileName)
    if err != nil {
        log.Fatal(err)
    }
    defer newZipFile.Close()

    zipWriter := zip.NewWriter(newZipFile)
    defer zipWriter.Close()


	err = filepath.Walk(inputPath,
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
					entry.Name = url;
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
	
}