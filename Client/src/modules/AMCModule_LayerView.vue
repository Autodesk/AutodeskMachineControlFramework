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
	
	<div v-if="(module.type == 'layerview')" style="width:100%; height:100%; display:block; overflow:hidden;">
			
			<div ref="layerViewDiv" style="width:100%;height:100%;" v-resize="onResize" @mousedown="onStartDragging" @mousemove="onDragging" @wheel="onMouseWheel">
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
			dragging: false,
			dragcurrentx: 0,
			dragcurrenty: 0,
			reloading: false
		
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

						
					}
				}
			},
			
			onStartDragging: function (event) {
				this.dragging = true;
				this.dragcurrentx = event.clientX;
				this.dragcurrenty = event.clientY;
				
				var boxRectangle = event.target.getBoundingClientRect();

				var localX = ( event.clientX - boxRectangle.left );
				var localY = ( event.clientY - boxRectangle.top );				  
				
				this.LayerViewerInstance.glInstance.pick2DElement (localX, localY);
			},
			
			onStopDragging: function () {
				this.dragging = 0;
				this.dragcurrentx = 0;
				this.dragcurrenty = 0;
			},
			
			onDragging: function () {
				if (this.dragging) {
				
				var deltaX = event.clientX - this.dragcurrentx;
				var deltaY = event.clientY - this.dragcurrenty;
				this.dragcurrentx = event.clientX;
				this.dragcurrenty = event.clientY;
									
				if (this.LayerViewerInstance) {
					this.LayerViewerInstance.Drag (deltaX, deltaY);
					this.LayerViewerInstance.RenderScene (true);
				}
				}
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
				if (this.glInstance) {
					const renderElementPosition = this.glInstance.renderer.domElement.getBoundingClientRect();
					const mouseX = event.clientX - renderElementPosition.left;
					const mouseY = event.clientY - renderElementPosition.top;

					let [elementType, faceIndex] = this.glInstance.getRaycasterCollisions(mouseX, mouseY);

					const infoboxDiv = document.getElementById('infobox_points');
					if (infoboxDiv) {

						if (elementType < 0) {
							infoboxDiv.style.display = 'none';
							infoboxDiv.innerText = '---';
							return;
						}

						// console.log(elementType);
						// console.log(faceIndex);

						// Each datapoint consists of two triangles and two faceIDs. Only even IDs are of interest
						if (faceIndex % 2 !== 0) {
							faceIndex -= 1;
						}

						faceIndex = faceIndex / 2; // Ignore odd IDs
					

						// console.log("Collision with face index " + faceIndex);
						if (elementType === 0) { // Point
							const x = this.LayerViewerInstance.pointCoordinates[faceIndex * 2];
							const y = this.LayerViewerInstance.pointCoordinates[faceIndex * 2 + 1];

							infoboxDiv.innerText = `Point ID = ${faceIndex.toFixed(0)}\nX = ${x.toFixed(3)} mm\nY = ${y.toFixed(3)} mm`;
							infoboxDiv.style.background = 'rgba(255, 0, 0, 0.7)';
						}
						else if (elementType === 1) { // Line
							const x1 = this.LayerViewerInstance.linesCoordinates[faceIndex * 4];
							const y1 = this.LayerViewerInstance.linesCoordinates[faceIndex * 4 + 1];
							const x2 = this.LayerViewerInstance.linesCoordinates[faceIndex * 4 + 2];
							const y2 = this.LayerViewerInstance.linesCoordinates[faceIndex * 4 + 3];

							const laserpower = this.LayerViewerInstance.segmentProperties[faceIndex].laserpower;
							const laserspeed = this.LayerViewerInstance.segmentProperties[faceIndex].laserspeed;
							const profilename = this.LayerViewerInstance.segmentProperties[faceIndex].profilename;

							infoboxDiv.innerText = `Line ID = ${faceIndex.toFixed(0)}: ${profilename}\n${x1.toFixed(3)}/${y1.toFixed(3)} - ${x2.toFixed(3)}/${y2.toFixed(3)} mm\n ${laserpower.toFixed(3)} W / ${laserspeed.toFixed(3)} mm/s`;
							infoboxDiv.style.background = 'rgba(0, 0, 0, 0.7)';
						}
						
						infoboxDiv.style.display = 'flex';
						infoboxDiv.style.left = `${mouseX}px`;
						infoboxDiv.style.top = `${mouseY - infoboxDiv.getBoundingClientRect().height}px`;					
					}
				}
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

				window.addEventListener('mousemove', this.onMouseMove, false);

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
				
				window.addEventListener('mouseup', this.onStopDragging);
			})
			
			if (this.LayerViewerInstance)
				this.LayerViewerInstance.RenderScene (true);
		}
	};
	
</script>