package main

import (
	"log"
	"net/http"
	"io/ioutil"
	"encoding/hex"
	"io"
	"crypto/sha256"
	"os"	
)


func main() {
		
	argsWithProg := os.Args;
	
	
	if (len (argsWithProg) < 2) {
		log.Fatal ("Please start with packageManager <command> <parameter1> <parameter2> ....");
	}
	
	commandToUse := argsWithProg[1];
	
	if (commandToUse == "download") {
	
	
		if (len (argsWithProg) < 6) {
			log.Fatal ("Please start with packageManager download <localfilename> <repositoryurl> <githash> <sha256checksum>");
		}
					
		downloadURLBase := argsWithProg[3];
		localFileName := argsWithProg[2];
		gitHash := argsWithProg[4];
		shaCheckSum := argsWithProg[5]; 

		log.Println ("---------------------------------------------------------------------------------");
		log.Println ("-- AMCF Package Manager                                                        --");
		log.Println ("---------------------------------------------------------------------------------");
		log.Println ("Repository URL:" + downloadURLBase);
		log.Println ("Local Filename:" + localFileName);
		log.Println ("GIT Hash:" + gitHash);
		log.Println ("SHA 256 Checksum:" + shaCheckSum);
		log.Println ("---------------------------------------------------------------------------------");
		
		log.Println ("Verifying " + localFileName + "...");
		
		calculatedCheckSum := "";	
		_, err := os.Stat(localFileName);
		if err != nil {
			if !os.IsNotExist(err) {
				log.Fatal(err);				
			}

			log.Println ("Local package does not exist...");

		} else {
			
			hasher := sha256.New()
			fileContent, err := ioutil.ReadFile(localFileName)    
			hasher.Write(fileContent)
			
			if err != nil {
				log.Fatal(err)
			}
			calculatedCheckSum = hex.EncodeToString(hasher.Sum(nil));		
			log.Println ("Calculated Checksum: " + calculatedCheckSum);	
			
			if (calculatedCheckSum != shaCheckSum) {
				log.Println ("Checksum mismatch!");
			}
			
		}
		
		
		if (calculatedCheckSum != shaCheckSum) {

			log.Println ("Creating " + localFileName + "...");
			targetFile, err := os.Create(localFileName)
			if err != nil {
				log.Fatal(err)
			}
			defer targetFile.Close()
			
			downloadURL := downloadURLBase + "/amcf_" + gitHash + ".zip";
			log.Println ("Downloading package from " + downloadURL);
				
			response, err := http.Get(downloadURL);
			defer response.Body.Close()

			_, err = io.Copy(targetFile, response.Body)
			if err != nil {
				log.Fatal(err)
			}	
		
			hasher := sha256.New()
			fileContent, err := ioutil.ReadFile(localFileName)    
			hasher.Write(fileContent)
		
			if err != nil {
				log.Fatal(err)
			}
			finalCheckSum := hex.EncodeToString(hasher.Sum(nil));
			log.Println ("Final Checksum: " + finalCheckSum);	
			
			
			if (finalCheckSum != shaCheckSum) {
				log.Fatal ("Checksum mismatch!");
			}
		
		}
		
			
		log.Println ("done..");
	
	
	} else {
	
		log.Fatal ("Unknown command. Valid commands are download and extract.");
	}
	

}