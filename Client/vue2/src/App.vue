<!--

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

!-->


<template>
<v-app id="AMCApp" app>
    <v-navigation-drawer v-if="appIsReady" v-model="ShowDrawer" clipped :clipped-left="$vuetify.breakpoint.lgAndUp" disable-resize-watcher app>
        <v-list three-line dense>
            <template v-for="(item, index) in Application.AppContent.MenuItems">                
									  
						  <v-list-item :key="item.uuid" v-on:click="uiMenuClicked(item)"
						  >
							<v-list-item-avatar >
							  <v-icon>{{ item.icon }}</v-icon>
							</v-list-item-avatar>
				  
							<v-list-item-content>
							  <v-list-item-title>{{ item.caption }}</v-list-item-title>
							  <v-list-item-subtitle>{{ item.description }}</v-list-item-subtitle>
							</v-list-item-content>
						  </v-list-item>
						  						  
						  <v-divider
							:key="index"
						  ></v-divider>
            </template>
        </v-list>
    </v-navigation-drawer>

    <v-app-bar app color="#3C3C3C" dark v-if="appIsReady" :clipped-left="$vuetify.breakpoint.lgAndUp">
        <v-app-bar-nav-icon v-on:click.stop="uiToggleDrawer" />		
        <v-btn text large dark v-on:click.stop="Application.changePage(Application.AppDefinition.MainPage)">
            <v-img v-if="Application.AppDefinition.ToolbarLogoUUID != ''" width="150" v-bind:src="Application.getImageURL (Application.AppDefinition.ToolbarLogoUUID)"></v-img>
			<p v-else>{{ uiButtonCaptionCheck(Application.AppDefinition.TextApplicationName) }}</p>
        </v-btn>

        <v-spacer />

        <template v-for="toolbaritem in Application.AppContent.ToolbarItems">
            <v-btn :key="toolbaritem.id" text large v-on:click.stop="uiToolbarClicked (toolbaritem)">
                <v-icon left>{{ toolbaritem.icon }}</v-icon>{{ uiButtonCaptionCheck(toolbaritem.caption) }}
            </v-btn>
        </template>
    </v-app-bar>

	<v-main app v-bind:style="appMainStyle" v-resize="uiUpdateAppSize" class="amc-main">
        <v-container class="fill-height" fluid v-if="appIsLoading">
            <v-row align="center" justify="center">
                <v-progress-circular :value="20" indeterminate></v-progress-circular>
            </v-row>
        </v-container>

        <v-container class="fill-height" fluid v-if="appIsLogin">
			<Dialog_Login :Application="Application" />
        </v-container>
		
		<v-container class="fill-height" fluid v-if="appIsError">
			<Dialog_Error :Application="Application" />
        </v-container>		

		<div class="amc-content-wrap" v-if="appIsReady">
			<v-card class="amc-content-card" elevation="2" width="100%" > 
				<template v-for="uiPage in Application.AppContent.Pages">
					<div :key="uiPage.name" v-if="appIsReady && Application.pageIsActive (uiPage)" style="width:100%; height:100%; display:block;">
										
					<template v-for="uiModule in uiPage.modules">
						<Module_Factory :key="uiModule.uuid" :module="uiModule" :Application="Application" />
					</template>
								
					</div>		
				</template>
				
				<template v-for="uiCustomPage in Application.AppContent.CustomPages">
					<div :key="uiCustomPage.name" v-if="appIsReady && (Application.AppState.activePage == uiCustomPage.name)" style="width:100%; height:100%; display:block;">
										
						<CustomPage_Example v-if="(uiCustomPage.component == 'Example')" :CustomPage="uiCustomPage" :Application="Application" />
								
					</div>		
				</template>
			</v-card>
		</div>
		
		
		<template v-for="uiDialog in Application.AppContent.Dialogs">
			<v-dialog :key="uiDialog.name"						
				v-model="uiDialog.dialogIsActive"
				transition="dialog-bottom-transition"
				max-width="50vw"
				height="80vh"
			>
			
			<v-card> 
          <v-card-title v-if="uiDialog.title">
            {{ uiDialog.title }}
          </v-card-title>
		  
		  <v-card-text>
			<div style="overflow:auto">
			
				<template v-for="uiModule in uiDialog.modules">
					<Module_Factory :key="uiModule.uuid" :module="uiModule" :Application="Application" />
				</template>										

			</div>
				
		   </v-card-text>
			</v-card>			
				
        </v-dialog>		
		</template>
    </v-main>
	
    <v-footer color="#3C3C3C" class="text-right" min-height="30" app dark>
        <v-spacer />
        <span class="caption" v-if="appHasInformation">&copy; {{ Application.AppDefinition.TextCopyRight }}</span>		
    </v-footer>
	
	<v-snackbar
        v-model="snackBarVisible"
        :timeout="snackBarTimeout"
		:color="snackbarColor"
      >
        <div>{{ snackBarText }}</div>
  
        <template v-slot:action="{ attrs }">
          <v-btn
			text
			:color="snackbarFontColor"
            v-bind="attrs"
            @click="snackBarVisible = false"
          >
            <v-icon>mdi-close</v-icon>
          </v-btn>
        </template>
      </v-snackbar>
	
