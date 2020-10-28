
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
			headers = { "Authorization": "Bearer " + authToken };		
		
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
			headers = { "Authorization": "Bearer " + authToken };
		
		return Axios ({
                    "method": "POST",
                    "url": this.API.baseURL + subURL,
					"headers": headers,
					"data": data
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
				
				//this.uiChangePage (this.AppDefinition.MainPage);
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
	
	
	retrieveStateUpdate () {
		
		this.axiosGetRequest ("/ui/state")		
		
                .then(resultJSON => {
                    this.AppContent.MenuItems = resultJSON.data.menuitems;
                    this.AppContent.ToolbarItems = resultJSON.data.toolbaritems;
					
					var page, module, item;
					for (page of resultJSON.data.pages) {
						for (module of page.modules) {
							if (module.type === "content") {
								for (item of module.items) {
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
								
							}
						}						
					
					}
					
					this.AppContent.Pages = resultJSON.data.pages;
                   
                })
                .catch(err => {
                    this.setStatusToError (err.response.data.message);
                });
        }	

}



