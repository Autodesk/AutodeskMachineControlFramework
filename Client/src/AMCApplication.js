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

import AMCApplicationModule_Content from "./AMCModule_Content.js"
import AMCApplicationModule_GLScene from "./AMCModule_GLScene.js"
import AMCApplicationModule_Graphic from "./AMCModule_Graphic.js"
import AMCApplicationModule_Grid from "./AMCModule_Grid.js"
import AMCApplicationModule_Tabs from "./AMCModule_Tabs.js"
import AMCApplicationModule_LayerView from "./AMCModule_LayerView.js"

import AMCApplicationPage from "./AMCPage.js"
import AMCApplicationDialog from "./AMCDialog.js"
import AMCUpload from "./AMCImplementation_Upload.js"


export default class AMCApplication extends Common.AMCObject {

    constructor(apiBaseURL) {
		
		super ();
		this.registerClass ("amcApplication");
		
        this.API = {
            baseURL: apiBaseURL,
            authToken: Common.nullToken (),
            unsuccessfulUpdateCounter: 0
        }

        this.AppState = {
            currentStatus: "initial", // one of "initial" / "login" / "ready" / "error",
            currentError: "",
            activePage: "",
            WebGLInstances: new Map()
        }

        this.AppDefinition = {
            TextApplicationName: "",
            TextCopyRight: "",
            MainPage: "",
            LogoUUID: "",
            LogoAspectRatio: 1.0,
            Colors: {}
        }

        this.AppContent = {
            MenuItems: [],
            ToolbarItems: [],
            Pages: [],
            Dialogs: [],
            PageMap: new Map(),
            DialogMap: new Map(),
			ModuleMap: new Map(),
			ItemMap: new Map(),			
            FormEntityMap: new Map()
        }

    }

    setStatus(newStatus) {
        this.AppState.currentStatus = newStatus;
        this.AppState.currentError = "";
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
    }

    requestLogin(userName, userPassword) {

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
                this.API.authToken = resultAuthenticate.data.token;
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

		if (moduleDefinitionJSON.type === "layerview") 
			return new AMCApplicationModule_LayerView (page, moduleDefinitionJSON);
		
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
			this.AppContent.Dialogs = [];
			this.AppContent.PageMap.clear ();
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

    updateContentItemResult(uuid, content) {
		
		Assert.UUIDValue (uuid);
		Assert.ObjectValue (content);
		Assert.ArrayValue (content.entries);
		
        if (uuid) {
            if (content) {
                if (content.entries) {
					
					if (this.AppContent.ItemMap.has (uuid)) {
						let item = this.AppContent.ItemMap.get (uuid);
						item.updateFromJSON (content);
					}

				}			
			}
        }
    }
	
	
    updateContentItem(item) {
		
		if (!item)
			return;

        item.refresh = false;

        let headers = {}
        let authToken = this.API.authToken;

        if (authToken != Common.nullToken ())
            headers.Authorization = "Bearer " + authToken;

        let url = this.API.baseURL + "/ui/contentitem/" + Assert.UUIDValue (item.uuid);
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
		

/*        


		*/
	

/*        let reader = new FileReader();
        reader.readAsArrayBuffer(chosenfile);

        reader.onload = () => {
            let fileContent = reader.result;

            itemstate.messages = ["Hashing file..."];

            let shaInstance = new asmCrypto.SHA256();
            shaInstance.reset();
            shaInstance.process(fileContent);
            shaInstance.finish();

            let bytesToHex = function (buffer) {
                let hex = "";
                let n;
                for (n in buffer) {
                    hex += ("0" + (0xff & buffer[n]).toString(16)).slice(-2);
                }
                return hex;
            }

            let sha256 = bytesToHex(shaInstance.result);

            shaInstance = null;

            itemstate.messages = ["Starting Upload..."];
            this.axiosPostRequest("/upload/", {
                "context": "build",
                "name": chosenfile.name,
                "size": chosenfile.size,
                "mimetype": "application/3mf",

            })

            .then(resultUploadInit => {
                let streamuuid = resultUploadInit.data.streamuuid;
                let contextuuid = resultUploadInit.data.contextuuid;

                const formData = new FormData();
                formData.append("size", chosenfile.size);
                formData.append("data", new Blob([fileContent], {
                        type: "application/3mf"
                    }), chosenfile.name);

                itemstate.messages = ["Uploading..."];

                this.axiosPostFormData("/upload/" + streamuuid, formData)

                .then(resultUploadHandle => {
                    resultUploadHandle;

                    this.axiosPostRequest("/upload/finish", {
                        "streamuuid": streamuuid,
                        "sha256": sha256
                    })
                    .then(resultUploadFinish => {

                        resultUploadFinish;

                        itemstate.messages = ["Preparing build..."];

                        this.axiosPostRequest("/build/prepare", {
                            "builduuid": contextuuid,
                        })
                        .then(resultBuildPrepare => {
                            resultBuildPrepare;
                            itemstate.messages = [];
                            itemstate.chosenFile = null;
                            itemstate.uploadid = 0;

                            if (successevent) {
                                let eventValues = {}
                                eventValues[itemuuid] = contextuuid;

                                this.triggerUIEvent(successevent, itemuuid, eventValues);
                            }

                        })
                        .catch(err => {
                            err;
                            if (failureevent)
                                this.triggerUIEvent(failureevent, itemuuid, {});
                        });
                    })
                    .catch(err => {
                        err;
                        if (failureevent)
                            this.triggerUIEvent(failureevent, itemuuid, {});
                    });

                })
                .catch(err => {
                    err;
                    if (failureevent)
                        this.triggerUIEvent(failureevent, itemuuid, {});
                });

            })
            .catch(err => {
                err;
                if (failureevent)
                    this.triggerUIEvent(failureevent, itemuuid, {});
            });

        }; */

    }

    changePage(page) {

        let pageString = String(page);
        this.AppState.activePage = pageString;

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

    }

    getImageURL(uuid) {
        return this.API.baseURL + '/ui/image/' + uuid;
    }

    triggerUIEvent(eventname, senderuuid, eventValues) {

        this.axiosPostRequest("/ui/event", {
            "eventname": eventname,
            "senderuuid": senderuuid,
            "formvalues": eventValues
        })
        .then(resultHandleEvent => {
            if (resultHandleEvent.data.pagetoactivate) {
                this.changePage(resultHandleEvent.data.pagetoactivate);
            }
            if (resultHandleEvent.data.dialogtoshow) {
                this.showDialog(resultHandleEvent.data.dialogtoshow);
            }
            if (resultHandleEvent.data.closedialogs) {
                this.closeAllDialogs();
            }
            if (resultHandleEvent.data.contentupdate) {
                for (let item of resultHandleEvent.data.contentupdate) {
                    if (item.uuid) {
                        if (item.entries) {
                            this.updateContentItemResult(item.uuid, item);
                        }
                    }
                }
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
				if (entity.value) {
					let formValue = entity.value;
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

}
