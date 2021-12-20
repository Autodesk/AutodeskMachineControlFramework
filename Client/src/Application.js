
import * as Axios from "axios";
import * as asmCrypto from "asmcrypto-lite";


const nullUUID = "00000000-0000-0000-0000-000000000000";
const nullToken = "0000000000000000000000000000000000000000000000000000000000000000";


	
function updateAMCFormEntityFromContentEntry (entry, entity) {
	
	if ((entity) && (entry)) {
		
		if (entity.remotevalue != entry.value) {
			entity.value = entry.value;				
		}		
		entity.remotevalue = entry.value;
		entity.prefix = entry.prefix;
		entity.suffix = entry.suffix;
		entity.readonly = entry.readonly;
		entity.disabled = entry.disabled;
		

	}
			
}

export default class AMCApplication {
	
	constructor (apiBaseURL)
	{
		this.API = {
			baseURL : apiBaseURL,
			authToken: nullToken,
			unsuccessfulUpdateCounter: 0
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
			LogoAspectRatio: 1.0,
			Colors: {}
        }
		
		this.AppContent = {
            MenuItems: [],
            ToolbarItems: [],
			ContentItems: [],					
			Pages: [],
			Dialogs: [],
			PageMap: new Map(),
			DialogMap: new Map(),
			FormEntities: []
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
		this.closeAllDialogs ();
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

				
	retrieveConfiguration (vuetifythemes) 
	{
		this.axiosGetRequest ("/ui/config")
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
				this.setStatus ("login");
									
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
				
				
				this.changePage (this.AppDefinition.MainPage);
			})
			.catch(err => {
				this.setStatusToError (err.response.data.message);
			});
	}			


