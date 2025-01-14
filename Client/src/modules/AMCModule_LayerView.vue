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
	
	<div v-if="(module.type == 'layerview')" class="layerview-container" @mousemove="onMouseMove" @mouseup="onStopDragging">
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
			</div>

			<div class="layerview-slider-container">
				<div class="layerview-slider" id="sliderDiv">
					<div class="slider-thumb" id="sliderThumbDiv" @mousedown="onStartDraggingSlider">
						0
					</div>
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
			LayerIndex: 1,
			draggingRenderView: false,
			draggingSlider: false,
			draggingRenderViewCurrentX: 0,
			draggingRenderViewCurrentY: 0,
			draggingSliderCurrentY: 0,
			sliderPosition: 0.0,
			sliderPositionWhileMove: 0.0,
			lastMouseX: 0,
			lastMouseY: 0,
			reloading: false,
		
		}),
		
		methods: {
			onResize: function () {
				var domelement = this.$refs.layerViewDiv; 
				if (!domelement) 
					return;	
					
				if (!this.LayerViewerInstance) 
					return;
					
				this.LayerViewerInstance.updateSize (domelement.clientWidth, domelement.clientHeight);
				this.LayerViewerInstance.RenderScene (true);
			},
			
			onResetViewClick: function () {
				var platform = this.module.platform;

				this.LayerViewerInstance.CenterOnRectangle (- ZOOM_MARGIN, - ZOOM_MARGIN, platform.sizex + ZOOM_MARGIN, platform.sizey + ZOOM_MARGIN);
				this.LayerViewerInstance.RenderScene (true);
			},

			onFitViewClick: function () {
				var platform = this.module.platform;
				var pathBoundaries = this.LayerViewerInstance.getPathBoundaries();

				const left = pathBoundaries.center.x - pathBoundaries.radius + platform.sizex / 2;
				const right = pathBoundaries.center.x + pathBoundaries.radius + platform.sizex / 2;
				const top = pathBoundaries.center.y - pathBoundaries.radius + platform.sizey / 2;
				const bottom = pathBoundaries.center.y + pathBoundaries.radius + platform.sizey / 2;

				this.LayerViewerInstance.CenterOnRectangle (left, top, right, bottom);
				this.LayerViewerInstance.RenderScene (true);
			},


			
			onLayerChanged: function () {
			
				var platform = this.module.platform;
				
				if (platform) {
									
					if ((platform.displayed_layer != platform.currentlayer) || (platform.displayed_build != platform.builduuid)) {
					
						platform.displayed_layer = platform.currentlayer;
						platform.displayed_build = platform.builduuid;
					
						this.reloading = true;
						this.Application.axiosPostRequest ("/build/toolpath", { "builduuid": platform.builduuid, "layerindex": platform.currentlayer })
							.then(layerJSON => {
							
							if (this.LayerViewerInstance) {
								this.LayerViewerInstance.loadLayer (layerJSON.data.segments);				
								this.LayerViewerInstance.RenderScene (true);
							}
							
							this.reloading = false;
						})
						.catch(err => {
							if (this.LayerViewerInstance) {
								this.LayerViewerInstance.RenderScene (true);
							}
							err;
							//
						});


						if (platform.scatterplotuuid != "00000000-0000-0000-0000-000000000000") {
							

								this.Application.axiosGetArrayBufferRequest("/ui/pointcloud/" + platform.scatterplotuuid)
								.then(responseData => {
									//alert ("pointcloud received")
									let pointcoordinates = new Float32Array(responseData.data);
									//alert ("pointcloud received: " + pointcoordinates.length)
									
									this.LayerViewerInstance.glInstance.add2DPointsGeometry("layerdata_points", pointcoordinates, 61, 0.002, 0xff0000);
								
									
								})
								.catch(err => {
									if (err.response) {
										console.log (err.response);
									} else {
										console.log ("fatal error while retrieving point cloud ");
									}
								});
						}
					}
				}
			},
			
			onStartDraggingRenderView: function (event) {
				this.draggingRenderView = true;
				this.draggingRenderViewCurrentX = event.clientX;
				this.draggingRenderViewCurrentY = event.clientY;
				
				var boxRectangle = event.target.getBoundingClientRect();

				var localX = ( event.clientX - boxRectangle.left );
				var localY = ( event.clientY - boxRectangle.top );				  
				
				this.LayerViewerInstance.glInstance.pick2DElement(localX, localY);
			},

			onStartDraggingSlider: function (event) {
				this.draggingSlider = true;
				this.draggingSliderCurrentY = event.clientY;
			},

			onStopDragging: function () {
				this.draggingRenderView = false;
				this.draggingRenderViewCurrentX = 0;
				this.draggingRenderViewCurrentY = 0;

				this.draggingSlider = false;
				this.sliderPositionWhileMove = this.sliderPosition;
			},
			
			onMouseWheel: function (event) {
				event.preventDefault();
				
				var domelement = this.$refs.layerViewDiv;
				if (!domelement) 
					return;			
				if (!this.LayerViewerInstance) 
					return;

				var deltaWheel = event.deltaY;
				if (deltaWheel > 5)
					deltaWheel = 5;
				if (deltaWheel < -5)
					deltaWheel = -5;
					
				var viewportX = event.clientX;
				var viewportY = event.clientY;
				var boxRectangle = domelement.getBoundingClientRect();
				var localX = ( viewportX - boxRectangle.left );
				var localY = ( viewportY - boxRectangle.top );				
									
				var scaleFactor = - deltaWheel * 1.5;
				
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
					
					this.sliderPosition = this.sliderPositionWhileMove + deltaY / sliderHeight;
					
					if (this.sliderPosition < 0.0) {
						this.sliderPosition = 0.0;
					}
					else if (this.sliderPosition > 1.0) {
						this.sliderPosition = 1.0;
					}

					this.LayerIndex = Math.round(this.sliderPosition * maxLayers);

					sliderThumbDiv.style.bottom = `${this.sliderPosition * (sliderHeight - sliderThumbDiv.offsetHeight) / sliderHeight * 100}%`;
					sliderThumbDiv.innerText = this.LayerIndex;
				}

				
					const infoboxDiv = document.getElementById('infobox_points');

					if (!infoboxDiv) {
						return;
					}

					/*const renderElementPosition = this.glInstance.renderer.domElement.getBoundingClientRect();
					const mouseX = event.clientX - renderElementPosition.left;
					const mouseY = event.clientY - renderElementPosition.top;

					let [elementType, faceIndex] = this.glInstance.getRaycasterCollisions(mouseX, mouseY);


					if (elementType < 0) {
					
						let deltaX = this.lastMouseX - mouseX;
						let deltaY = this.lastMouseY - mouseY;
						
						if ((Math.abs (deltaX) > 10) || (Math.abs (deltaY) > 10)) {
					
							infoboxDiv.style.display = 'none';
							infoboxDiv.innerText = '---';
							return;
						}
					} else {
						this.lastMouseX = mouseX;
						this.lastMouseY = mouseY;
					} 
					

					// Each datapoint consists of two triangles and two faceIDs. Only even IDs are of interest
					if (faceIndex % 2 !== 0) {
						faceIndex -= 1;
					}

					faceIndex = faceIndex / 2; // Ignore odd IDs

					// console.log("Collision with face index " + faceIndex);
					if (elementType === 0) { // Point
					
						if (this.LayerViewerInstance.pointCoordinates.length > 0) {					
							const x = this.LayerViewerInstance.pointCoordinates[faceIndex * 2];
							const y = this.LayerViewerInstance.pointCoordinates[faceIndex * 2 + 1];

							infoboxDiv.innerText = `Point ID = ${faceIndex.toFixed(0)}\nX = ${x.toFixed(3)} mm\nY = ${y.toFixed(3)} mm`;
							infoboxDiv.style.background = 'rgba(255, 0, 0, 0.7)';
						}
					}
					else if (elementType === 1) { // Line
						if (this.LayerViewerInstance.linesCoordinates.length > 0) {
							const x1 = this.LayerViewerInstance.linesCoordinates[faceIndex * 4];
							const y1 = this.LayerViewerInstance.linesCoordinates[faceIndex * 4 + 1];
							const x2 = this.LayerViewerInstance.linesCoordinates[faceIndex * 4 + 2];
							const y2 = this.LayerViewerInstance.linesCoordinates[faceIndex * 4 + 3];
							
							const laserpower = this.LayerViewerInstance.segmentProperties[faceIndex].laserpower;

							infoboxDiv.innerText = `Line ID = ${faceIndex.toFixed(0)}\n${x1.toFixed(3)} / ${y1.toFixed(3)} - ${x2.toFixed(3)} / ${y2.toFixed(3)} mm\n${laserpower.toFixed(0)}W`;
							infoboxDiv.style.background = 'rgba(0, 0, 0, 0.7)';
						}
					}
					
					infoboxDiv.style.display = 'flex';
					infoboxDiv.style.left = `${mouseX}px`;
					infoboxDiv.style.top = `${mouseY - infoboxDiv.getBoundingClientRect().height}px`; */
				}
			
		},
		
		created () {
			this.module.onDataHasChanged = this.onLayerChanged;			
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
		
		mounted() {	
			this.$nextTick(() => {
				const layerViewDiv = this.$refs.layerViewDiv;
				if (layerViewDiv && this.glInstance) {						
					var width = layerViewDiv.clientWidth;
					var height = layerViewDiv.clientHeight;

					if ((width > 0) && (height > 0)) {						
						this.glInstance.setupDOMElement (layerViewDiv);														
						this.LayerViewerInstance.updateSize (width, height);
						
						var platform = this.module.platform;

						if (platform) {						
							if (platform.baseimageresource) {
								this.LayerViewerInstance.SetBuildPlateSVG (this.Application.getImageURL (platform.baseimageresource));
							}
						
							this.LayerViewerInstance.setOrigin (platform.originx, platform.originy);
							this.LayerViewerInstance.CenterOnRectangle (- ZOOM_MARGIN, - ZOOM_MARGIN, platform.sizex + ZOOM_MARGIN, platform.sizey + ZOOM_MARGIN);
						}
						
						this.LayerViewerInstance.RenderScene (true);						
					}
				}
							
			})
			
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
</style>
