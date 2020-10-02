<template>
<v-app id="inspire">
    <v-navigation-drawer v-if="appIsReady" v-model="AppState.showDrawer" clipped :clipped-left="$vuetify.breakpoint.lgAndUp" disable-resize-watcher app>
        <v-list dense>
            <template v-for="item in AppState.MenuItems">
                <v-list-group v-if="item.children" :key="item.caption" v-model="item.model" :prepend-icon="item.model ? item.icon : item['icon-alt']">
                    <template v-slot:activator>
                        <v-list-item-content>
                            <v-list-item-title v-on:click.stop="uiChangePage(item.targetpage)">
                                {{ item.caption }}
                            </v-list-item-title>
                        </v-list-item-content>
                    </template>

                    <v-list-item v-for="(child, i) in item.children" :key="i" link v-on:click.stop="uiChangePage(child.targetPage)">
                        <v-list-item-action v-if="child.icon">
                            <v-icon>{{ child.icon }}</v-icon>
                        </v-list-item-action>
                        <v-list-item-content>
                            <v-list-item-title>
                                {{ child.text }}
                            </v-list-item-title>
                        </v-list-item-content>
                    </v-list-item>
                </v-list-group>

                <v-list-item v-else :key="item.caption" link v-on:click.stop="uiChangePage(item.targetpage)">
                    <v-list-item-action>
                        <v-icon>{{ item.icon }}</v-icon>
                    </v-list-item-action>
                    <v-list-item-content>
                        <v-list-item-title>
                            {{ item.caption }}
                        </v-list-item-title>
                    </v-list-item-content>
                </v-list-item>
            </template>
        </v-list>
    </v-navigation-drawer>

    <v-app-bar app color="primary" dark v-if="appIsReady" :clipped-left="$vuetify.breakpoint.lgAndUp">
        <v-app-bar-nav-icon v-on:click.stop="uiToggleDrawer" />
        <v-btn tile large color="primary" dark v-on:click.stop="uiChangePage(AppDefinition.MainPage)">
            {{ uiButtonCaptionCheck(AppDefinition.TextApplicationName) }}
        </v-btn>

        <v-spacer />

        <template v-for="toolbaritem in AppState.ToolbarItems">
            <v-btn :key="toolbaritem.id" color="primary" large v-on:click.stop="uiChangePage(toolbaritem.targetpage)">
                <v-icon left>{{ toolbaritem.icon }}</v-icon>{{ uiButtonCaptionCheck(toolbaritem.caption) }}
            </v-btn>
        </template>
    </v-app-bar>

    <v-main>
        <v-container class="fill-height" fluid v-if="appIsLoading">
            <v-row align="center" justify="center">
                <v-progress-circular :value="20" indeterminate></v-progress-circular>
            </v-row>
        </v-container>

        <v-container class="fill-height" fluid v-if="appIsLogin">
            <v-row align="center" justify="center">
                <v-col cols="12" sm="8" md="4">
                    <v-card class="elevation-12">
                        <v-toolbar color="primary" dark flat>
                            <v-toolbar-title>{{ AppDefinition.TextApplicationName }}</v-toolbar-title>
                        </v-toolbar>
                        <v-card-text>
							<v-img v-if="AppDefinition.LogoUUID != ''" v-bind:src="getImageURL (AppDefinition.LogoUUID)" v-bind:aspect-ratio="AppDefinition.LogoAspectRatio" contain></v-img>
						
                            <v-form>
                                <v-text-field label="User" name="login" prepend-icon="mdi-account" type="text" v-model="AppState.uiLoginUser" autofocus clearable />
                                <v-text-field id="password" label="Password" name="password" prepend-icon="mdi-lock" type="password" clearable v-model="AppState.uiLoginPassword" />
                            </v-form>
                        </v-card-text>
                        <v-card-actions>
                            <v-spacer />
                            <v-btn color="primary" v-on:click="uiLogInClick">
                                <v-icon left>mdi-login</v-icon>Login
                            </v-btn>
                        </v-card-actions>
                    </v-card>
                </v-col>
            </v-row>
        </v-container>

        <v-container fluid v-if="appIsReady">
			
				<template v-for="uiPage in AppState.uiPages">

					<v-row align="start" justify="center" :key="uiPage.name" v-if="(AppState.activePage == uiPage.name)">
					
						<template v-for="uiModule in uiPage.modules">
																
							<v-card :key="uiModule.name" width="99%" v-if="(uiModule.type == 'content')">
								<v-card-text>
									<div v-if="uiModule.title != ''"> {{ uiModule.headline }}</div>
									<p v-if="uiModule.title != ''" class="display-1 text--primary">
										{{ uiModule.title }}
									</p>
									<p v-if="uiModule.subtitle != ''" >{{ uiModule.subtitle }}</p>
								
									<div v-if="uiModule.items.length > 0" class="text--primary">
									
										<template v-for="item in uiModule.items">
											
											<p :key="item.uuid" v-if="(item.type=='paragraph')">{{ item.text }}</p>												
											<p :key="item.uuid" v-if="(item.type=='image')"><v-img v-bind:src="getImageURL (item.uuid)" v-bind:aspect-ratio="item.aspectratio" contain></v-img></p>
											
											<div :key="item.uuid" v-if="(item.type=='upload')">
																						
												<v-file-input accept=".3mf" show-size full-width v-model="item.state.chosenFile" v-bind:label="item.uploadcaption" v-bind:messages="item.state.messages" @change="uiUploadStart (item)"></v-file-input>
								
											</div>
											
											<div :key="item.uuid" v-if="(item.type=='parameterlist')">											
												<v-data-table
													:headers="item.headers"
													:items="item.entries"
													:items-per-page="item.entriesperpage"
													class="elevation-1"
													disable-pagination
													hide-default-footer
													width="100%"
													loadingText="item.loadingtext">
												</v-data-table>											
											</div>											
											
										</template>
											
									</div>
								</v-card-text>
							
								<v-card-actions v-if="(uiModule.buttons.length > 0)">
									<v-spacer></v-spacer>
									
									<template v-for="button in uiModule.buttons">
									
										<v-btn color="primary" :key="button.name" v-on:click.stop="uiModuleButtonClick (button);">
											{{ button.caption }}
										</v-btn>
									
									</template>
								</v-card-actions>
							</v-card>
					


							<v-card :key="uiModule.name" width="99%" v-if="(uiModule.type == 'buildlist')">
								  								  
								<v-data-table
									:headers="uiModule.headers"
									:items="uiModule.items"
									:items-per-page="15"
									class="elevation-1"
									disable-pagination
									hide-default-footer
									width="100%"
									loadingText="uiModule.loadingtext">

								</v-data-table>																  
																  
							</v-card>
							
						</template>
					
					</v-row>
				</template>
								
        </v-container>


        <v-container class="fill-height" fluid v-if="appIsError">
            <v-row align="center" justify="center">
                <v-col cols="12" sm="8" md="4">
                    <v-card class="elevation-12">
                        <v-toolbar color="primary" dark flat>
                            <v-toolbar-title>Fatal error</v-toolbar-title>
                        </v-toolbar>
                        <v-card-text>
                            Could not connect to server. Please try again later!
							
							{{ AppState.currentError }}
                        </v-card-text>
                        <v-card-actions>
                            <v-spacer />
                            <v-btn color="primary" v-on:click="uiReloadPageClick">
                                <v-icon left>mdi-refresh</v-icon>Reload page
                            </v-btn>
                        </v-card-actions>
                    </v-card>
                </v-col>
            </v-row>
        </v-container>

        <v-container fluid v-if="appIsReady"> </v-container>
    </v-main>

    <v-footer color="primary" class="text-right" min-height="30" app >
        <v-spacer />
        <span class="caption white--text" v-if="appHasInformation">&copy; {{ AppDefinition.TextCopyRight }}</span>		
    </v-footer>
