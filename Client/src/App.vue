<template>
<v-app id="inspire">
    <v-navigation-drawer v-if="appIsReady" v-model="ShowDrawer" clipped :clipped-left="$vuetify.breakpoint.lgAndUp" disable-resize-watcher app>
        <v-list dense>
            <template v-for="item in Application.AppContent.MenuItems">
                <v-list-group v-if="item.children" :key="item.caption" v-model="item.model" :prepend-icon="item.model ? item.icon : item['icon-alt']">
                    <template v-slot:activator>
                        <v-list-item-content>
                            <v-list-item-title v-on:click.stop="Application.changePage(item.targetpage)">
                                {{ item.caption }}
                            </v-list-item-title>
                        </v-list-item-content>
                    </template>

                    <v-list-item v-for="(child, i) in item.children" :key="i" link v-on:click.stop="Application.changePage(child.targetPage)">
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

                <v-list-item v-else :key="item.caption" link v-on:click.stop="Application.changePage(item.targetpage)">
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
        <v-btn tile large color="primary" dark v-on:click.stop="Application.changePage(Application.AppDefinition.MainPage)">
            {{ uiButtonCaptionCheck(Application.AppDefinition.TextApplicationName) }}
        </v-btn>

        <v-spacer />

        <template v-for="toolbaritem in Application.AppContent.ToolbarItems">
            <v-btn :key="toolbaritem.id" color="primary" large v-on:click.stop="Application.changePage(toolbaritem.targetpage)">
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
                            <v-toolbar-title>{{ Application.AppDefinition.TextApplicationName }}</v-toolbar-title>
                        </v-toolbar>
                        <v-card-text>
							<v-img v-if="Application.AppDefinition.LogoUUID != ''" v-bind:src="getImageURL (Application.AppDefinition.LogoUUID)" v-bind:aspect-ratio="Application.AppDefinition.LogoAspectRatio" contain></v-img>
						
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
			
				<template v-for="uiPage in Application.AppContent.Pages">

					<v-row align="start" justify="center" :key="uiPage.name" v-if="(Application.AppState.activePage == uiPage.name)">
					
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
											
											<div :key="item.uuid" v-if="(item.type=='paragraph')">{{ item.text }}</div>		
											<div :key="item.uuid" v-if="(item.type=='image')"><v-img v-bind:src="getImageURL (item.uuid)" v-bind:aspect-ratio="item.aspectratio" v-bind:max-width="item.maxwidth" v-bind:max-height="item.maxheight" contain></v-img></div>
											
											<div :key="item.uuid" v-if="(item.type=='upload')">
																						
												<v-file-input accept=".3mf" show-size full-width v-model="item.state.chosenFile" v-bind:label="item.uploadcaption" v-bind:messages="item.state.messages" @change="uiUploadStart (item)"></v-file-input>
																
											</div>

											<div :key="item.uuid" v-if="(item.type=='layerview')">
																														
												<LayerView />
								
											</div>
											
											<div :key="item.uuid" v-if="(item.type=='parameterlist')">											
												<v-data-table
													:headers="item.headers"
													:items="item.entries"
													:items-per-page="item.entriesperpage"
													class="elevation-1"
													search 
													disable-pagination
													hide-default-footer
													width="100%"
													loadingText="item.loadingtext">
												</v-data-table>											
											</div>											

											<div :key="item.uuid" v-if="(item.type=='buildlist')">											
												<v-data-table
													:headers="item.headers"
													:items="item.entries"
													:items-per-page="item.entriesperpage"
													class="elevation-1"
													disable-pagination
													hide-default-footer
													search 
													width="100%"
													loadingText="item.loadingtext"
													@click:row="uiModuleBuildListClick"
													>													
																					
												</v-data-table>					
												
											</div>											
											
											<div :key="item.uuid" v-if="(item.type=='buttongroup')">
												<v-spacer></v-spacer>
												
												<template v-for="button in item.buttons">
												
													<v-btn color="primary" :key="button.name" v-on:click.stop="uiModuleButtonClick (button);">
														{{ button.caption }}
													</v-btn>
												
												</template>
											</div>											
											
										</template>
											
									</div>
									
																		
								</v-card-text>
								
								
							
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
							
							{{ Application.AppState.currentError }}
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
        <span class="caption white--text" v-if="appHasInformation">&copy; {{ Application.AppDefinition.TextCopyRight }}</span>		
    </v-footer>