</v-app>
</template>

<script>

	import AMCApplication from "@core/common/AMCApplication.js"
	
	import Dialog_Login from "./dialogs/Dialog_Login.vue";
	import Dialog_Error from "./dialogs/Dialog_Error.vue";
	
	import Module_Factory from "./modules/AMCModule_Factory.vue";

	import CustomPage_Example from "./CustomPage_Example.vue";

	export default {

		created() {
			this.GlobalTimer = setInterval(this.uiOnTimer, 600);
			
			var baseURL = "/api";
			if (process.env.NODE_ENV === "development") {
				baseURL = "http://localhost:8869/api";
			}
			
			this.Application = new AMCApplication (baseURL, this.uiUpdateAppSizeDeferred);
			this.Application.retrieveConfiguration(this.$vuetify.theme.themes);
								
		},
		
		mounted() 
		{
			this.uiUpdateAppSizeDeferred ();
		},
				

		beforeDestroy() {
			if (this.GlobalTimer) {
				clearInterval(this.GlobalTimer);
			}
		},

		computed: {
			appIsLoading() {
				if (!this.Application)
					return false;

				if (!this.Application.AppState)
					return false;

				return this.Application.AppState.currentStatus === "initial";
			},

			appIsLogin() {
				if (!this.Application)
					return false;

				if (!this.Application.AppState)
					return false;
					
				return this.Application.AppState.currentStatus === "login";
			},

			appIsReady() {
				if (!this.Application)
					return false;

				if (!this.Application.AppState)
					return false;

				return this.Application.AppState.currentStatus === "ready";
			},

			appIsError() {
				if (!this.Application)
					return false;

				if (!this.Application.AppState)
					return false;

				return this.Application.AppState.currentStatus === "error";
			},

			appHasInformation() {
				if (!this.Application)
					return false;

				if (!this.Application.AppState)
					return false;

				return (this.Application.AppState.currentStatus === "login") || (this.Application.AppState.currentStatus === "ready");
			},
			
			appMainStyle() {
			
				let mainStyle = "overflow: none;";
			
				//if (this.appIsLogin) {
				
					if (this.Application.AppDefinition.LoginBackgroundImageUUID) {
						let imageurl = this.Application.getImageURL (this.Application.AppDefinition.LoginBackgroundImageUUID);
						mainStyle = mainStyle + "background-image: url(\"" + imageurl + "\"); background-size: cover; "
					}					
					
				//}
				return mainStyle;

			},

			snackBarVisible () {
				if (!this.Application)
					return false;
				return this.Application.SnackBar.Visible;
			},
			
			snackBarTimeout () {
				if (!this.Application)
					return 0;
				return this.Application.SnackBar.Timeout;
			},
			
			snackBarText () {
				if (!this.Application)
					return 0;
				return this.Application.SnackBar.Text;
			},
						
			snackbarColor () {
				if (!this.Application)
					return 0;
				return this.Application.SnackBar.Color;
			},
			
			snackbarFontColor () {
				if (!this.Application)
					return 0;
				return this.Application.SnackBar.FontColor;
			}
			
			
			
		},
		
		components: {
			Dialog_Login,
			Dialog_Error,
			Module_Factory,
			CustomPage_Example
		},	

		methods: {
			

			uiButtonCaptionCheck(caption) {
				var size = this.$vuetify.breakpoint.name;
				if (size == "xs" || size == "sm") return "";

				return caption;
			},
			
			
			uiToggleDrawer() {
				this.ShowDrawer = !this.ShowDrawer;
			},
								
				
			uiOnTimer() {
				if (this.Application) {
					// Refresh v2 frontend state first, then update local widgets.
					this.Application.retrieveFrontendState ()
					.finally (() => {
					this.Application.updateModules ();
					});
				}													
			},
			
			uiUpdateAppSizeDeferred() {
				setTimeout(() => {
					this.uiUpdateAppSize ();			
				})
			},
			
			uiToolbarClicked(toolbaritem) {
			
				if (toolbaritem.targetpage)
					this.Application.changePage(toolbaritem.targetpage);
					
				if (toolbaritem.eventname) 
					this.Application.triggerUIEvent(toolbaritem.eventname, toolbaritem.uuid, {});
					
			},
			
			uiMenuClicked(menuitem) {
			
				if (menuitem.targetpage)
					this.Application.changePage(menuitem.targetpage);
					
				if (menuitem.eventname) 
					this.Application.triggerUIEvent(menuitem.eventname, menuitem.uuid, {});
					
			},			

			uiUpdateAppSize () {
				// Height is handled by CSS flex layout; nothing to compute.
			}
						
		},

		data: () => ({
		
			Application: null,
			GlobalTimer: null,
			ShowDrawer: true,
								
		})
	};
