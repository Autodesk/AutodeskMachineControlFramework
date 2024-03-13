/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
 * Neither the name of the Autodesk Inc. nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

import * as Axios from "axios";
import * as asmCrypto from "asmcrypto-lite";

import * as Assert from "./AMCAsserts.js";
import * as Common from "./AMCCommon.js"
import * as GitHash from "./AMCGitHash.js"

import AMCApplicationModule_Content from "../modules/AMCModule_Content.js"
import AMCApplicationModule_GLScene from "../modules/AMCModule_GLScene.js"
import AMCApplicationModule_Graphic from "../modules/AMCModule_Graphic.js"
import AMCApplicationModule_Grid from "../modules/AMCModule_Grid.js"
import AMCApplicationModule_Tabs from "../modules/AMCModule_Tabs.js"
import AMCApplicationModule_Logs from "../modules/AMCModule_Logs.js"
import AMCApplicationModule_LayerView from "../modules/AMCModule_LayerView.js"
import AMCApplicationModule_Custom from "../modules/AMCModule_Custom.js"

import AMCApplicationPage from "./AMCPage.js"
import AMCApplicationCustomPage from "./AMCCustomPage.js"
import AMCUpload from "./AMCImplementation_Upload.js"

import AMCApplicationDialog from "./AMCDialog.js"

export default class AMCApplication extends Common.AMCObject {
	

    constructor(apiBaseURL, uiResizeEvent) {
		
		super ();
		this.registerClass ("amcApplication");
		
		console.log ("AMC Client git hash: " + GitHash.getClientGitHash ());
				
        this.API = {
            baseURL: apiBaseURL,
            authToken: Common.nullToken (),
            unsuccessfulUpdateCounter: 0,
			userUUID: Common.nullUUID (),
			userLogin: "",
			userDescription: "",
			userRole: "",
			userLanguage: "",
			userPermissions: new Set ()
        }

        this.AppState = {
            currentStatus: "initial", // one of "initial" / "login" / "ready" / "error",
            currentError: "",
            activePage: "",
			appResizeEvent: uiResizeEvent,
            WebGLInstances: new Map()
        }

        this.AppDefinition = {
            TextApplicationName: "",
			ToolbarLogoUUID: "",
            TextCopyRight: "",
            MainPage: "",
            LogoUUID: "",
            LogoAspectRatio: 1.0,
			LoginBackgroundImageUUID: "",
			LoginWelcomeMessage: "",
			FirstLaunchMode: false,
            Colors: {}
        }

        this.AppContent = {
            MenuItems: [],
            ToolbarItems: [],
            Pages: [],
            CustomPages: [],
            Dialogs: [],
            PageMap: new Map(),
            CustomPageMap: new Map(),
            DialogMap: new Map(),
			ModuleMap: new Map(),
			ItemMap: new Map(),			
            FormEntityMap: new Map()
        }
		
		this.SnackBar = {
			Visible: false,
			Timeout: -1,
			Text: "",
			Color: "secondary",
			FontColor: "white"			
		}

    }

    setStatus(newStatus) {
        this.AppState.currentStatus = newStatus;
        this.AppState.currentError = "";
		if (this.AppState.appResizeEvent)
			this.AppState.appResizeEvent ();
    }

    setStatusToError(message) {
        this.AppState.currentStatus = "error";
        this.AppState.currentError = message;
        this.closeAllDialogs();
    }

    axiosGetRequest(subURL) {
        let headers = {}
        let authToken = this.API.authToken;

        if (authToken != Common.nullToken ())
            headers.Authorization = "Bearer " + authToken;

        return Axios({
            method: "GET",
            "headers": headers,
            url: this.API.baseURL + subURL
        });
    }

    axiosGetArrayBufferRequest(subURL) {
        let headers = {}
        let authToken = this.API.authToken;

        if (authToken != Common.nullToken ())
            headers.Authorization = "Bearer " + authToken;

        return Axios({
            method: "GET",
            "headers": headers,
			"responseType": "arraybuffer",
            url: this.API.baseURL + subURL
        });
    }


    axiosPostRequest(subURL, data) {
        let headers = {}
        let authToken = this.API.authToken;

        if (authToken != Common.nullToken ())
            headers.Authorization = "Bearer " + authToken;

        return Axios({
            "method": "POST",
            "url": this.API.baseURL + subURL,
            "headers": headers,
            "data": data
        });
    }

    axiosPostFormData(subURL, formData) {
        let headers = {
            "Content-Type": "multipart/form-data"
        }
        let authToken = this.API.authToken;

        if (authToken != Common.nullToken ())
            headers.Authorization = "Bearer " + authToken;

        return Axios({
            "method": "POST",
            "url": this.API.baseURL + subURL,
            "headers": headers,
            "data": formData
        });
    }