</v-app>
</template>

<script>
import * as Axios from "axios";
import * as asmCrypto from "asmcrypto-lite";

export default {
    props: {
        source: String
    },

    created() {
        this.AppState.globalTimer = setInterval(this.uiOnTimer, 600);

        if (process.env.NODE_ENV == "development") {
            this.API.baseURL = "http://localhost:8869/api";
        }

        this.appRetrieveConfiguration();
    },

    beforeDestroy() {
        if (this.AppState.globalTimer != "") {
            clearInterval(this.AppState.globalTimer);
        }
    },

    computed: {
        appIsLoading() {
            return this.AppState.currentStatus === "initial";
        },

        appIsLogin() {
            return this.AppState.currentStatus === "login";
        },

        appIsReady() {
            return this.AppState.currentStatus === "ready";
        },

        appIsError() {
            return this.AppState.currentStatus === "error";
        },

		appHasInformation() {
			return (this.AppState.currentStatus === "login") || (this.AppState.currentStatus === "ready");
		} 
    },

    methods: {
        appRetrieveConfiguration() {
            var url = this.API.baseURL + "/ui/config";
            Axios({
                    method: "GET",
                    url: url
                })
                .then(resultJSON => {
                    this.AppDefinition.TextApplicationName = resultJSON.data.appname;
                    this.AppDefinition.TextCopyRight = resultJSON.data.copyright;
                    this.AppDefinition.MainPage = resultJSON.data.mainpage;
                    this.AppDefinition.LogoUUID = resultJSON.data.logouuid;
                    this.AppDefinition.LogoAspectRatio = resultJSON.data.logoaspectratio;
					this.AppState.currentStatus = "login";
					
					document.title = this.AppDefinition.TextApplicationName;
                })
                .catch(err => {
                    this.AppState.currentStatus = "error";
                    this.AppState.currentError = err.response.data.message;
                });
        },
		
        appRetrieveStateUpdate () {
            var url = this.API.baseURL + "/ui/state";
            Axios({
                    method: "GET",
                    url: url,
					headers: {
						"Authorization": "Bearer " + this.API.authToken,
					},
                })
                .then(resultJSON => {
                    this.AppState.MenuItems = resultJSON.data.menuitems;
                    this.AppState.ToolbarItems = resultJSON.data.toolbaritems;
					
					var page, module, item;
					for (page of resultJSON.data.pages) {
						for (module of page.modules) {
							if (module.type === "content") {
								for (item of module.items) {
									if (item.type === "parameterlist") {
									
										this.AppState.ContentItems[item.uuid] = { uuid: item.uuid, entries: [], refresh: true };
										item.entries = this.AppState.ContentItems[item.uuid].entries;
										
									}
									
									if (item.type === "upload") {
										item.state = { uploadid: 0, chosenFile: null, idcounter: 0, messages: [] }
									
									}
								}
								
							}
						}						
					
					}
					
					this.AppState.uiPages = resultJSON.data.pages;
					
					
                    this.AppState.activePage = this.AppDefinition.MainPage;
                })
                .catch(err => {
                    this.AppState.currentStatus = "error";
                    this.AppState.currentError = err.response.data.message;
                });
        },
		
		
		appPerformJobUpload (itemuuid, itemstate, uploadid, chosenfile) {
					
		
			// Attention: itemstate might change with UI interaction. Always check if uploadid matches!
			var url = this.API.baseURL + "/upload/";
			var prepareurl = this.API.baseURL + "/build/prepare/";
						
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
				Axios({			
						method: "POST",
						url: url,
						headers: {
							"Authorization": "Bearer " + this.API.authToken,
						},
						data: {
							"context": "build",
							"name": chosenfile.name,
							"size": chosenfile.size,
							"mimetype": "application/3mf",
							
						}
					})
					.then(resultUploadInit => {
						var streamuuid = resultUploadInit.data.streamuuid;
						var contextuuid = resultUploadInit.data.contextuuid;
						
						const formData = new FormData();
						formData.append("size", chosenfile.size);					
						formData.append("data",  new Blob([fileContent], {type: "application/3mf"} ), chosenfile.name);					
						
						itemstate.messages = ["Uploading..."];
						
						Axios({			
							method: "POST",
							url: url + streamuuid,
							headers: {
								"Authorization": "Bearer " + this.API.authToken,
								"Content-Type": "multipart/form-data"
							},
							data: formData
							
						})
						.then(resultUploadHandle => {
							resultUploadHandle;
							
							Axios({			
								method: "POST",
								url: url + "finish",
								headers: {
									"Authorization": "Bearer " + this.API.authToken,
								},
								data: {
									"streamuuid": streamuuid,						
									"sha256": sha256
								}
							})
							.then(resultUploadFinish => {

								resultUploadFinish;
								
								itemstate.messages = ["Preparing build..."];
								
								Axios({			
									method: "POST",
									url: prepareurl,
									headers: {
										"Authorization": "Bearer " + this.API.authToken,
									},
									data: {
										"builduuid": contextuuid,						
									}
								})
								.then(resultBuildPrepare => {
									alert ("uploaded " + resultBuildPrepare.data.name);
									itemstate.messages = [];
									itemstate.chosenFile = null;
									itemstate.uploadid = 0;
									
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
		
		},

		appUpdateContentItem (uuid) {
		
			this.AppState.ContentItems[uuid].refresh = false;
		
            var url = this.API.baseURL + "/ui/contentitem/" + uuid;
            Axios({
                    method: "GET",
                    url: url
                })
                .then(resultJSON => {					

					var oldentrycount = this.AppState.ContentItems[uuid].entries.length;					
					for (var i = 0; i < oldentrycount; i++) {
						this.AppState.ContentItems[uuid].entries.pop ();
					}
					
					for (var entry of resultJSON.data.content.entries) {
						this.AppState.ContentItems[uuid].entries.push (entry);
					}
					this.AppState.ContentItems[uuid].refresh = true;
                })
                .catch(err => {
					err;
                    this.AppState.ContentItems[uuid].refresh = true;                    
                });
				
		},

        uiLogInClick() {
		
			var url = this.API.baseURL + "/auth/";
			
            Axios({			
                    method: "POST",
                    url: url,
					data: {
						username: this.AppState.uiLoginUser
					}
                })
                .then(resultCreateSession => {
																
					var sessionuuid = resultCreateSession.data.sessionuuid;
					var sessionkey = resultCreateSession.data.sessionkey;
					var loginsalt = resultCreateSession.data.loginsalt;
					var clientkey = sessionkey;
					
					var saltedpassword = asmCrypto.SHA256.hex (loginsalt + this.AppState.uiLoginPassword);
					var clientkeyhash = asmCrypto.SHA256.hex (clientkey + saltedpassword);
					var sessionkeyhash = asmCrypto.SHA256.hex (sessionkey + clientkeyhash);
														
					Axios({
						method: "POST",
						url: url + sessionuuid,
						data: {
							"clientkey": clientkey,
							"password": sessionkeyhash
						}
					})
					.then(resultAuthenticate => {
					
						this.AppState.uiLoginPassword = "";
						this.AppState.currentStatus = "ready";
						
						this.API.authToken = resultAuthenticate.data.token;
						
						this.appRetrieveStateUpdate ();
						
					})
					.catch(err => {
						this.AppState.currentStatus = "error";
						this.AppState.currentError = err.response.data.message;
					}); 
									  
					
                })
                .catch(err => {
                    this.AppState.currentStatus = "error";
                    this.AppState.currentError = err.response.data.message;
                });
		
        },

        uiReloadPageClick() {
            this.AppState.currentStatus = "initial";
            this.appRetrieveConfiguration();
        },

        uiButtonCaptionCheck(caption) {
            var size = this.$vuetify.breakpoint.name;
            if (size == "xs" || size == "sm") return "";

            return caption;
        },
		
		uiUploadStart (item) {
			item.state.idcounter = item.state.idcounter + 1;
			if (item.state.chosenFile) {
			
				if (item.state.uploadid === 0) {
					item.state.messages = ["Uploading..."];					
			
				} else {
					item.state.messages = ["Replacing upload..."];
				}
				item.state.uploadid = item.state.idcounter; 
				
				this.appPerformJobUpload (item.uuid, item.state, item.state.uploadid, item.state.chosenFile);
				
			} else {
			
				if (!(item.state.uploadid === 0)) {
					item.state.uploadid = 0; 
					item.state.messages = ["Canceled upload..."];
				}
			
			}
			
		
		},

        uiToggleDrawer() {
            this.AppState.showDrawer = !this.AppState.showDrawer;
        },

        uiChangePage(page) {
            this.AppState.activePage = page;				
        },
		
		uiModuleButtonClick (button) {
			if (button.targetpage != "") {
				this.uiChangePage (button.targetpage);
			}
		
		},

        uiOnTimer() {
		
			for (var key in this.AppState.ContentItems) {
				var item = this.AppState.ContentItems [key];
				if (item.refresh) {				
					this.appUpdateContentItem (item.uuid);
				}
			}												
		
		},

        updateTheme() {
            // Light theme
            // this.$vuetify.theme.themes.light.primary = '#4caf50'
            // Dark theme
            // this.$vuetify.theme.themes.dark.primary = '#4caf50'
        },
		
		getImageURL (uuid) {
			return this.API.baseURL + '/ui/image/' + uuid;
		}
		
    },

    data: () => ({
        API: {
            baseURL: "/api",
			authToken: "0000000000000000000000000000000000000000000000000000000000000000"
        },

        AppDefinition: {
            TextApplicationName: "",
            TextCopyRight: "",
            MainPage: "",
			LogoUUID: "",
			LogoAspectRatio: 1.0
        },

        AppState: {
		
            MenuItems: [],
            ToolbarItems: [],
			ContentItems: [],
					
            currentStatus: "initial", // one of "initial" / "login" / "ready" / "error"
            currentError: "",
            showDrawer: true,
            activePage: "",
            globalTimer: "",			
			uiPages: [],
			
            uiLoginUser: "test",
            uiLoginPassword: "test",					
			
        }
    })
};
</script>

<style lang="scss">
.dropbox {
    outline: 2px dashed grey;
    /* the dash box */
    outline-offset: -10px;
    color: dimgray;
    padding: auto;
    min-height: 100px;
    /* minimum height */
    position: relative;
    cursor: pointer;
}

.input-file {
    opacity: 0;
    /* invisible but it's there! */
    width: 100%;
    height: 100px;
    position: absolute;
    cursor: pointer;
}

.dropbox:hover {
    background: #f0f0f0;
    /* when mouse over to the drop zone, change color */
}

.dropbox p {
    font-size: 1.2em;
    text-align: center;
    padding: 50px 0;
}
</style>
