
package main

import (
	"log"
	"os"
	"fmt"
	"errors"
	"io/ioutil"
	"strings"
	"path/filepath"
	"encoding/xml"
	"archive/zip"
	"mime"
)


type PackageXMLEntry struct {
	XMLName xml.Name `xml:"entry"`
	Name string `xml:"name,attr"`
	FileName string `xml:"filename,attr"`
	Size uint32 `xml:"size,attr"`
	ContentType string `xml:"contenttype,attr"`
}


type PackageXMLRoot struct {
	XMLName xml.Name `xml:"package"`
	XMLNs string `xml:"xmlns,attr"`
	Entries []PackageXMLEntry `xml:"entry"`
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
//////////////////////////////////////////////////////////////////////////////////////////////////////

func main() {

	var Root PackageXMLRoot;
	Root.XMLNs = "http://schemas.autodesk.com/amc/resourcepackage/2020/07";
	
		
	argsWithProg := os.Args;
	if (len (argsWithProg) < 3) {
		log.Fatal ("Please start with buildResources <inputpath> <outputfile>");
	}
			
	
	InputDir := filepath.Clean (argsWithProg[1]) + "/";
	OutputFileName := argsWithProg[2];
				
	PackageXMLName := "package.xml";
		
    newZipFile, err := os.Create(OutputFileName)
    if err != nil {
        log.Fatal(err)
    }
    defer newZipFile.Close()

    zipWriter := zip.NewWriter(newZipFile)
    defer zipWriter.Close()	
	
	
	writtenResources := make (map[string]string);


	err = filepath.Walk(InputDir,
		func(path string, info os.FileInfo, err error) error {		
			if err != nil {
				return err
			}
			
			if (!info.IsDir()) {
			
				slashpath := strings.ReplaceAll (path, "\\", "/");
				
				_, file := filepath.Split(slashpath);
				
				
				fileext := filepath.Ext(file);

				lenwithoutext := len (slashpath) - len (fileext);
				resourcename := slashpath[0:lenwithoutext];

				if (resourcename == "") {
					return errors.New ("Invalid resource name for: " + file);
				}

				
				if (writtenResources[resourcename] != "") {
					return errors.New ("Duplicate resource name: " + resourcename);
				}
				writtenResources[resourcename] = resourcename;
				
				contenttype := mime.TypeByExtension(fileext);
				if (contenttype == "") {
				
					if (fileext==".ico") { contenttype="image/x-icon"; }
					if (fileext==".woff") { contenttype="font/woff"; }
					if (fileext==".woff2") { contenttype="font/woff2"; }
					if (fileext==".ttf") { contenttype="font/ttf"; }
				
				}
				
				if (contenttype == "") {
					return errors.New ("Invalid content type for file extension: " + fileext);
				}

				
				if (file != PackageXMLName) {
								
					var entry PackageXMLEntry;
					entry.Name = resourcename;
					entry.FileName = file;
					entry.Size = uint32 (info.Size());
					entry.ContentType = contenttype;
					
					Root.Entries = append (Root.Entries, entry);
					
					fmt.Printf("Adding %s as %s (%d bytes)\n", path, resourcename, info.Size())
					
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
	header.Name = PackageXMLName;
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