    retrieveConfiguration(vuetifythemes) {
        this.axiosGetRequest("/ui/config")
        .then(resultJSON => {
            this.AppDefinition.TextApplicationName = resultJSON.data.appname;
            this.AppDefinition.TextCopyRight = resultJSON.data.copyright;
            this.AppDefinition.MainPage = resultJSON.data.mainpage;
            this.AppDefinition.LogoUUID = resultJSON.data.logouuid;
            this.AppDefinition.LoginBackgroundImageUUID = resultJSON.data.loginbackgrounduuid;
			this.AppDefinition.ToolbarLogoUUID = resultJSON.data.toolbarlogouuid;
			this.AppDefinition.LoginWelcomeMessage = resultJSON.data.loginwelcomemessage;

            this.AppDefinition.LogoAspectRatio = resultJSON.data.logoaspectratio;
            if (resultJSON.data.colors) {
                this.AppDefinition.Colors = resultJSON.data.colors;
            } else {
                this.AppDefinition.Colors = {};
            }
            this.setStatus("login");

            document.title = this.AppDefinition.TextApplicationName;

            if (vuetifythemes) {
                if (this.AppDefinition.Colors.primary)
                    vuetifythemes.light.primary = this.AppDefinition.Colors.primary;
                if (this.AppDefinition.Colors.secondary)
                    vuetifythemes.light.secondary = this.AppDefinition.Colors.secondary;
                if (this.AppDefinition.Colors.accent)
                    vuetifythemes.light.accent = this.AppDefinition.Colors.accent;
                if (this.AppDefinition.Colors.error)
                    vuetifythemes.light.error = this.AppDefinition.Colors.error;
            }

            this.changePage(this.AppDefinition.MainPage);
        })
        .catch(err => {
            this.setStatusToError(err.response.data.message);
        });
    }

    performLogout() {
        this.API.authToken = Common.nullToken ();
        this.API.unsuccessfulUpdateCounter = 0;
		this.API.userUUID = Common.nullUUID ();
		this.API.userLogin = "";
		this.API.userDescription = "";
		this.API.userRole = "";
		this.API.userLanguage = "";
		this.API.userPermissions = new Set ();
    }

    requestLogin(userName, userPassword) {

		this.performLogout ();

        this.axiosPostRequest("/auth/", {
            "username": userName
        })
        .then(resultCreateSession => {

            let sessionuuid = resultCreateSession.data.sessionuuid;
            let sessionkey = resultCreateSession.data.sessionkey;
            let loginsalt = resultCreateSession.data.loginsalt;
            let clientkey = sessionkey;

            let saltedpassword = asmCrypto.SHA256.hex(loginsalt + userPassword);
            let clientkeyhash = asmCrypto.SHA256.hex(clientkey + saltedpassword);
            let sessionkeyhash = asmCrypto.SHA256.hex(sessionkey + clientkeyhash);

            this.axiosPostRequest("/auth/" + sessionuuid, {
                "clientkey": clientkey,
                "password": sessionkeyhash
            })

            .then(resultAuthenticate => {
                this.API.authToken = Assert.SHA256Value (resultAuthenticate.data.token);
				this.API.userUUID = Assert.UUIDValue (resultAuthenticate.data.useruuid);
				this.API.userLogin = Assert.IdentifierString (resultAuthenticate.data.userlogin);
				this.API.userDescription = Assert.StringValue (resultAuthenticate.data.userdescription);
				this.API.userRole = Assert.IdentifierString (resultAuthenticate.data.userrole);
				//this.API.userLanguage = Assert.IdentifierString (resultAuthenticate.data.userlanguage);
				this.API.userPermissions = new Set ();
				
				let permissionArray = Assert.ArrayValue (resultAuthenticate.data.userpermissions);
				for (let permission of permissionArray) {
					this.API.userPermissions.add (permission);
				}
				
                this.setStatus("ready");

                this.retrieveStateUpdate();

            })
            .catch(err => {
				if (err.response) {
					this.setStatusToError(err.response.data.message.toString ());
				} else {
					this.setStatusToError(err.toString ());
				}
            });

        })
        .catch(err => {
			if (err.response) {
				this.setStatusToError(err.response.data.message.toString ());
			} else {
				this.setStatusToError(err.toString ());
			}
        });

    }
	
	
	createModuleInstance (page, moduleDefinitionJSON)
	{
		Assert.ObjectValue (moduleDefinitionJSON);
		Assert.ObjectInstance (page, "amcPage");
		
		if (moduleDefinitionJSON.type === "content") 
			return new AMCApplicationModule_Content (page, moduleDefinitionJSON);

		if (moduleDefinitionJSON.type === "glscene") 
			return new AMCApplicationModule_GLScene (page, moduleDefinitionJSON);

		if (moduleDefinitionJSON.type === "graphic") 
			return new AMCApplicationModule_Graphic (page, moduleDefinitionJSON);

		if (moduleDefinitionJSON.type === "grid") 
			return new AMCApplicationModule_Grid (page, moduleDefinitionJSON);

		if (moduleDefinitionJSON.type === "tabs") 
			return new AMCApplicationModule_Tabs (page, moduleDefinitionJSON);

		if (moduleDefinitionJSON.type === "logs") 
			return new AMCApplicationModule_Logs (page, moduleDefinitionJSON);

		if (moduleDefinitionJSON.type === "layerview") 
			return new AMCApplicationModule_LayerView (page, moduleDefinitionJSON);

		if (moduleDefinitionJSON.type === "custom") 
			return new AMCApplicationModule_Custom (page, moduleDefinitionJSON);
		
		return null;
		
	}
	
	
	addModule (moduleInstance) {			
		if (moduleInstance && moduleInstance.uuid) {			
			this.AppContent.ModuleMap.set (moduleInstance.uuid, moduleInstance);			
			
		}	
	}
	