</script>


<style>
/*
 * Establish a definite height chain from the document root.
 * Without height:100% on html+body, Vuetify's .v-application--wrap uses
 * only min-height:100vh (not height:100vh), which breaks height:100%
 * resolution in all descendant modules (Grid, LayerView, Logs, Tabs, etc.).
 */
html, body {
  height: 100%;
  overflow: hidden;
}

/* Override Vuetify's min-height:100vh so percentage heights resolve correctly. */
#AMCApp,
#AMCApp .v-application--wrap {
  height: 100%;
  min-height: 0 !important;
}

html {
  overflow: hidden !important;
  scrollbar-width: none;
  -ms-overflow-style: none;
}

html::-webkit-scrollbar {
  width: 0;
  height: 0;
}

/* Make v-main fill its allocated space and arrange children in a column. */
.amc-main .v-main__wrap {
  display: flex;
  flex-direction: column;
  height: 100%;
}

/* Full-height wrapper for the content card. */
.amc-content-wrap {
  flex: 1 1 0;
  min-height: 0;
  display: flex;
  flex-direction: column;
}

/* The card itself stretches to fill the wrapper. */
.amc-content-card {
  flex: 1 1 0;
  min-height: 0;
  background: var(--amcf-color-bg, #E0E3E7) !important;
  overflow: auto;
}

/* Navigation drawer — light surface with subtle border */
.v-navigation-drawer {
  background: var(--amcf-color-surface, #FFFFFF) !important;
  border-right: 1px solid var(--amcf-color-border, #C8CCD0) !important;
}

.v-navigation-drawer .v-list-item__title {
  color: var(--amcf-color-text, #1E1E1E) !important;
  font-weight: 500;
}

.v-navigation-drawer .v-list-item__subtitle {
  color: var(--amcf-color-text-muted, #5F6368) !important;
}

.v-navigation-drawer .v-icon {
  color: var(--amcf-color-text-muted, #5F6368) !important;
}

.v-navigation-drawer .v-divider {
  border-color: var(--amcf-color-border, #C8CCD0) !important;
}

/* Active/hovered nav item gets a ScanLab red left accent */
.v-navigation-drawer .v-list-item--active {
  border-left: 3px solid var(--amcf-color-primary, #C8102E);
  background: var(--amcf-color-primary-bg, rgba(200, 16, 46, 0.06)) !important;
}

.v-navigation-drawer .v-list-item--active .v-icon {
  color: var(--amcf-color-primary, #C8102E) !important;
}

/* Footer — light with top border */
.v-footer {
  background: var(--amcf-color-surface, #FFFFFF) !important;
  border-top: 1px solid var(--amcf-color-border, #C8CCD0) !important;
}

.v-footer .caption {
  color: var(--amcf-color-text-muted, #5F6368) !important;
}

/* Dialog styling */
.v-dialog .v-card {
  background: var(--amcf-color-surface, #FFFFFF) !important;
}

.v-dialog .v-card__title {
  color: var(--amcf-color-text, #1E1E1E) !important;
}

/* Scrollbar styling — subtle for light theme */
::-webkit-scrollbar {
  width: 8px;
  height: 8px;
}

::-webkit-scrollbar-track {
  background: var(--amcf-color-surface-inset, #D8DBDF);
}

::-webkit-scrollbar-thumb {
  background: var(--amcf-color-border-strong, #9BA3AB);
  border-radius: var(--amcf-radius-pill, 999px);
}

::-webkit-scrollbar-thumb:hover {
  background: var(--amcf-color-text-muted, #5F6368);
}
</style>
