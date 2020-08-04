<template>
	<v-app id="inspire">
		<v-navigation-drawer
				v-if="UI.isLoggedIn"
				v-model="UI.showDrawer"
				clipped
				:clipped-left="$vuetify.breakpoint.lgAndUp"
				disable-resize-watcher
				app
				>
				
			<v-list dense>
				<template v-for="item in UI.MenuItems">
					<v-list-group
							v-if="item.children"
							:key="item.text"
							v-model="item.model"
							:prepend-icon="item.model ? item.icon : item['icon-alt']"			
							>
						<template v-slot:activator>
							<v-list-item-content>
								<v-list-item-title v-on:click.stop="uiChangePage (item.targetPage, item.targetSubPage)">
									{{ item.text }}
								</v-list-item-title>
							</v-list-item-content>
						</template>

						<v-list-item
								v-for="(child, i) in item.children"
								:key="i"
								link
								v-on:click.stop="uiChangePage (child.targetPage, child.targetSubPage)"
								>
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

					<v-list-item
							v-else
							:key="item.text"
							link
							v-on:click.stop="uiChangePage (item.targetPage, item.targetSubPage)"
							>
						<v-list-item-action>
							<v-icon>{{ item.icon }}</v-icon>
						</v-list-item-action>
						<v-list-item-content>
							<v-list-item-title>
								{{ item.text }}
							</v-list-item-title>
						</v-list-item-content>
					</v-list-item>
				</template>
			</v-list>
		</v-navigation-drawer>

		
		<v-app-bar
				app
				color="primary"
				dark
				v-if="UI.isLoggedIn"
				:clipped-left="$vuetify.breakpoint.lgAndUp"
				>
			<v-app-bar-nav-icon v-on:click.stop="uiToggleDrawer" />
			<v-btn     
					tile		
					large
					color="primary"
					dark
					v-on:click.stop="uiChangePage ('main')"
					>
					
					{{ uiButtonCaptionCheck (UI.textApplicationName) }}
			</v-btn>


			<v-spacer />


			<v-btn color="primary" large v-on:click.stop="uiChangePage ('builds')">
				<v-icon left>mdi-factory</v-icon>{{ uiButtonCaptionCheck ("Builds") }}
			</v-btn>
			<v-btn color="primary" large v-on:click.stop="uiChangePage ('states')">
				<v-icon left>mdi-speedometer</v-icon>{{ uiButtonCaptionCheck ("Status") }}
			</v-btn>
			<v-btn color="primary" large v-on:click.stop="uiChangePage ('logs')">
				<v-icon left>mdi-message-text</v-icon>{{ uiButtonCaptionCheck ("Logs") }}
			</v-btn>
			<v-btn color="primary" large v-on:click.stop="uiChangePage ('settings')">
				<v-icon left>mdi-settings</v-icon>{{ uiButtonCaptionCheck ("Settings") }}				
			</v-btn>

		</v-app-bar>
		
		
		
		<v-content>
		
		
			<v-container class="fill-height" fluid v-if="!UI.isLoggedIn">
		
				<v-row align="center" justify="center">
					<v-col cols="12" sm="8" md="4">
						<v-card class="elevation-12">
							<v-toolbar color="primary" dark flat>
								<v-toolbar-title>{{ UI.textApplicationName }}</v-toolbar-title>
							</v-toolbar>
							<v-card-text>
								<v-form>
									<v-text-field label="User" name="login" prepend-icon="mdi-account" type="text" v-model="UI.uiLoginUser" autofocus clearable />
									<v-text-field id="password" label="Password" name="password" prepend-icon="mdi-lock" type="password" clearable v-model="UI.uiLoginPassword" />
								</v-form>
							</v-card-text>
							<v-card-actions>
								<v-spacer />
								<v-btn color="primary" v-on:click="uiLogInClick"><v-icon left>mdi-login</v-icon>Login</v-btn>
							</v-card-actions>
						</v-card>
					</v-col>
				</v-row>
				
			</v-container>
		
		
			<v-container fluid v-if="UI.isLoggedIn">
			
				<v-row v-if="UI.activePage == 'main'">
					<v-col cols="12" sm="12">

						<v-card width="100%" outlined>

							<v-card-title>Home</v-card-title>

						</v-card>

					</v-col>

				</v-row>
				
				
				<v-row v-if="UI.activePage == 'builds'">
					<v-col cols="12" sm="12">

						<v-card width="100%" outlined>

							<v-card-title>Builds</v-card-title>

								 <form enctype="multipart/form-data" novalidate v-if="uploadIsInitial || uploadIsSaving">							
									<div class="dropbox">
									  <input type="file" multiple :name="uploadFieldName" :disabled="uploadIsSaving" @change="uploadFilesChange($event.target.name, $event.target.files); fileCount = $event.target.files.length"
										accept="" class="input-file">
										<p v-if="uploadIsInitial">
										  Drag your build file here to begin or click to browse
										</p>
										<p v-if="uploadIsSaving">
										  Uploading {{ fileCount }} files...
										</p>
									</div>
								  </form>

								<v-data-table
												:headers="Builds.Headers"
												:items="Builds.Items"
												:items-per-page="15"
												class="elevation-1"
												width="100%"
												loadingText="Loading... Please wait"
												>

										</v-data-table>

						</v-card>
				
					</v-col>

				</v-row>

				<v-row v-if="(UI.activePage == 'states') && (UI.activeSubPage == '')">
					<v-col cols="12" sm="12">

						<v-card width="100%" outlined>

							<v-card-title>Machine Status</v-card-title>
							
										<v-data-table
												:headers="Parameters.Headers"
												:items="Parameters.Items"
												:items-per-page="15"
												class="elevation-1"
												disable-pagination
												hide-default-footer
												width="100%"
												loadingText="Loading... Please wait"
												>

										</v-data-table>
							
						</v-card>

						<v-card width="100%" outlined>

							<v-card-title>Subsystems</v-card-title>
							
							<v-list>
							
								<v-list-item-group color="primary">
								
									<v-list-item v-for="(instance, i) in Data.instanceList" :key="i">								
									
										<v-list-item-content> {{ instance.description }}</v-list-item-content>
									
									</v-list-item>
								
								</v-list-item-group>
							
							</v-list>
							
							
						</v-card>

						
					</v-col>
					
				</v-row>
					
				<v-row v-if="(UI.activePage == 'states') && (UI.activeSubPage != '')">
					
					<v-col cols="12" sm="12">

						<v-card width="100%" outlined>

							<v-card-title>{{ Data.instanceMap[this.UI.activeSubPage].description }}: {{ UI.activePageStatus }}</v-card-title>

						<div id="paper">

						</div>		

						<v-btn class="ms-2" color="primary" large v-on:click="drawGraph ()">
							<v-icon left>mdi-refresh</v-icon>
							Draw Graph
						</v-btn>

						</v-card>
				
					</v-col>

				</v-row>


				<v-row v-if="UI.activePage == 'settings'">
					<v-col cols="12" sm="12">

						<v-card width="100%" outlined>

							<v-card-title>Settings</v-card-title>

						</v-card>
				
					</v-col>

				</v-row>



				<v-row v-if="UI.activePage == 'logs'">
					<v-col cols="12" sm="12">

						<v-card width="100%" outlined>

							<v-card-title>Logs</v-card-title>

							<v-container fluid>
								<v-row>
									<v-col
											cols="12"
											sm="2"
											align="left">
										<v-card flat>
											<v-switch
													v-model="Logs.AutoUpdate"
													:label="'Automatic Update'">
											</v-switch>	  
										</v-card>
									</v-col>
									<v-col cols="12" sm="6">
										<v-card flat>
											<v-alert dense outlined type="error" v-if="logsError" color="primary">
												Could not retrieve machine logs!
											</v-alert>				
										</v-card>
									</v-col>
									<v-col
											cols="12"
											sm="4"
											align="right">

										<v-card flat>


											<v-btn class="ms-2" color="primary" large v-on:click="uiLogsDownload">
												<v-icon left>mdi-download</v-icon>
												Download
											</v-btn>
											<v-btn class="ms-2" color="primary" large v-on:click="uiLogsReset">
												<v-icon left>mdi-refresh</v-icon>
												Reload
											</v-btn>
										</v-card>
									</v-col>
								</v-row>
								<v-row>
									<v-col
											cols="12"
											sm="12"
											>
										<v-card>

											<v-data-table
													:headers="Logs.Headers"
													:items="Logs.Items"
													:items-per-page="15"
													disable-pagination
													class="elevation-1"
													dense
													disable-sort
													width="100%"
													loadingText="Loading... Please wait"
													loading="!logsAvailable"
													>

											</v-data-table>

										</v-card>
									</v-col>
								</v-row>
							</v-container>


						</v-card>
				
					</v-col>

				</v-row>

			</v-container>
		</v-content>

		<v-footer
				color="primary"
				class="text-right"
				app	  
				>
			<v-spacer/>
			<span class="caption white--text">&copy; {{ UI.textCopyRight }}</span>
		</v-footer>	

	</v-app>