	addItem (item) {
		if (item && item.uuid) {
			this.AppContent.ItemMap.set (item.uuid, item);
		}
	}
				

    retrieveStateUpdate() {

        this.axiosGetRequest("/ui/state")

        .then(resultJSON => {
			
			this.AppContent.Pages = [];
			this.AppContent.CustomPages = [];
			this.AppContent.Dialogs = [];
			this.AppContent.PageMap.clear ();
			this.AppContent.CustomPageMap.clear ();
			this.AppContent.DialogMap.clear ();
			this.AppContent.ModuleMap.clear ();
			this.AppContent.ItemMap.clear ();
			
            this.AppContent.MenuItems = resultJSON.data.menuitems;
            this.AppContent.ToolbarItems = resultJSON.data.toolbaritems;

            for (let pageJSON of resultJSON.data.pages) {
				
				let page = new AMCApplicationPage (this, pageJSON);
				this.AppContent.Pages.push (page);
                this.AppContent.PageMap.set(page.name, page);
            }

			if (resultJSON.data.custompages) {
				for (let customPageJSON of resultJSON.data.custompages) {
					
					let custompage = new AMCApplicationCustomPage (this, customPageJSON);
					
					//alert (custompage.component);
					
					this.AppContent.CustomPages.push (custompage);
					this.AppContent.CustomPageMap.set(custompage.name, custompage);

				}
			}


            for (let dialogJSON of resultJSON.data.dialogs) {
				let dialog = new AMCApplicationDialog (this, dialogJSON);
				this.AppContent.Dialogs.push (dialog);
                this.AppContent.DialogMap.set(dialog.name, dialog);
				
            }

        })
        .catch(err => {
			if (err.response) {
				this.setStatusToError(err.response.data.message.toString ());
			} else {
				this.setStatusToError(err.toString ());
			}
        });
    }

   
	
    updateContentItem(item) {
		
		if (!item)
			return;

        item.refresh = false;

        let headers = {}
        let authToken = this.API.authToken;

        if (authToken != Common.nullToken ())
            headers.Authorization = "Bearer " + authToken;

		let stateidstring = "";
		if (item.stateid > 0)
			stateidstring = "/" + item.stateid;
		
        let url = this.API.baseURL + "/ui/contentitem/" + Assert.UUIDValue (item.uuid) + stateidstring;
        Axios({
            method: "GET",
            "headers": headers,
            url: url
        })
        .then(resultJSON => {
						
			if (resultJSON.data) {
				if (resultJSON.data.content) {
					item.updateFromJSON (resultJSON.data.content);					
				}				
			}
						
            this.unsuccessfulUpdateCounter = 0;
            item.setRefreshFlag ();

        })
        .catch(err => {

            this.unsuccessfulUpdateCounter = this.unsuccessfulUpdateCounter + 1;
            if (this.unsuccessfulUpdateCounter > 5) {
                this.setStatusToError(err.message);
            } else {
                item.setRefreshFlag ();
            }

        });

    }


