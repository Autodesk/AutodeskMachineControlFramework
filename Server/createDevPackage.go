
package main

import (
	"log"
	"os"
	"fmt"
	"io/ioutil"
	"strings"
	"path/filepath"
	"archive/zip"
)

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
//////////////////////////////////////////////////////////////////////////////////////////////////////

func addFile (zipWriter * zip.Writer, diskPath string, zipPath string) (error) {

	fmt.Printf("Adding %s\n", diskPath)

	input, err := ioutil.ReadFile(diskPath)
	if err != nil {
		return err
	}
		
		
	var header zip.FileHeader;
	header.Name = zipPath;
	header.Method = zip.Deflate;
		
	filewriter, err := zipWriter.CreateHeader(&header)
	if err != nil {
		return err
	}
		
	_, err = filewriter.Write(input)
	if err != nil {
		return err
	}
	
	return nil;

}


func addDirectory (zipWriter * zip.Writer, basePath string, relativePath string) (error) {

	err := filepath.Walk(basePath + relativePath,
		func(path string, info os.FileInfo, err error) error {		
			if err != nil {
				return err
			}
			
			if (!info.IsDir()) {
			
				slashpath := strings.ReplaceAll (path, "\\", "/");
				
				_, file := filepath.Split(slashpath);
																
				err = addFile (zipWriter, path, relativePath + file);
					
			}
						
			
		return nil
	})
	
	
	return err;
}


func main() {
			
	argsWithProg := os.Args;
	if (len (argsWithProg) < 4) {
		log.Fatal ("Please start with createDist <devpackagepath> <outputpath> <githash>");
	}
			
	
	DevPackageDir := filepath.Clean (argsWithProg[1]) + "/";
	OutputPath := filepath.Clean (argsWithProg[2]) + "/";
	hexSum := argsWithProg[3];
		
	zipName := "AMCF_" + hexSum + ".zip"
		
    newZipFile, err := os.Create(OutputPath + zipName)
    if err != nil {
        log.Fatal(err)
    }
    defer newZipFile.Close()

    zipWriter := zip.NewWriter(newZipFile)
    defer zipWriter.Close()	

	
	err = addDirectory (zipWriter, DevPackageDir, "Dist/");
	if err != nil {
		log.Fatal(err)
	}
	
	err = addDirectory (zipWriter, DevPackageDir, "HeadersDev/CppDynamic/");
	if err != nil {
		log.Fatal(err)
	}
	
	err = addDirectory (zipWriter, DevPackageDir, "HeadersDriver/CppDynamic/");
	if err != nil {
		log.Fatal(err)
	}

	err = addDirectory (zipWriter, DevPackageDir, "InterfacesDev/");
	if err != nil {
		log.Fatal(err)
	}

	err = addDirectory (zipWriter, DevPackageDir, "PluginCpp/");
	if err != nil {
		log.Fatal(err)
	}
	
	err = addFile (zipWriter, DevPackageDir + "buildresources.exe", "buildresources.exe");
	if err != nil {
		log.Fatal(err)
	}

	err = addFile (zipWriter, DevPackageDir + "buildresources.linux", "buildresources.linux");
	if err != nil {
		log.Fatal(err)
	}

	err = addFile (zipWriter, DevPackageDir + "buildresources.arm", "buildresources.arm");
	if err != nil {
		log.Fatal(err)
	}
}