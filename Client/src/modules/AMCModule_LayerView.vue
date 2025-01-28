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
	
	<div v-if="(module.type == 'layerview')" class="layerview-container" @mousemove="onMouseMove">
			<div ref="layerViewDiv" class="layerview" v-resize="onResize" @mousedown="onStartDraggingRenderView" @wheel="onMouseWheel">
			</div>
								
			<div class="button-container">			
				<button class="rounded-button" @click="onResetViewClick">
					<v-icon left color="white">mdi-crop-free</v-icon>
					Reset View
				</button>
				<button class="rounded-button" @click="onFitViewClick">
					<v-icon left color="white">mdi-magnify-scan</v-icon>
					Fit to path
				</button>
				<button class="rounded-button" @click="onToggleHoverClick">
                    <v-icon v-if="hoverOverData" left size="20px" color="white">mdi-checkbox-outline</v-icon>
                    <v-icon v-else left size="20px" color="white">mdi-checkbox-blank-outline</v-icon>
                    Tooltips
                </button>
				
				<button class="rounded-button" @click="onToggleToolpathClick">
                    <v-icon v-if="LayerViewerInstance.toolpathVisible" left size="20px" color="white">mdi-checkbox-outline</v-icon>
                    <v-icon v-else left size="20px" color="white">mdi-checkbox-blank-outline</v-icon>
                    Toolpath
                </button>
				
				<!--<button class="rounded-button" @click="onToggleJumpsClick">
                    <v-icon v-if="showJumps" left size="20px" color="white">mdi-checkbox-outline</v-icon>
                    <v-icon v-else left size="20px" color="white">mdi-checkbox-blank-outline</v-icon>
                    Jumps
                </button>-->

			</div>

			<div class="layerview-slider-container">
                <div class="layerview-slider" id="sliderDiv">
                    <div class="slider-thumb" id="sliderThumbDiv" @mousedown="onStartDraggingSlider">
                        0
                    </div>
                </div>
 
                <div class="layerbutton-container">
                    <button class="rounded-button-small" @click="onPreviousLayerClick">
                        <v-icon center size="17px" color="white">mdi-minus</v-icon>
                    </button>
                    <button class="rounded-button-small" @click="onNextLayerClick">
                        <v-icon center size="17px" color="white">mdi-plus</v-icon>
                    </button>
                </div>
            </div>  					
			
			<div class="label-container">
                <div class="layertime-label" v-if="getLabelVisible()">
                    <v-icon center size="16px" color="white">{{ getLabelIcon () }}</v-icon>
					<div>{{ getLabelCaption () }}</div>
                </div>
 
                <div class="loading-label" v-if="loadingLayerData || loadingScatterplot">
                    <v-icon class="loading-icon" center size="26px" color="white">mdi-loading</v-icon>
                    <div class="loading-text">Loading layer data</div>
                </div>
            </div>
													
	</div>

</template>