    updateContentItems() {
		
		let uuid, item;
		
		if (this.AppContent.ItemMap) {

			for ([uuid, item] of this.AppContent.ItemMap) {
									
				if (item.refresh) {
					uuid;
					this.updateContentItem(item);
				}
			}
		
		}
		
	}
	
	
	onJobUploadChunkSuccess (application, uploadObject, chunkData, uploadOffset) {
		
		Assert.ObjectInstance (application, "amcApplication");
		Assert.ObjectInstance (uploadObject, "amcUpload");
		Assert.IntegerValue (uploadOffset);
		
		if (!uploadObject.checkIfUploadIsActive())
			return;
		
		uploadObject.setStateMessageToProgress ();
		
		const formData = new FormData();
		formData.append("size", chunkData.byteLength);
		formData.append("offset", uploadOffset);
		formData.append("data", new Blob([chunkData], {
                        type: "application/3mf"
                    }), uploadObject.getFileName ());

		application.axiosPostFormData("/upload/" + uploadObject.streamuuid, formData)
			.then(resultUploadHandle => {
				
				if (!uploadObject.checkIfUploadIsActive())
					return;
				
				resultUploadHandle;
				if (!uploadObject.readChunk (application, application.onJobUploadChunkSuccess)) {
					
					let checkSum = uploadObject.calculateChecksum ();
					
					uploadObject.setStateMessageToWaiting ();
					
                    application.axiosPostRequest("/upload/finish", {
                        "streamuuid": uploadObject.streamuuid,
                        "sha256": checkSum
                    })
                    .then(resultUploadFinish => {

                        resultUploadFinish;

						uploadObject.setStateMessageToPreparing ();

                        application.axiosPostRequest("/build/prepare", {
                            "builduuid": uploadObject.contextuuid,
                        })
                        .then(resultBuildPrepare => {
							
							if (!uploadObject.checkIfUploadIsActive())
								return;
							
                            resultBuildPrepare;
							uploadObject.clearUploadState ();
							
                            if (uploadObject.hasSuccessEvent ()) {
								
								let itemuuid = uploadObject.getItemUUID ();
                                let eventValues = {}
                                eventValues[itemuuid] = uploadObject.contextuuid;

                                application.triggerUIEvent(uploadObject.getSuccessEvent (), itemuuid, eventValues); 
                            } 

                        })
                        .catch(err => {
                            err;
//                            if (failureevent)
                                //this.triggerUIEvent(failureevent, itemuuid, {});
                        });
                    })					
					
					
				}
			})
            .catch(err => {
                err;
            });
		
		
	}
	

    performJobUpload(itemState, successEventName, failureEventName) {
		
		Assert.ObjectInstance (itemState, "amcUploadState");
		
		let chosenfile = itemState.getChosenFile ();		
							
		this.axiosPostRequest("/upload/", {
			"context": "build",
            "name": chosenfile.name,
            "size": chosenfile.size,
            "mimetype": "application/3mf",
        })

            .then(resultUploadInit => {
				
				Assert.ObjectValue (resultUploadInit);
				Assert.ObjectValue (resultUploadInit.data);				
                let streamuuid = Assert.UUIDValue (resultUploadInit.data.streamuuid);
                let contextuuid = Assert.UUIDValue (resultUploadInit.data.contextuuid);
												
				let currentUpload = new AMCUpload (itemState, streamuuid, contextuuid);
				currentUpload.setSuccessEvent (successEventName);
				currentUpload.setFailureEvent (failureEventName);
				
				//let reader = new FileReader();
				currentUpload.readChunk (this, this.onJobUploadChunkSuccess);
				
				
			})
            .catch(err => {
                err;
/*                if (failureeventname)
                    this.triggerUIEvent(failureeventname, itemuuid, {}); */
            });
		

    }

    changePage(page) {

        let pageString = String(page);
        this.AppState.activePage = pageString;
		
		if (this.AppState.appResizeEvent)
			this.AppState.appResizeEvent ();

    }

    closeAllDialogs() {
        let dialog;
        for (dialog of this.AppContent.Dialogs) {
            dialog.dialogIsActive = false;
        }
    }

    showDialog(dialog) {

        this.closeAllDialogs();

        if (dialog) {

            if (this.AppContent.DialogMap.has(dialog)) {
                let dialogObject = this.AppContent.DialogMap.get(dialog);
                dialogObject.dialogIsActive = true;
            }
        }
		
		if (this.AppState.appResizeEvent)
			this.AppState.appResizeEvent ();
		

    }
	
	streamDownload (downloadticketuuid, downloadfilename) 
	{
				
	
		// Start download in Browser
		const a = document.createElement('a');
		a.href = this.getDownloadURL (downloadticketuuid);
		a.download = downloadfilename;
		document.body.appendChild(a);    
		a.click();
		document.body.removeChild(a);
		
	}

