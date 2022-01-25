<template>
	
	<div v-if="(module.type == 'layerview')" style="width:100%; height:100%; display:block; overflow:hidden;">
		
			<div ref="layerViewDiv" style="width:100%;height:100%;" v-resize="onResize" @mousedown="onStartDragging" @mousemove="onDragging" @wheel="onMouseWheel" />
		
	</div>

</template>

<script>

	import WebGLImpl from "./WebGLImpl.js"
	import LayerViewImpl from "./LayerViewImpl.js"

	export default {
		props: ["Application", "module"],
	  
		components: {
		},
		
		data: () => ({				
			glInstance: null,
			LayerViewerInstance: null,
			ScaleFactor: 0,
			LayerIndex: 1,
			dragging: false,
			dragcurrentx: 0,
			dragcurrenty: 0
		
		}),
		
		methods: {
			    animate: function () {
					//requestAnimationFrame(this.animate);

					if (this.glInstance)
						this.glInstance.renderScene ();

				},
				
				onResize: function () {
					var domelement = this.$refs.layerViewDiv; 
					if (!domelement) 
						return;	
						
					if (!this.LayerViewerInstance) 
						return;
						
					this.LayerViewerInstance.updateSize (domelement.clientWidth, domelement.clientHeight);
				},
				
				
				onScalingChanged: function () {
					if (!this.LayerViewerInstance) 
						return;
						
					this.LayerViewerInstance.SetAbsoluteScaling (Math.pow (1.03, this.ScaleFactor));
								
				},
				
				
				onLayerChanged: function () {
				
					var platform = this.module.platform;
					
					if (platform) {
										
						if ((platform.displayed_layer != platform.currentlayer) || (platform.displayed_build != platform.builduuid)) {
						
							platform.displayed_layer = platform.currentlayer;
							platform.displayed_build = platform.builduuid;
						
							this.Application.axiosPostRequest ("/build/toolpath", { "builduuid": platform.builduuid, "layerindex": platform.currentlayer })
							  .then(layerJSON => {
								
								this.LayerViewerInstance.loadLayer (layerJSON.data.segments);				
							  
							})
							.catch(err => {
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
					
					this.ScaleFactor = this.ScaleFactor - deltaWheel * 1.5;
					
					this.LayerViewerInstance.SetAbsoluteScaling (Math.pow (1.03, this.ScaleFactor), localX, localY);
			
				}
				
		},
		
		
		created () {
			this.module.onDataHasChanged = this.onLayerChanged;
			
			this.glInstance = this.Application.retrieveWebGLInstance (module.uuid);
			if (!this.glInstance) {
				this.glInstance = new WebGLImpl ();
				this.Application.storeWebGLInstance (this.glInstance);
				
				this.LayerViewerInstance = new LayerViewImpl (this.glInstance);				
				
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
					}
					
				}
				
				window.addEventListener('mouseup', this.onStopDragging);			
							
					
			})
		}
		
	};
	
</script>