<script>


	const ZOOM_MARGIN = 10;

	import WebGLImpl from "../common/AMCImplementation_WebGL.js"
	import LayerViewImpl from "../common/AMCImplementation_LayerView.js"

	export default {
		props: ["Application", "module"],
	  
		components: {
		},
		
		data: () => ({				
			glInstance: null,
			LayerViewerInstance: null,
			
			sliderLayerIndex: 1,
			sliderDragStartPosition: 0.0,
			
			draggingRenderView: false,
			draggingSlider: false,
			draggingRenderViewCurrentX: 0,
			draggingRenderViewCurrentY: 0,
			draggingSliderCurrentY: 0,
			lastMouseX: 0,
			lastMouseY: 0,
			reloading: false,
			showJumps: true,
			loadingLayerData: false,
			loadingScatterplot: false,
			hoverOverData: true
		
		}),
		
		methods: {
			onResize: function () {
				let domelement = this.$refs.layerViewDiv; 
				if (!domelement) 
					return;	
					
				if (!this.LayerViewerInstance) 
					return;
					
				this.LayerViewerInstance.updateSize (domelement.clientWidth, domelement.clientHeight);
				this.LayerViewerInstance.RenderScene (true);
			},
			
			onToggleJumpsClick: function () {
                this.showJumps = !this.showJumps;
            },
 
            onToggleToolpathClick: function () {
                this.LayerViewerInstance.toolpathVisible = !this.LayerViewerInstance.toolpathVisible;
				this.LayerViewerInstance.updateLoadedLayer ();
            },

			
			onResetViewClick: function () {
				let platform = this.module.platform;
				
				if (platform) {
					this.LayerViewerInstance.CenterOnRectangle (- ZOOM_MARGIN, - ZOOM_MARGIN, platform.sizex + ZOOM_MARGIN, platform.sizey + ZOOM_MARGIN);
					this.LayerViewerInstance.RenderScene (true);
				}
			},
			
			getLabelVisible: function ()
			{
				let platform = this.module.platform;
				if (platform) {						
					return platform.labelvisible;
				} 
			
				return false;
			},
			
			getLabelCaption: function ()
			{
				let platform = this.module.platform;
				if (platform) {						
					return platform.labelcaption;
				} 
			
				return "";
			},
			
			getLabelIcon: function ()
			{
				let platform = this.module.platform;
				if (platform) {						
					return platform.labelicon;
				} 
			
				return "";
			},
			
			onPreviousLayerClick: function () {
			
				if (this.module.platform) {		
					if (this.sliderLayerIndex > 0) {
						this.changeLayerTo (this.sliderLayerIndex - 1);				
					}
				}
            },
 
            onNextLayerClick: function () {
				if (this.module.platform) {
					let maxLayers = this.module.platform.layercount;
					if (this.sliderLayerIndex < maxLayers - 1) {
						this.changeLayerTo (this.sliderLayerIndex + 1);				
					}
				}						

            },
			
			onToggleHoverClick: function ()
			{
				this.hoverOverData = !this.hoverOverData;
			},

			onFitViewClick: function () {
				let platform = this.module.platform;
				let pathBoundaries = this.LayerViewerInstance.getPathBoundaries();

				if (pathBoundaries) {
				
					if (pathBoundaries.radius > 0) {

						const left = pathBoundaries.center.x - pathBoundaries.radius + platform.sizex / 2;
						const right = pathBoundaries.center.x + pathBoundaries.radius + platform.sizex / 2;
						const top = pathBoundaries.center.y - pathBoundaries.radius + platform.sizey / 2;
						const bottom = pathBoundaries.center.y + pathBoundaries.radius + platform.sizey / 2;

						this.LayerViewerInstance.CenterOnRectangle (left, top, right, bottom);
						this.LayerViewerInstance.RenderScene (true);
						return;
					
					}
					
				}

				this.onResetViewClick ();

			},

			updateSliderIndex: function (newSliderIndex) {
						
				let maxLayers = 0;
				if (this.module.platform) {
					maxLayers = this.module.platform.layercount;
				}						
					
				if (typeof newSliderIndex === 'number') {
				
					if (Number.isInteger(newSliderIndex)) {
									
						if ((newSliderIndex >= 0) && (maxLayers > 0)) {
							this.sliderLayerIndex = newSliderIndex;
							const sliderDiv = document.getElementById('sliderDiv');
							const sliderThumbDiv = document.getElementById('sliderThumbDiv');
							if (sliderThumbDiv && sliderDiv) {
								const sliderHeight = sliderDiv.getBoundingClientRect().height - sliderThumbDiv.offsetHeight;

								let sliderPosition = newSliderIndex / maxLayers;
								sliderThumbDiv.innerText = this.sliderLayerIndex;
								sliderThumbDiv.style.bottom = `${sliderPosition * (sliderHeight - sliderThumbDiv.offsetHeight) / sliderHeight * 100}%`;
							}
							
							
						} else {
							this.sliderLayerIndex = 0;
						}
					}
				}
			},
			
			onLayerChanged: function (sender) {

				let module = this.module;				
				let platform = this.module.platform;
				if (platform && sender) {

					if (module.isActive () && (sender.uuid === module.uuid)) {
																								
						if ((platform.displayed_layer != platform.currentlayer) || (platform.displayed_build != platform.builduuid)) {
						
							platform.displayed_layer = platform.currentlayer;
							platform.displayed_build = platform.builduuid;
							
							this.updateSliderIndex (platform.displayed_layer);
						
							this.loadingLayerData = true;
							this.Application.axiosPostRequest ("/build/toolpath", { "builduuid": platform.builduuid, "layerindex": platform.currentlayer })
								.then(layerJSON => {
								
								if (this.LayerViewerInstance) {
									this.LayerViewerInstance.loadLayer (layerJSON.data.segments);				
									this.LayerViewerInstance.RenderScene (true);
								}
								
								this.loadingLayerData = false;
							})
							.catch(err => {
								if (this.LayerViewerInstance) {
									this.LayerViewerInstance.RenderScene (true);
								}
								this.loadingLayerData = false;
								err;
								//
							});


							if (platform.scatterplotuuid != "00000000-0000-0000-0000-000000000000") {
							
									this.loadingScatterplot = true;
									this.LayerViewerInstance.glInstance.removeElement("layerdata_points");
								
									this.Application.axiosGetArrayBufferRequest("/ui/pointcloud/" + platform.scatterplotuuid)
									.then(responseData => {
										let pointcoordinates = new Float32Array(responseData.data);
										
										if (this.LayerViewerInstance) {
											this.LayerViewerInstance.loadPoints (pointcoordinates);
										}
									
										this.loadingScatterplot = false;
										
									})
									.catch(err => {
										if (err.response) {
											console.log (err.response);
										} else {
											console.log ("fatal error while retrieving point cloud ");
										}
										if (this.LayerViewerInstance) {
											this.LayerViewerInstance.RenderScene (true);
										}
										
										this.loadingScatterplot = false;
									});
							} else {
							
									this.loadingScatterplot = false;
									this.LayerViewerInstance.glInstance.removeElement("layerdata_points");
								
							
							}
						
						}
					}
				}
			},
			
			changeLayerTo (targetLayer)
			{
				console.log ("Changing layer to: " + targetLayer);
			
				if (this.module.platform) {					
					let requestObject = {
						targetlayer: targetLayer
					}
					this.Application.triggerWidgetRequest (this.module.platform.uuid, "changelayer", requestObject);
				}
			},
			
			onStartDraggingRenderView: function (event) {
				this.draggingRenderView = true;
				this.draggingRenderViewCurrentX = event.clientX;
				this.draggingRenderViewCurrentY = event.clientY;
				
				let boxRectangle = event.target.getBoundingClientRect();

				let localX = ( event.clientX - boxRectangle.left );
				let localY = ( event.clientY - boxRectangle.top );				  
				
				this.LayerViewerInstance.glInstance.pick2DElement(localX, localY);
			},

			onStartDraggingSlider: function (event) {
				if (this.module.platform) {
					this.draggingSlider = true;
					this.draggingSliderCurrentY = event.clientY;

					this.sliderDragStartPosition = this.sliderLayerIndex / this.module.platform.layercount;
				}						 
				
			},

			onStopDragging: function () {
				this.draggingRenderView = false;
				this.draggingRenderViewCurrentX = 0;
				this.draggingRenderViewCurrentY = 0;

				if (this.draggingSlider) {
					if (this.module.platform) {
						if (this.module.platform.displayed_layer != this.sliderLayerIndex) {
							this.changeLayerTo (this.sliderLayerIndex);
						}
					}
				}
		
				this.draggingSlider = false;
			},
			
			onMouseWheel: function (event) {
				event.preventDefault();
				
				let domelement = this.$refs.layerViewDiv;
				if (!domelement) 
					return;			
				if (!this.LayerViewerInstance) 
					return;

				let deltaWheel = event.deltaY;
				if (deltaWheel > 5)
					deltaWheel = 5;
				if (deltaWheel < -5)
					deltaWheel = -5;
					
				let viewportX = event.clientX;
				let viewportY = event.clientY;
				let boxRectangle = domelement.getBoundingClientRect();
				let localX = ( viewportX - boxRectangle.left );
				let localY = ( viewportY - boxRectangle.top );				
									
				let scaleFactor = - deltaWheel * 1.5;
				
				this.LayerViewerInstance.ScaleRelative (Math.pow (1.03, scaleFactor), localX, localY);
				this.LayerViewerInstance.RenderScene (true);
		
			},

			onMouseMove: function (event) {
				if (!this.glInstance || !this.LayerViewerInstance) {
					return;
				}

				if (this.draggingRenderView) {
					const deltaX = event.clientX - this.draggingRenderViewCurrentX;
					const deltaY = event.clientY - this.draggingRenderViewCurrentY;
					this.draggingRenderViewCurrentX = event.clientX;
					this.draggingRenderViewCurrentY = event.clientY;
					
					this.LayerViewerInstance.Drag(deltaX, deltaY);
					this.LayerViewerInstance.RenderScene (true);
				}
				else if (this.draggingSlider) {
				
					let maxLayers = 0;
					if (this.module.platform) {
						maxLayers = this.module.platform.layercount;
					}
				
					const sliderThumbDiv = document.getElementById('sliderThumbDiv');
					const sliderHeight = document.getElementById('sliderDiv').getBoundingClientRect().height - sliderThumbDiv.offsetHeight;
					
					const deltaY = this.draggingSliderCurrentY - event.clientY;
					
					let sliderPosition = this.sliderDragStartPosition + deltaY / sliderHeight;
					
					if (sliderPosition < 0.0) {
						sliderPosition = 0.0;
					}
					else if (sliderPosition > 1.0) {
						sliderPosition = 1.0;
					}

					this.updateSliderIndex (Math.round(sliderPosition * maxLayers));

				}

				
					const infoboxDiv = document.getElementById('infobox_points');

					if (!infoboxDiv) {
						return;
					}

					if (this.hoverOverData) {

						const renderElementPosition = this.glInstance.renderer.domElement.getBoundingClientRect();
						const mouseX = event.clientX - renderElementPosition.left;
						const mouseY = event.clientY - renderElementPosition.top;

						let pointIndex = this.LayerViewerInstance.glInstance.getRaycasterCollisions ("layerdata_points", mouseX, mouseY);
						if (pointIndex >= 0) {
						
							let pointPosition = this.LayerViewerInstance.getPointPosition (pointIndex);
							let pointVelocity = this.LayerViewerInstance.getPointVelocity (pointIndex);
							let pointAcceleration = this.LayerViewerInstance.getPointAcceleration (pointIndex);
							let pointJerk = this.LayerViewerInstance.getPointJerk (pointIndex);
							
							let infoCaption = `Point ID = ${pointIndex.toFixed(0)}\n`;
							if (pointPosition) {
								infoCaption += `Position: ${pointPosition.x.toFixed(4)}/${pointPosition.y.toFixed(4)} mm\n`;
							}
							if (pointVelocity) {
								infoCaption += `Velocity: ${pointVelocity.v.toFixed(4)} mm/s\n`;
							}
							if (pointAcceleration) {
								let accelerationinmeterspersecondsquared = pointAcceleration.a / 1000.0;
								infoCaption += `Acceleration: ${accelerationinmeterspersecondsquared.toFixed(4)} m/s²\n`;
							}

							if (pointJerk) {
								let jerkinmeterspersecondsquared = pointJerk.j / 1000000.0;
								infoCaption += `Jerk: ${jerkinmeterspersecondsquared.toFixed(4)} km/s³\n`;
							}

						
							this.lastMouseX = mouseX;
							this.lastMouseY = mouseY;
							infoboxDiv.innerText = infoCaption;
							infoboxDiv.style.background = 'rgba(0, 0, 0, 0.7)';
							infoboxDiv.style.display = 'flex';
							infoboxDiv.style.left = `${mouseX}px`;
							infoboxDiv.style.top = `${mouseY - infoboxDiv.getBoundingClientRect().height}px`; 
							
						} else {		 

							let lineIndex = this.LayerViewerInstance.glInstance.getRaycasterCollisions ("layerdata_lines", mouseX, mouseY);
							if (lineIndex >= 0) {

								this.lastMouseX = mouseX;
								this.lastMouseY = mouseY;
								
								if (this.LayerViewerInstance.linesCoordinates.length > 0) {
									const x1 = this.LayerViewerInstance.linesCoordinates[lineIndex * 4];
									const y1 = this.LayerViewerInstance.linesCoordinates[lineIndex * 4 + 1];
									const x2 = this.LayerViewerInstance.linesCoordinates[lineIndex * 4 + 2];
									const y2 = this.LayerViewerInstance.linesCoordinates[lineIndex * 4 + 3];
									
									const laserpower = this.LayerViewerInstance.segmentProperties[lineIndex].laserpower;
									const laserspeed = this.LayerViewerInstance.segmentProperties[lineIndex].laserspeed;

									let infoCaption = `Line ID = ${lineIndex.toFixed(0)}\n${x1.toFixed(3)} / ${y1.toFixed(3)} - ${x2.toFixed(3)} / ${y2.toFixed(3)} mm\n`;
									infoCaption += `${laserpower.toFixed(0)}W / ${laserspeed.toFixed (1)} mm/s`;
									infoboxDiv.innerText = infoCaption;
									infoboxDiv.style.background = 'rgba(0, 0, 0, 0.7)';
								}
								
								infoboxDiv.style.display = 'flex';
								infoboxDiv.style.left = `${mouseX}px`;
								infoboxDiv.style.top = `${mouseY - infoboxDiv.getBoundingClientRect().height}px`; 
							
							} else {
							
								let deltaX = this.lastMouseX - mouseX;
								let deltaY = this.lastMouseY - mouseY;
								
								if ((Math.abs (deltaX) > 10) || (Math.abs (deltaY) > 10)) {
							
									infoboxDiv.style.display = 'none';
									infoboxDiv.innerText = '---';
									return;
									
								}
								
							} 
						
						}
									

	
					}
				}
			
		},
		
		created () {
			this.module.onDataHasChanged = null;			
			if (this.module.platform) {
				this.module.platform.displayed_layer = 0;
				this.module.platform.displayed_build = 0;
			}
			
			this.glInstance = this.Application.retrieveWebGLInstance (module.uuid);

			if (!this.glInstance) {
				this.glInstance = new WebGLImpl ();

				this.Application.storeWebGLInstance (this.glInstance);				
				this.LayerViewerInstance = new LayerViewImpl (this.glInstance);				
				this.LayerViewerInstance.RenderScene (true);
				
			}
		},
		
		unmounted ()
		{
			if (this.module) {
				this.module.onDataHasChanged = null;			
				if (this.module.platform) {
					this.module.platform.displayed_layer = 0;
					this.module.platform.displayed_build = 0;
				}
			}
		},
		
		mounted() {	
	
			
			if (this.module) {
				this.module.onDataHasChanged = this.onLayerChanged;			
				if (this.module.platform) {
					this.module.platform.displayed_layer = 0;
					this.module.platform.displayed_build = 0;
				}
			}
		
			this.$nextTick(() => {
				const layerViewDiv = this.$refs.layerViewDiv;
				if (layerViewDiv && this.glInstance) {						
					let width = layerViewDiv.clientWidth;
					let height = layerViewDiv.clientHeight;

					if ((width > 0) && (height > 0)) {						
						this.glInstance.setupDOMElement (layerViewDiv);														
						this.LayerViewerInstance.updateSize (width, height);
						
						let platform = this.module.platform;

						if (platform) {						
							if (platform.baseimageresource) {
								this.LayerViewerInstance.SetBuildPlateSVG (this.Application.getImageURL (platform.baseimageresource));
							}
						
							this.LayerViewerInstance.setOrigin (platform.originx, platform.originy);
							this.LayerViewerInstance.CenterOnRectangle (- ZOOM_MARGIN, - ZOOM_MARGIN, platform.sizex + ZOOM_MARGIN, platform.sizey + ZOOM_MARGIN);
							this.viewed_platform_uuid = platform.uuid;
						}
						
						this.LayerViewerInstance.RenderScene (true);												
					}
				}
							
			})
			
			window.addEventListener ("mouseup", this.onStopDragging);
			
			if (this.LayerViewerInstance)
				this.LayerViewerInstance.RenderScene (true);
		}
	};
	
