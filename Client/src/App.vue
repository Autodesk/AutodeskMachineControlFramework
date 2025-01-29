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
    <v-navigation-drawer v-if="appIsReady" v-model="ShowDrawer" clipped :clipped-left="$vuetify.breakpoint.lgAndUp" disable-resize-watcher app style="opacity:90%">
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

    <v-app-bar app color="primary" dark v-if="appIsReady" :clipped-left="$vuetify.breakpoint.lgAndUp">
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

	<v-main app v-bind:style="appMainStyle" v-resize="uiUpdateAppSize">
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

		<div v-bind:style="appContentDivStyle" v-if="appIsReady">
			<v-card v-bind:style="appContentCardStyle" elevation="2" class="pa-3" width="90%" > 
				<template v-for="uiPage in Application.AppContent.Pages">
					<v-container :key="uiPage.name" v-if="appIsReady && Application.pageIsActive (uiPage)" style="width:100%; height:100%; display:block;">
										
						<template v-for="uiModule in uiPage.modules">
							<Module_Content :key="uiModule.name" v-if="(uiModule.type == 'content')" :module="uiModule" :Application="Application" />					
							<Module_Tabs :key="uiModule.name" v-if="(uiModule.type == 'tabs')" :module="uiModule" :Application="Application" />							
							<Module_Grid :key="uiModule.name" v-if="(uiModule.type == 'grid')" :module="uiModule" :Application="Application" />							
							<Module_Logs :key="uiModule.name" v-if="(uiModule.type == 'logs')" :module="uiModule" :Application="Application" />							
							<Module_GLScene :key="uiModule.name" v-if="(uiModule.type == 'glscene')" :module="uiModule" :Application="Application" />
							<Module_Graphic :key="uiModule.name" v-if="(uiModule.type == 'graphic')" :module="uiModule" :Application="Application" />
							<Module_LayerView :key="uiModule.name" v-if="(uiModule.type == 'layerview')" :module="uiModule" :Application="Application" />
						</template>										
								
					</v-container>		
				</template>
				
				<template v-for="uiCustomPage in Application.AppContent.CustomPages">
					<v-container :key="uiCustomPage.name" v-if="appIsReady && (Application.AppState.activePage == uiCustomPage.name)" style="width:100%; height:100%; display:block;">
										
						<CustomPage_Example v-if="(uiCustomPage.component == 'Example')" :CustomPage="uiCustomPage" :Application="Application" />
								
					</v-container>		
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
					<Module_Content :key="uiModule.name" v-if="(uiModule.type == 'content')" :module="uiModule" :Application="Application" />					
					<Module_Tabs :key="uiModule.name" v-if="(uiModule.type == 'tabs')" :module="uiModule" :Application="Application" />							
					<Module_Grid :key="uiModule.name" v-if="(uiModule.type == 'grid')" :module="uiModule" :Application="Application" />							
					<Module_Logs :key="uiModule.name" v-if="(uiModule.type == 'logs')" :module="uiModule" :Application="Application" />							
					<Module_GLScene :key="uiModule.name" v-if="(uiModule.type == 'glscene')" :module="uiModule" :Application="Application" />
					<Module_Graphic :key="uiModule.name" v-if="(uiModule.type == 'graphic')" :module="uiModule" :Application="Application" />
					<Module_LayerView :key="uiModule.name" v-if="(uiModule.type == 'layerview')" :module="uiModule" :Application="Application" />
				</template>										

			</div>
				
		   </v-card-text>
			</v-card>			
				
        </v-dialog>		
		</template>
    </v-main>
	
    <v-footer color="primary" class="text-right" min-height="30" app >
        <v-spacer />
        <span class="caption white--text" v-if="appHasInformation">&copy; {{ Application.AppDefinition.TextCopyRight }}</span>		
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

	import AMCApplication from "./common/AMCApplication.js"
	
	import Dialog_Login from "./dialogs/Dialog_Login.vue";
	import Dialog_Error from "./dialogs/Dialog_Error.vue";
	
	import Module_Content from "./modules/AMCModule_Content.vue";
	import Module_Tabs from "./modules/AMCModule_Tabs.vue";
	import Module_Grid from "./modules/AMCModule_Grid.vue";
	import Module_GLScene from "./modules/AMCModule_GLScene.vue";
	import Module_Graphic from "./modules/AMCModule_Graphic.vue";
	import Module_LayerView from "./modules/AMCModule_LayerView.vue";
	import Module_Logs from "./modules/AMCModule_Logs.vue";

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
			Module_Content,
			Module_Tabs,
			Module_Logs,
			Module_GLScene,
			Module_Graphic,
			Module_Grid,
			Module_LayerView,
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
					this.Application.updateContentItems ();			
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

				let height = window.innerHeight - this.$vuetify.application.footer - this.$vuetify.application.top;
				let style = "height: " + height + "px; padding-top: 20px;";
				if (style != this.appContentDivStyle) {
					this.appContentDivStyle = style;
				}

				height = window.innerHeight - this.$vuetify.application.footer - this.$vuetify.application.top - 40;
				style = "height: " + height + "px; background: rgba(255.0, 255.0, 255.0, 0.9); overflow:auto; margin:auto";
				if (style != this.appContentCardStyle) {
					this.appContentCardStyle = style;
				}
				
			}
						
		},

		data: () => ({
		
			Application: null,
			GlobalTimer: null,
			ShowDrawer: true,
			appContentDivStyle: "",
			appContentCardStyle: "",
								
		})
	};
</script>


<style>
html {
  overflow: hidden !important;
  scrollbar-width: none;
  -ms-overflow-style: none;
}

html::-webkit-scrollbar {
  width: 0;
  height: 0;
}
</style>