	performLogout ()
	{
		this.authToken = nullToken;		
		this.unsuccessfulUpdateCounter = 0;
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
		
		if (item.type === "form") {

			this.AppContent.ContentItems[item.uuid] = { uuid: item.uuid, entries: [], refresh: true, 
				callback: function(appcontent, item) {
					
					for (var entry of item.entries) {						
						updateAMCFormEntityFromContentEntry (entry, appcontent.FormEntities[entry.uuid]);																	
					}
				}
			};
			
			for (var entity of item.entities) {
				
				this.AppContent.FormEntities[entity.uuid] = 
					{ uuid: entity.uuid, 
					  value: entity.value, 
					  remotevalue: entity.value, 
					  disabled: entity.disabled, 
					  readonly: entity.readonly
					  };
					  
					  
				entity.dataObject = this.AppContent.FormEntities[entity.uuid];
							
			}
			
		
			
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

		if (module.type === "grid") {
						
						
			module.cssstyle = "display: grid; width:100%; height:100%;";
			
			var columnString = "";
			var rowString = "";
			var areaString = "";
			
			if (module.columns) {
				if (module.rows) {
					var columnCount = module.columns.length;
					var rowCount = module.rows.length;
			
					var row, column;
					var gridMap = new Array(rowCount);
					for (row = 0; row < rowCount; row++) {
						gridMap[row] = new Array(columnCount);
						
						for (column = 0; column < columnCount; column++) {
							var templatename = "_grid_" + module.name + "_" + column + "_" + row;
							gridMap[row][column] = templatename;							
						}
					} 
					
					for (section of module.sections) {
						
						var columnstart = section.columnstart;
						var columnend = section.columnend;
						var rowstart = section.rowstart;
						var rowend = section.rowend;						
												
						if ((columnstart <= columnend) && (rowstart <= rowend) && (columnstart > 0) && (rowstart > 0) &&
						   (columnend <= columnCount) && (rowend <= rowCount)) {
						
							for (row = rowstart - 1; row < rowend; row++) {						
								for (column = columnstart - 1; column < columnend; column++) {
									gridMap[row][column] = section.name;
								}
							}
						}
					}
					
					for (row = 0; row < rowCount; row++) {
						var rowObject = module.rows[row];
											
						if (rowObject.unit === "px") {
							rowString = rowString + rowObject.height + "px ";
						} else if (rowObject.unit === "free") {
							rowString = rowString + rowObject.height + "fr ";
						} else if (rowObject.unit === "pt") {
							rowString = rowString + rowObject.height + "pt ";
						} else {
							rowString = rowString + "auto ";
						}
					}

					for (column = 0; column < columnCount; column++) {
						var columnObject = module.columns[column];
											
						if (columnObject.unit === "px") {
							columnString = columnString + columnObject.width + "px ";
						} else if (columnObject.unit === "free") {
							columnString = columnString + columnObject.width + "fr ";
						} else if (columnObject.unit === "pt") {
							columnString = columnString + columnObject.width + "pt ";
						} else {
							columnString = columnString + "auto ";
						}
							
					}
					
					for (row = 0; row < rowCount; row++) {						
						areaString = areaString + "\"";
						for (column = 0; column < columnCount; column++) {
							if (column > 0) {
								areaString = areaString + " ";
							}
							areaString = areaString + gridMap[row][column];
						}
						areaString = areaString + "\" ";
					}
					
			
				}
			} 		
									
			module.cssstyle = module.cssstyle + "grid-template-columns: " + columnString + ";";
			module.cssstyle = module.cssstyle + "grid-template-rows: "+ rowString + ";";			
			module.cssstyle = module.cssstyle + "grid-template-areas: " + areaString;
						
			for (section of module.sections) {
								
				var marginx = "margin-left: 0; margin-right: 0;";
				var marginy = "margin-top: 0; margin-bottom: 0;";
				
				if (section.columnposition) {
					if (section.columnposition == "centered")
						marginx = "margin-left: auto; margin-right: auto;";
					if (section.columnposition == "right")
						marginx = "margin-left: auto; margin-right: 0;"
				}

				if (section.rowposition) {
					if (section.rowposition == "centered")
						marginy = "margin-top: auto; margin-bottom: auto;";
					if (section.rowposition == "bottom")
						marginy = "margin-top: auto; margin-bottom: 0;";
				} 
				
				section.cssstyle = "overflow:auto; grid-area:" + section.name + ";" + marginx + marginy;				
												
				this.prepareModule (section)
			}
		}

	}

	
	retrieveStateUpdate () {
		
		this.axiosGetRequest ("/ui/state")		
		
                .then(resultJSON => {
                    this.AppContent.MenuItems = resultJSON.data.menuitems;
                    this.AppContent.ToolbarItems = resultJSON.data.toolbaritems;
					
					var page, dialog, module;
					for (page of resultJSON.data.pages) {						
						for (module of page.modules) {
							this.prepareModule (module)
						}											
						
						this.AppContent.PageMap.set (page.name, page);
						
					}

					for (dialog of resultJSON.data.dialogs) {
						for (module of dialog.modules) {
							this.prepareModule (module)
						}											

						dialog.dialogIsActive = false;
						this.AppContent.DialogMap.set (dialog.name, dialog);
					}

					
					this.AppContent.Pages = resultJSON.data.pages;
					this.AppContent.Dialogs = resultJSON.data.dialogs;					
                   
                })
                .catch(err => {
                    this.setStatusToError (err.response.data.message);
                });
        }	


		updateContentItem (uuid) {
		
			this.AppContent.ContentItems[uuid].refresh = false;

			var headers = {}
			var authToken = this.API.authToken; 
		
			if (authToken != nullToken)
				headers.Authorization = "Bearer " + authToken;
		
            var url = this.API.baseURL + "/ui/contentitem/" + uuid;
            Axios({
                    method: "GET",
					"headers": headers,
                    url: url
                })
                .then(resultJSON => {					
				
					this.unsuccessfulUpdateCounter = 0;

					var oldentrycount = this.AppContent.ContentItems[uuid].entries.length;					
					for (var i = 0; i < oldentrycount; i++) {
						this.AppContent.ContentItems[uuid].entries.pop ();
					}
					
					for (var entry of resultJSON.data.content.entries) {
						this.AppContent.ContentItems[uuid].entries.push (entry);
					}
					this.AppContent.ContentItems[uuid].refresh = true;
					
					var itemCallback = this.AppContent.ContentItems[uuid].callback;
					if (itemCallback) {						
						itemCallback (this.AppContent, this.AppContent.ContentItems[uuid]);
					}
					
                })
                .catch(err => {
					
					this.unsuccessfulUpdateCounter = this.unsuccessfulUpdateCounter + 1;
					if (this.unsuccessfulUpdateCounter > 5) {
						this.setStatusToError (err.message);
					} else {
						this.AppContent.ContentItems[uuid].refresh = true;
					}
					
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
		
		closeAllDialogs () {
			var	dialog;
			for (dialog of this.AppContent.Dialogs) {
				dialog.dialogIsActive = false;
			}
		}
		
		showDialog (dialog) {
			
			this.closeAllDialogs ();
			
			if (dialog) {
							
				if (this.AppContent.DialogMap.has (dialog)) {
					var dialogObject = this.AppContent.DialogMap.get (dialog);
					dialogObject.dialogIsActive = true;
				}
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
					if (resultHandleEvent.data.pagetoactivate) {
						this.changePage (resultHandleEvent.data.pagetoactivate);
					}
					if (resultHandleEvent.data.dialogtoshow) {
						this.showDialog (resultHandleEvent.data.dialogtoshow);
					}
					if (resultHandleEvent.data.closedialogs) {
						this.closeAllDialogs ();
					}
					
					
				})
                .catch(err => {
					alert (err);
                });				
		}
		
		
		assembleFormValues (formValueUUIDList)
		{
			var resultObject = {}
			
			for (var entityuuid of formValueUUIDList) {
				var formValue = this.AppContent.FormEntities[entityuuid].value;
				resultObject[entityuuid] = formValue;				
			}
			
			return resultObject;
			
		}

}