</script>

<style>
	.layerview-container {
		position: relative;
		width:100%;
		height:100%;
		display:block;
		overflow:hidden;
	}

	.layerview {
		width: 100%;
		height: 100%
	}

	.button-container {
		position: absolute;
		top: 10px;
		left: 10px;
		display: flex;
		gap: 5px;
	}

    .rounded-button {
		display: flex;
      	padding: 8px 20px;
     	border: none;
     	border-radius: 5px;
     	background: rgba(0, 0, 0, 0.7);
     	color: white;
     	font-size: 16px;
     	cursor: pointer;
     	transition: background-color 0.3s ease;
    }

    .rounded-button:hover {
      	background-color: rgba(0, 0, 0, 0.8);
    }
	
	.rounded-button-small {
        display: flex;
        align-items: center;
        justify-content: center;
        padding: 1px;
        border: none;
        border-radius: 5px;
        background: rgba(0, 0, 0, 0.7);
        color: white;
        cursor: pointer;
        transition: background-color 0.3s ease;
    }
 
    .rounded-button-small:hover {
        background-color: rgba(0, 0, 0, 0.8);
    }
	
	.layerbutton-container {
        display: flex;
        flex-direction: row;
        gap: 1px;
    }
 

	.layerview-slider-container {
      	position: absolute;
		top: 0px;
		right: 10px;
		display: flex;
  		flex-direction: column;
		align-items: center;
		gap: 5px;
		height: 100%;
		padding: 20px;
    }

    .layerview-slider {
		flex: 1;
      	position: relative;
		height: 100%;
  		width: 10px;
		border-radius: 5px;
		background: rgba(0, 0, 0, 0.4);
    }

	.slider-thumb {
		position: absolute;
		bottom: 0;
		width: 40px;
		height: 25px;
		display: flex;
		justify-content: center;
		align-items: center;
		color: #ffffff;
		font-size: 8pt;
		font-weight: bold;
		user-select: none;
		transform: translateX(-15px);
		background: #000000;
		border-radius: 10px;
		cursor: pointer;
	}
	
	
	.label-container {
        position: absolute;
        bottom: 10px;
        left: 10px;
        display: flex;
        flex-direction: row;
        gap: 4px;
        height: 44px;
    }
 
    .layertime-label {      
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        margin: 0px !important;
        padding: 4px 8px;
        border: none;
        border-radius: 5px;
        color: white;
        font-size: 8pt;
        background: rgba(0, 0, 0, 0.8);
        gap: 2px;
    }
 
    .layertime-value {
        display: flex;
        flex-direction: row;
        gap: 2px;
    }
 
    .loading-label {
        display: flex;
        flex-direction: row;
        margin: 0px !important;
        padding: 4px 8px;
        border: none;
        border-radius: 5px;
        background: rgba(0, 0, 0, 0.8);
        gap: 4px;
    }
 
    .loading-icon {
        animation: spin 1s linear infinite;
    }
 
    .loading-text {
        display: flex;
        align-items: center;
        color: white;
        font-size: 9pt;
    }
 
    @keyframes spin {
        from {
            transform: rotate(0deg);
        }
        to {
            transform: rotate(360deg);
        }
    }
 	
</style>
