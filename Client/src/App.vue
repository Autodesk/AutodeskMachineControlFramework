<template>
<v-app id="inspire">
    <v-navigation-drawer v-if="appIsReady" v-model="AppState.showDrawer" clipped :clipped-left="$vuetify.breakpoint.lgAndUp" disable-resize-watcher app>
        <v-list dense>
            <template v-for="item in AppDefinition.MenuItems">
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
        <v-btn tile large color="primary" dark v-on:click.stop="uiChangePage('main')">
            {{ uiButtonCaptionCheck(AppDefinition.TextApplicationName) }}
        </v-btn>

        <v-spacer />

        <template v-for="toolbaritem in AppDefinition.ToolbarItems">
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
var SHAJS = require('sha.js')

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
                    this.AppDefinition.MenuItems = resultJSON.data.menuitems;
                    this.AppDefinition.ToolbarItems = resultJSON.data.toolbaritems;
                    this.AppState.currentStatus = "login";
                })
                .catch(err => {
                    this.AppState.currentStatus = "error";
                    this.AppState.currentError = err.response.data.message;
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
					
					var saltedpassword = SHAJS("sha256").update(loginsalt + this.AppState.uiLoginPassword).digest("hex");
					var clientkeyhash = SHAJS("sha256").update(clientkey + saltedpassword).digest("hex");
					var sessionkeyhash = SHAJS("sha256").update(sessionkey + clientkeyhash).digest("hex");
														
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
						this.AppState.authToken = resultAuthenticate.data.token;
						
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

        uiToggleDrawer() {
            this.AppState.showDrawer = !this.AppState.showDrawer;
        },

        uiChangePage(page) {
            this.AppState.activePage = page;
        },

        uiOnTimer() {},

        updateTheme() {
            // Light theme
            // this.$vuetify.theme.themes.light.primary = '#4caf50'
            // Dark theme
            // this.$vuetify.theme.themes.dark.primary = '#4caf50'
        }
    },

    data: () => ({
        API: {
            baseURL: "/api"
        },

        AppDefinition: {
            TextApplicationName: "",
            TextCopyRight: "",
            MainPage: "",
            PageDefinitions: {},
            ToolbarItems: [],
            MenuItems: []
        },

        AppState: {
            currentStatus: "initial", // one of "initial" / "login" / "ready" / "error"
            currentError: "",
            showDrawer: true,
            activePage: "",
            globalTimer: "",
			authToken: "0000000000000000000000000000000000000000000000000000000000000000",
			
            uiLoginUser: "",
            uiLoginPassword: ""
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