    getImageURL(uuid) {
        return this.API.baseURL + '/ui/image/' + uuid;
    }
	
    getChartURL(uuid) {
        return this.API.baseURL + '/ui/chart/' + uuid;
    }
	
    getDownloadURL(uuid) {
        return this.API.baseURL + '/ui/download/' + uuid;
    }

    triggerUIEvent(eventname, senderuuid, eventValues, executionCallback) {

        this.axiosPostRequest("/ui/event", {
            "eventname": eventname,
            "senderuuid": senderuuid,
            "formvalues": eventValues
        })
        .then(resultHandleEvent => {
			
			if (resultHandleEvent.data.actions) {
				if (Array.isArray(resultHandleEvent.data.actions)) {
					let action;
					for (action of resultHandleEvent.data.actions) {
						if (action.action === "activatemodaldialog") {
							this.showDialog(action.dialogname);
						}
						if (action.action === "activatepage") {
							this.changePage(action.pagename);
						}
						if (action.action === "closemodaldialog") {
							this.closeAllDialogs();
						}
						if (action.action === "streamdownload") {
							this.streamDownload(action.downloadticket, action.downloadfilename);
						}
						
						//this.updateContentItemResult(item.uuid, item);
					}
				}
			}
			
			if (executionCallback) {
				executionCallback ();
			}				
			
        })
        .catch(err => {
            console.log(err);
        });
    }

    assembleFormValues(formValueUUIDList) {
		
		
		Assert.ArrayValue (formValueUUIDList);
		
        let resultObject = {}
        for (let entityUUID of formValueUUIDList) {
			
			if (this.AppContent.FormEntityMap.has (entityUUID)) {			
				let entity = this.AppContent.FormEntityMap.get (entityUUID);
				if (entity.dataObject) {
					let formValue = entity.dataObject.value;
					resultObject[entityUUID] = formValue;
				}
			}
        }

        return resultObject;

    }

    retrieveWebGLInstance(uuid) {
        return this.AppState.WebGLInstances[uuid];
    }

    storeWebGLInstance(uuid, instance) {
        this.AppState.WebGLInstances[uuid] = instance;
    }
	
	findPage (pageName) {
		
		if (this.PageMap.has (pageName))
			return this.PageMap.get (pageName);
		
		return null;
		
	}

	findCustomPage (pageName) {
		
		if (this.CustomPageMap.has (pageName))
			return this.CustomPageMap.get (pageName);
		
		return null;
		
	}
	
	userIsLoggedIn ()
	{
		return (this.API.authToken !== Common.nullToken ());
	}

	userUUID ()
	{
		if (!this.userIsLoggedIn ())
			throw "could not get user UUID: user is not logged in";
			
		return this.API.userUUID;
	}

	userLogin ()
	{
		if (!this.userIsLoggedIn ())
			throw "could not get user login: user is not logged in";

		return this.API.userLogin;
	}

	userDescription ()
	{
		if (!this.userIsLoggedIn ())
			throw "could not get user description: user is not logged in";

		return this.API.userDescription;
	}

	userRole ()
	{
		if (!this.userIsLoggedIn ())
			throw "could not get user role: user is not logged in";
		
		return this.API.userRole;
	}

	userLanguage ()
	{
		if (!this.userIsLoggedIn ())
			throw "could not get user language: user is not logged in";
		
		return this.API.userLanguage;
	}

	userPermissions ()
	{
		if (!this.userIsLoggedIn ())
			throw "could not get user permissions: user is not logged in";
		
		return new Set(this.API.userPermissions);
	}
	
	checkPermission (permissionIdentifier)
	{
		if (!this.userIsLoggedIn ())
			return false;
		
		return this.API.userPermissions.has (Assert.IdentifierString (permissionIdentifier));
	}
	
	generateUserPassword (clearTextPassword)
	{
		if (!clearTextPassword.isString ()) 
			throw "could not set user password: invalid input string";
		
		let trimmedPassword = clearTextPassword.trim ();
		
		if (trimmedPassword.length < Common.minimumPasswordLength ())
			throw "could not set user password: invalid user password length";
					
		if (!this.userIsLoggedIn ())
			throw "could not get user permissions: user is not logged in";
		
		const randomArray = new Uint32Array(1024);
		asmCrypto.random.getValues( randomArray );
		
		let randomString = this.API.authToken;
		for (let value in randomArray)
			randomString = randomString + value.toString ();

		let passwordSalt = asmCrypto.SHA256.hex(randomString);
		let passwordHash = asmCrypto.SHA256.hex(passwordSalt + trimmedPassword);
		
		return {
			"salt": passwordSalt,
			"hash": passwordHash
		}
		
	}


}