</template>



<script>

  import axios from "axios";
  import Dracula from 'graphdracula'
  
  export default {
    props: {
      source: String,
    },
	
	created () {
		this.UI.uiTimer = setInterval (this.uiOnTimer, 600);
							
		this.Graph = Dracula.Graph
		this.Renderer = Dracula.Renderer.Raphael
		this.Layout = Dracula.Layout.Spring
		
		if (process.env.NODE_ENV == "development") {
			this.API.baseURL = "http://localhost:8869/api";
		}
		
		this.dataRetrieveSetup ();
									
	},	
	
	beforeDestroy() {
			if (this.UI.uiTimer != '') {
				clearInterval (this.UI.uiTimer);
			}
	},
	
	computed: {
		
		logsAvailable: function () {
			return (this.Logs.LastID > 0);
		
		},
		
		logsError: function () {
			return (this.Logs.ErrorCount > 5);
		},
		
        uploadIsInitial() {
          return this.Upload.currentStatus == "initial";
        },
        uploadIsSaving() {
          return this.Upload.currentStatus == "saving";
        },
        uploadIsSuccess() {
          return this.Upload.currentStatus == "success";
        },
        uploadIsFailed() {
          return this.Upload.currentStatus == "failed";
        }		
		
		
	},
		
	methods: {
	
		uiLogInClick () {
			
			this.UI.uiLoginPassword = "";
			
			this.UI.isLoggedIn = true;
		
		},
		
		
		uiButtonCaptionCheck (caption) {
		
			var size = this.$vuetify.breakpoint.name;
			if ((size == "xs") || (size == "sm")) 
				return "";
				
			return caption;
		},
	
		uiToggleDrawer () {
			this.UI.showDrawer = !this.UI.showDrawer;
		},
				
		uiChangePage (page, subpage) {		
			this.UI.activePage = page;			
			if (subpage) {
				this.UI.activeSubPage = subpage;
			} else {
				this.UI.activeSubPage = "";
			}
			
			if (page == 'logs') {
				this.retrieveLogs();
			}
			
			this.uiUpdateActivePageStatus ();
			
		},
		
		uiOnTimer () {
			if (this.Logs.AutoUpdate) {
				this.retrieveLogs();		
			}
			
			this.dataRetrieveStatus();
		},

		uiLogsReset () {
			this.Logs.Items = [];
			this.Logs.LastID = 0;
			this.Logs.ErrorCount = 0;
			this.retrieveLogs();		
		},
		
		uiLogsDownload () {		
		
		
		},
		
		uiUpdateActivePageStatus ()
		{
			if (this.UI.activeSubPage != "") {
				this.UI.activePageStatus = this.Data.instanceStatus[this.UI.activeSubPage];
			} else {
				this.UI.activePageStatus = "";
			}
		},
		
		
		uploadReset () {
			this.Upload.currentStatus = "initial";
			this.Upload.uploadedFiles = [];
			this.Upload.uploadError = null;
		},
		
		
		
		uploadSave (formData) {
        
			this.Upload.currentStatus = 'saving';
			formData.teset = 4;

			//upload(formData)
//				.then(x => {
					//this.uploadedFiles = [].concat(x);
					//this.currentStatus = STATUS_SUCCESS;
//				})
				//.catch(err => {
//					this.uploadError = err.response;
					//this.currentStatus = STATUS_FAILED;
				//});
				
		},
		
		uploadFilesChange (fieldName, fileList) {

			const formData = new FormData();
			if (!fileList.length) 
				return;

			// append the files to FormData
			Array
				.from(Array(fileList.length).keys())
				.map(x => {
					formData.append(fieldName, fileList[x], fileList[x].name);
				});

			// save it
			this.uploadSave(formData);
		},
		
		
		
		dataRetrieveSetup () {
			var url = this.API.baseURL + "/setup/";	
			axios({ method: "GET", "url": url }).then(result => {
			
				var instances = result.data.instances;
				if (instances) {
				
				
					var Sections = [];
					this.UI.MenuItems[2].children = Sections;
					
					var instanceCount = instances.length;
					for (var instanceIdx = 0; instanceIdx < instanceCount; instanceIdx++) {
						
						var instance = instances[instanceIdx];
						
						var instanceSection = { icon: 'mdi-signal-cellular-outline', text: instance.description, id: instance.name, targetPage: 'states', targetSubPage: instance.name }
						Sections.push (instanceSection);
						
						
						this.Data.instanceList.push (instance);
						this.Data.instanceMap[instance.name] = instance;											
												
						var parameterGroups = instance.parametergroups;
						if (parameterGroups) {
						
							var groupCount = parameterGroups.length;
							for (var groupIndex = 0; groupIndex < groupCount; groupIndex++) {
							
								var group = parameterGroups[groupIndex];
								var parameters = group.parameters;
																
								if (parameters) {
																		
									var paramCount = parameters.length;
									
									for (var paramIdx = 0; paramIdx < paramCount; paramIdx++) {
										
										var parameter = parameters[paramIdx];
										var dataParam =
										{
											parameterName: parameter.name,
											parameterDesc: parameter.description,
											parameterGroup: group.name,
											parameterGroupDesc: group.description,
											parameterValue: "",
											instanceName: instance.name,
											instanceDescription: instance.description,
										}				

										var paramURL = instance.name + "/" + group.name + "/" + parameter.name;
										this.Data.parameterListMap[paramURL] = dataParam;
																			
										this.Parameters.Items.push (dataParam);
									
									}								
								
								}														
							
							}						
						
						}
						
					}
									
				}
			
			
			}, error => {			
				console.error(error);
			});
			
		},
		
		dataRetrieveStatus () {
			var url = this.API.baseURL + "/status/";	
			axios({ method: "GET", "url": url }).then(result => {
			
				var instances = result.data.instances;
				if (instances) {
				
									
					var instanceCount = instances.length;
					for (var instanceIdx = 0; instanceIdx < instanceCount; instanceIdx++) {
						
						var instance = instances[instanceIdx];
																	
						this.Data.instanceStatus[instance.name] = instance.state;
												
						var parameterGroups = instance.parametergroups;
						if (parameterGroups) {
						
							var groupCount = parameterGroups.length;
							for (var groupIndex = 0; groupIndex < groupCount; groupIndex++) {
							
								var group = parameterGroups[groupIndex];
								var parameters = group.parameters;
																
								if (parameters) {
																		
									var paramCount = parameters.length;
									
									for (var paramIdx = 0; paramIdx < paramCount; paramIdx++) {
																											
										var parameter = parameters[paramIdx];
										var paramURL = instance.name + "/" + group.name + "/" + parameter.name;
										this.Data.parameterListMap[paramURL].parameterValue = parameter.value;
																																						
									}								
								
								}														
							
							}						
						
						}
						
					}
									
				}
				
				
				this.uiUpdateActivePageStatus ();
			
			
			}, error => {			
				console.error(error);
			});
			
		},
		
		drawGraph () {

			
					var states = this.Data.instanceMap[this.UI.activeSubPage].states;


					var graphData = { graph: {}, layout: {}, renderer: {} };
					graphData.graph = new this.Graph()

				
					var stateCount = states.length;
					var idx;
					
					for (idx = 0; idx < stateCount; idx++) {
						graphData.graph.addNode (states[idx].name);
						
						var outstates = states[idx].outstates;
						if (outstates) {
							var outstatelen = outstates.length;
							
							for (var j = 0; j < outstatelen; j++) {
								graphData.graph.addEdge (states[idx].name, outstates[j]);
								
							}
						}
						
						
					}
				
				

//					graphData.graph.addEdge('Banana', 'Apple')

					graphData.layout = new this.Layout(graphData.graph)
					
					var paperDiv = document.getElementById("paper");
					var graphdivname = "graphpaper";
					
					
					var edgecount = graphData.graph.edges.length;
					for (idx = 0; idx < edgecount; idx++) {
						if (graphData.graph.edges[idx].shape) {
							graphData.graph.edges[idx].shape = null;
						}
					}
					
					if (paperDiv.childNodes.length > 0) {
						paperDiv.removeChild (paperDiv.childNodes[0]);
					}
					
					
					var div = document.createElement(graphdivname);
					div.style.width = "1000px";
					div.style.height = "500px";
					div.style.color = "white";			
					graphData.element = div;

					paperDiv.appendChild(div);
					
					graphData.renderer = new this.Renderer(div, graphData.graph, 1000, 500)
					graphData.renderer.draw()
				
				
				
		
		
					
		},
		
		
			retrieveLogs () {
			
			
			
				var url = this.API.baseURL + "/logs/" + (this.Logs.LastID + 1);	
				console.log (url);
				axios({ method: "GET", "url": url }).then(result => {
												
				var logs=result.data.logs;
				
				if (logs) {
				
				var loglen = logs.length;
				for (var idx = 0; idx < loglen; idx++) {
					var resultLog = logs[idx];
				
					var logitem = {
						Id: resultLog.id,
						logDate: resultLog.date,
						logTime: resultLog.time,
						subsystem: resultLog.subsystem,
						logText: resultLog.message										
					}
					
					this.Logs.LastID = resultLog.id;
					this.Logs.Items.push (logitem);
				}
				
				
				}

				this.Logs.ErrorCount = 0;
				
				
			}, error => {
				this.Logs.ErrorCount++;
			
				console.error(error);
			});

		}
		
	},
	
    data: () => ({
	
	  API: {
		baseURL: "/api"
	  },
	
	  UI: {
		textApplicationName: "Autodesk Machine Control",
		textCopyRight: "2020 Autodesk Inc.",
		
		isLoggedIn: false,
		uiLoginUser: "",
		uiLoginPassword: "",
		
		showDrawer: true,
		activePage: "main", 
	    activeSubPage: "",	
		activePageStatus: "",
		uiTimer: '',
		
		MenuItems: [
			{ icon: 'mdi-apps', text: 'Home', targetPage: 'main' },
			{ icon: 'mdi-factory', text: 'Builds', targetPage: 'builds' },
			{
				icon: 'mdi-speedometer-medium',
				'icon-alt': 'mdi-speedometer',
				text: 'Machine Status',
				model: false,
				targetPage: 'states',
				children: [
				],
			},
			{ icon: 'mdi-message-text', text: 'Machine Log', targetPage: 'logs'},
			{ icon: 'mdi-settings', text: 'Settings', targetPage: 'settings' },
		],
		
		
	  },
	  
	  
	  Upload: {
		uploadedFiles: [],
        uploadError: null,
        currentStatus: "initial",
        uploadFieldName: "buildfile"
	  },
	  
	  Data: {
		instanceList: [	],
		instanceMap: [	],
		instanceStatus: [ ],
		parameterListMap: [ ]
	  },
	  
	  Parameters: {

		Headers: [
          { text: 'Parameter', value: 'parameterDesc' },
		  { text: 'Value', value: 'parameterValue' },
          { text: 'Group', value: 'parameterGroupDesc' },
		  { text: 'System', value: 'instanceDescription' },
		  
        ],
        Items: [ ],
		ErrorCount: 0,
	  },

	  Builds: {

		Headers: [
          { text: 'Image', value: 'buildImage' },
          { text: 'Build', value: 'buildName' },
		  { text: 'File name', value: 'fileName' },
		  { text: 'Upload time', value: 'uploadTime' },
		  { text: 'Uploader', value: 'uploadAccount' },
          { text: 'Layers', value: 'layerCount' },
		  
        ],
        Items: [ ],
		ErrorCount: 0,
	  },
	  
	  
	  Logs: {
	  
		Headers: [
          { text: 'Time', value: 'logDate' },
          { text: 'Date', value: 'logTime' },
          { text: 'Subsystem', value: 'subsystem' },
          { text: 'Text', value: 'logText' },
        ],
        Items: [ ],
		LastID: 0,
		ErrorCount: 0,
		AutoUpdate: false,
	  
	  },
	  	  	  
	  
	  Graph: {},
	  Renderer: {},
	  Layout: {},
	  
	    	  
    }),
  }
</script>



<style lang="scss">
  .dropbox {
    outline: 2px dashed grey; /* the dash box */
    outline-offset: -10px;
    color: dimgray;
    padding: auto;
    min-height: 100px; /* minimum height */
    position: relative;
    cursor: pointer;
  }

  .input-file {
    opacity: 0; /* invisible but it's there! */
    width: 100%;
    height: 100px;
    position: absolute;
    cursor: pointer;
  }

  .dropbox:hover {
    background: #f0f0f0; /* when mouse over to the drop zone, change color */
  }

  .dropbox p {
    font-size: 1.2em;
    text-align: center;
    padding: 50px 0;
  }
</style>