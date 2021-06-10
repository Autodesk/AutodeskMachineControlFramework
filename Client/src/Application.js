
import * as Axios from "axios";
import * as asmCrypto from "asmcrypto-lite";


const nullUUID = "00000000-0000-0000-0000-000000000000";
const nullToken = "0000000000000000000000000000000000000000000000000000000000000000";


export default class AMCApplication {
	
	constructor (apiBaseURL)
	{
		this.API = {
			baseURL : apiBaseURL,
			authToken: nullToken			
		}

		this.AppState = {
            currentStatus: "initial", // one of "initial" / "login" / "ready" / "error",
            currentError: "",
            activePage: "",
			activeObject: nullUUID
		}

        this.AppDefinition = {
            TextApplicationName: "",
            TextCopyRight: "",
            MainPage: "",
			LogoUUID: "",
			LogoAspectRatio: 1.0
        }
		
		this.AppContent = {
            MenuItems: [],
            ToolbarItems: [],
			ContentItems: [],					
			Pages: []
		}

		
	}


	setStatus (newStatus)
	{
		this.AppState.currentStatus = newStatus;
		this.AppState.currentError = "";
	}


	setStatusToError (message)
	{
		this.AppState.currentStatus = "error";
		this.AppState.currentError = message;
	}
	
	
	axiosGetRequest (subURL)
	{
		var headers = {}
		var authToken = this.API.authToken; 
		
		if (authToken != nullToken)
			headers.Authorization = "Bearer " + authToken;
		
		return Axios ({
                    method: "GET",
					"headers": headers,
                    url: this.API.baseURL + subURL
               });
	}		

	axiosPostRequest (subURL, data)
	{
		var headers = {}
		var authToken = this.API.authToken; 
		
		if (authToken != nullToken)
			headers.Authorization = "Bearer " + authToken;
		
		return Axios ({
                    "method": "POST",
                    "url": this.API.baseURL + subURL,
					"headers": headers,
					"data": data
               });
	}		


	axiosPostFormData (subURL, formData)
	{
		var headers = {	"Content-Type": "multipart/form-data" }
		var authToken = this.API.authToken; 
		
		if (authToken != nullToken)
			headers.Authorization = "Bearer " + authToken;
		
		return Axios ({
                    "method": "POST",
                    "url": this.API.baseURL + subURL,
					"headers": headers,
					"data": formData
               });
	}		

				
	retrieveConfiguration () 
	{
		this.axiosGetRequest ("/ui/config")
			.then(resultJSON => {
				this.AppDefinition.TextApplicationName = resultJSON.data.appname;
				this.AppDefinition.TextCopyRight = resultJSON.data.copyright;
				this.AppDefinition.MainPage = resultJSON.data.mainpage;
				this.AppDefinition.LogoUUID = resultJSON.data.logouuid;
				this.AppDefinition.LogoAspectRatio = resultJSON.data.logoaspectratio;			
				this.setStatus ("login");
					
				document.title = this.AppDefinition.TextApplicationName;
				
				this.changePage (this.AppDefinition.MainPage);
			})
			.catch(err => {
				this.setStatusToError (err.response.data.message);
			});
	}				
	
	requestLogin (userName, userPassword) 
	{
		
		this.axiosPostRequest ("/auth/", 
			{
				"username": userName
			})
			.then(resultCreateSession => {
				
					var sessionuuid = resultCreateSession.data.sessionuuid;
					var sessionkey = resultCreateSession.data.sessionkey;
					var loginsalt = resultCreateSession.data.loginsalt;
					var clientkey = sessionkey;
					
					var saltedpassword = asmCrypto.SHA256.hex (loginsalt + userPassword);
					var clientkeyhash = asmCrypto.SHA256.hex (clientkey + saltedpassword);
					var sessionkeyhash = asmCrypto.SHA256.hex (sessionkey + clientkeyhash);
									
					this.axiosPostRequest ("/auth/" + sessionuuid, 
						{
							"clientkey": clientkey,
							"password": sessionkeyhash
						})
											
					.then(resultAuthenticate => {										
						this.API.authToken = resultAuthenticate.data.token;
						this.setStatus ("ready");	
						
						this.retrieveStateUpdate ();
						
					})
					.catch(err => {
						this.setStatusToError (err.response.data.message);
					}); 
									  
					
                })
                .catch(err => {
					this.setStatusToError (err.response.data.message);
                });	
			
	}
	
	
	prepareModuleItem (item) 
	{
		if (item.type === "parameterlist") {
		
			this.AppContent.ContentItems[item.uuid] = { uuid: item.uuid, entries: [], refresh: true };
			item.entries = this.AppContent.ContentItems[item.uuid].entries;
			
		}

		if (item.type === "buildlist") {
		
			this.AppContent.ContentItems[item.uuid] = { uuid: item.uuid, entries: [], refresh: true };
			item.entries = this.AppContent.ContentItems[item.uuid].entries;
			
		}
		
		if (item.type === "upload") {
			item.state = { uploadid: 0, chosenFile: null, idcounter: 0, messages: [] }
		
		}
	}
	