</v-app>
</template>

<script>

import * as Axios from "axios";

import LayerView from './LayerView.vue';
import AMCApplication from './Application.js'


export default {
    props: {
    },

    created() {
        this.GlobalTimer = setInterval(this.uiOnTimer, 600);
		
		var baseURL = "/api";
        if (process.env.NODE_ENV === "development") {
            baseURL = "http://localhost:8869/api";
        }
		
		this.Application = new AMCApplication (baseURL);
        this.Application.retrieveConfiguration();
    },
	

    beforeDestroy() {
        if (this.GlobalTimer) {
            clearInterval(this.GlobalTimer);
        }
    },

    computed: {
        appIsLoading() {
            return this.Application.AppState.currentStatus === "initial";
        },

        appIsLogin() {
            return this.Application.AppState.currentStatus === "login";
        },

        appIsReady() {
            return this.Application.AppState.currentStatus === "ready";
        },

        appIsError() {
            return this.Application.AppState.currentStatus === "error";
        },

		appHasInformation() {
			return (this.Application.AppState.currentStatus === "login") || (this.Application.AppState.currentStatus === "ready");
		} 
    },
	
	components: {
		LayerView
	},	

    methods: {
        

        uiLogInClick() {
		
			this.Application.requestLogin (this.AppState.uiLoginUser, this.AppState.uiLoginPassword);			
					
        },

        uiReloadPageClick() {
            this.setStatus ("initial");
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
				
				this.appPerformJobUpload (item.uuid, item.state, item.state.uploadid, item.state.chosenFile, item.uploadsuccesspage);
				
			} else {
			
				if (!(item.state.uploadid === 0)) {
					item.state.uploadid = 0; 
					item.state.messages = ["Canceled upload..."];
				}
			
			}
			
		
		},

        uiToggleDrawer() {
            this.ShowDrawer = !this.ShowDrawer;
        },

		
		
		uiTriggerUIEvent (eventname, senderuuid, contextuuid) {
			var url = this.Application.API.baseURL + "/ui/event";
			
            Axios({			
                    method: "POST",
                    url: url,
					headers: {
						"Authorization": "Bearer " + this.Application.API.authToken,
					},
					data: {
						"eventname": eventname,
						"senderuuid": senderuuid,
						"contextuuid": contextuuid						
					}
                })
                .then(resultHandleEvent => {
					resultHandleEvent;
					//alert (resultHandleEvent.data);
				})
                .catch(err => {
					alert (err);
                });				
		},
		
		uiModuleButtonClick (button) {
			
			var contextuuid = this.Application.AppState.activeObject;
			
			if (button.event != "") {		
				this.uiTriggerUIEvent (button.event, button.uuid, contextuuid);
			}
			
			if (button.targetpage != "") {
				this.Application.changePage (button.targetpage);
			}
		
		},
		
		
		uiModuleBuildListClick (item) {
			if (item.detailpage != "") {
				this.Application.changePage (String (item.detailpage) + ":" + String (item.buildUUID));
			}
		},
			
				
        uiOnTimer() {
			if (this.Application) {
				this.Application.updateContentItems ();			
			}													
		},
		

        updateTheme() {
            // Light theme
            // this.$vuetify.theme.themes.light.primary = '#4caf50'
            // Dark theme
            // this.$vuetify.theme.themes.dark.primary = '#4caf50'
        },
		
		getImageURL (uuid) {
			return this.Application.API.baseURL + '/ui/image/' + uuid;
		}
		
    },

    data: () => ({
	
		Application: null,
        GlobalTimer: null,
        ShowDrawer: true,
	

        AppState: {
		
            uiLoginUser: "test",
            uiLoginPassword: "test",					
			
        }
    })
};
</script>

