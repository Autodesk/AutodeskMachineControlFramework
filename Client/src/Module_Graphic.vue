<template>
	
	<div v-if="(module.type == 'graphic')" style="width:100%; height:100%; display:block; overflow:hidden;">
			
			<div ref="graphicDiv" style="width:100%;height:100%;" v-resize="onResize" @mousedown="onStartDragging" @mousemove="onDragging">
			</div>
								
	</div>

</template>

<script>

	import WebGLImpl from "./WebGLImpl.js"
	import GraphicImpl from "./GraphicImpl.js"

	export default {
		props: ["Application", "module"],
	  
		components: {
		},
		
		data: () => ({				
			glInstance: null,
			GraphicInstance: null,
			dragging: false,
			dragcurrentx: 0,
			dragcurrenty: 0
		
		}),
		
		methods: {
				
				onResize: function () {
					var domelement = this.$refs.GraphicDiv; 
					if (!domelement) 
						return;	
						
					if (!this.GraphicInstance) 
						return;
						
					this.GraphicInstance.updateSize (domelement.clientWidth, domelement.clientHeight);
					this.GraphicInstance.RenderScene (true);
				},
				
													
				
				onStartDragging: function (event) {
				  this.dragging = true;
				  this.dragcurrentx = event.clientX;
				  this.dragcurrenty = event.clientY;
				  
				  var boxRectangle = event.target.getBoundingClientRect();
 
				  var localX = ( event.clientX - boxRectangle.left );
			      var localY = ( event.clientY - boxRectangle.top );				  
				  
				  this.GraphicInstance.glInstance.pick2DElement (localX, localY);
				},
				
				onStopDragging: function () {
				  this.dragging = 0;
				  this.dragcurrentx = 0;
				  this.dragcurrenty = 0;
				},
				
				onDragging: function () {
				  if (this.dragging) {
				  
				  
/*					var deltaX = event.clientX - this.dragcurrentx;
					var deltaY = event.clientY - this.dragcurrenty;
					this.dragcurrentx = event.clientX;
					this.dragcurrenty = event.clientY;
										
					if (this.GraphicInstance) {
						//this.GraphicInstance.Drag (deltaX, deltaY);
						this.GraphicInstance.RenderScene (true);
					} */
				  }
				},							
				
		},
		
		
		created () {
			
			this.glInstance = this.Application.retrieveWebGLInstance (module.uuid);
			if (!this.glInstance) {
				this.glInstance = new WebGLImpl ();
				this.Application.storeWebGLInstance (this.glInstance);
				
				this.GraphicInstance = new GraphicImpl (this.glInstance);				
				this.GraphicInstance.RenderScene (true);
				
			}
		},
		
		mounted() {
		
		
				
			this.$nextTick(() => {

				const graphicDiv = this.$refs.graphicDiv;
				if (graphicDiv && this.glInstance) {						
				
					var width = graphicDiv.clientWidth;
					var height = graphicDiv.clientHeight;
					if ((width > 0) && (height > 0)) {						
						this.glInstance.setupDOMElement (graphicDiv);														
						this.GraphicInstance.updateSize (width, height);
						
						/*var platform = this.module.platform;
						if (platform) {
						
							if (platform.baseimageresource) {
								this.GraphicInstance.SetBuildPlateSVG (this.Application.getImageURL (platform.baseimageresource));
							}
						
							this.GraphicInstance.CenterOnRectangle (- ZOOM_MARGIN, - ZOOM_MARGIN, platform.sizex + ZOOM_MARGIN, platform.sizey + ZOOM_MARGIN);
						} */
						
						this.GraphicInstance.RenderScene (true);
						
					}
					
				}
				
				window.addEventListener('mouseup', this.onStopDragging);			
							
					
			})
			
			if (this.GraphicInstance)
				this.GraphicInstance.RenderScene (true);
		}
		
	};
	
</script>