	prepareModule (module) 
	{
		var item, tab, section;
		
		if (module.type === "content") {
			for (item of module.items) {
				this.prepareModuleItem (item)
			}
			
		}

		if (module.type === "tabs") {
			for (tab of module.tabs) {
				this.prepareModule (tab)
			}			
		}

		if (module.type === "verticalsplit") {
			for (section of module.sections) {
				this.prepareModule (section)
			}			
		}

		if (module.type === "horizontalsplit") {
			for (section of module.sections) {
				this.prepareModule (section)
			}			
		}
	}

	
	retrieveStateUpdate () {
		
		this.axiosGetRequest ("/ui/state")		
		
                .then(resultJSON => {
                    this.AppContent.MenuItems = resultJSON.data.menuitems;
                    this.AppContent.ToolbarItems = resultJSON.data.toolbaritems;
					
					var page, module;
					for (page of resultJSON.data.pages) {
						for (module of page.modules) {
							this.prepareModule (module)
						}						
					
					}
					
					this.AppContent.Pages = resultJSON.data.pages;
                   
                })
                .catch(err => {
                    this.setStatusToError (err.response.data.message);
                });
        }	


		updateContentItem (uuid) {
		
			this.AppContent.ContentItems[uuid].refresh = false;
		
            var url = this.API.baseURL + "/ui/contentitem/" + uuid;
            Axios({
                    method: "GET",
                    url: url
                })
                .then(resultJSON => {					

					var oldentrycount = this.AppContent.ContentItems[uuid].entries.length;					
					for (var i = 0; i < oldentrycount; i++) {
						this.AppContent.ContentItems[uuid].entries.pop ();
					}
					
					for (var entry of resultJSON.data.content.entries) {
						this.AppContent.ContentItems[uuid].entries.push (entry);
					}
					this.AppContent.ContentItems[uuid].refresh = true;
                })
                .catch(err => {
					err;
                    this.AppContent.ContentItems[uuid].refresh = true;                    
                });
				
		}

		updateContentItems () {

			for (var key in this.AppContent.ContentItems) {
				var item = this.AppContent.ContentItems [key];
				if (item.refresh) {				
					this.updateContentItem (item.uuid);
				}
			}
			
		}


		performJobUpload (itemuuid, itemstate, uploadid, chosenfile, successpage) {
					
		
			// Attention: itemstate might change with UI interaction. Always check if uploadid matches!						
			itemstate.messages = ["Reading file..."];
			
			var reader = new FileReader();
			reader.readAsArrayBuffer (chosenfile);		
			
			reader.onload = () => {
				var fileContent = reader.result;						

				itemstate.messages = ["Hashing file..."];
				
				var shaInstance = new asmCrypto.SHA256 (); 
				shaInstance.reset ();
				shaInstance.process (fileContent);
				shaInstance.finish ();
				
				var bytesToHex = function (buffer) {
					var hex = "";
					var n;
					for (n in buffer) {
						hex += ("0" + (0xff & buffer[n]).toString(16)).slice(-2);
					}
					return hex;
				}
				
				var sha256 = bytesToHex (shaInstance.result);

				shaInstance = null;
			
				itemstate.messages = ["Starting Upload..."];
				this.axiosPostRequest ("/upload/", {
							"context": "build",
							"name": chosenfile.name,
							"size": chosenfile.size,
							"mimetype": "application/3mf",
							
						})
						
					.then(resultUploadInit => {
						var streamuuid = resultUploadInit.data.streamuuid;
						var contextuuid = resultUploadInit.data.contextuuid;
						
						const formData = new FormData();
						formData.append("size", chosenfile.size);					
						formData.append("data",  new Blob([fileContent], {type: "application/3mf"} ), chosenfile.name);					
						
						itemstate.messages = ["Uploading..."];
						
						this.axiosPostFormData ("/upload/" + streamuuid, formData)
						
						.then(resultUploadHandle => {
							resultUploadHandle;
							
							this.axiosPostRequest ("/upload/finish", {
									"streamuuid": streamuuid,						
									"sha256": sha256
								})
							.then(resultUploadFinish => {

								resultUploadFinish;
								
								itemstate.messages = ["Preparing build..."];
								
								this.axiosPostRequest ("/build/prepare", {
										"builduuid": contextuuid,						
									})
								.then(resultBuildPrepare => {
									resultBuildPrepare;
									itemstate.messages = [];
									itemstate.chosenFile = null;
									itemstate.uploadid = 0;
									
									if (successpage != "") {
										this.changePage (successpage + ":" + contextuuid);
										
									}
									
								})
								.catch(err => {
									err;                    
								});
							})
							.catch(err => {
								err;                    
							});
					
						})
						.catch(err => {
							err;                    
						});					
					
					})
					.catch(err => {
						err;                    
					});
					
				};
		
		}


        changePage(page) {
		
			var pageString = String (page);
			var colonIndex = pageString.search(":");
						
			if (colonIndex === -1) {
				this.AppState.activePage = pageString;
				this.AppState.activeObject = "00000000-0000-0000-0000-000000000000";
			}
			
			if (colonIndex > 0) {
				this.AppState.activePage = pageString.substring (0, colonIndex);
				this.AppState.activeObject = pageString.substring (colonIndex + 1);
			}
										
        }


		getImageURL (uuid) {
			return this.API.baseURL + '/ui/image/' + uuid;
		}
		
		
		triggerUIEvent (eventname, senderuuid, contextuuid, formvalues) {
			
            this.axiosPostRequest("/ui/event", {
				"eventname": eventname,
				"senderuuid": senderuuid,
				"contextuuid": contextuuid,
				"formvalues": formvalues
			})
				.then(resultHandleEvent => {
					resultHandleEvent;
					//alert (resultHandleEvent.data);
				})
                .catch(err => {
					alert (err);
                });				
		}

}



