<template>
	
	<div v-if="(module.type == 'layerview')" style="width:100%; height:100%; display:block; overflow:hidden;">
		
			<div ref="layerViewDiv" style="width:100%;height:100%;" v-resize="onResize" @mousedown="onStartDragging" @mousemove="onDragging" @wheel="onMouseWheel">
								
			</div>
								
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
			LayerIndex: 1,
			dragging: false,
			dragcurrentx: 0,
			dragcurrenty: 0
		
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
						
							this.Application.axiosPostRequest ("/build/toolpath", { "builduuid": platform.builduuid, "layerindex": platform.currentlayer })
							  .then(layerJSON => {
								
								if (this.LayerViewerInstance) {
									this.LayerViewerInstance.loadLayer (layerJSON.data.segments);				
									this.LayerViewerInstance.RenderScene (true);
								}
							  
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
						this.LayerViewerInstance.CenterOnRectangle (-10, -10, 110, 110);
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