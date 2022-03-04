<template>
	
	<div v-if="(module.type == 'graphic')" style="width:100%; height:100%; display:block; overflow:hidden;">
			
			<div ref="graphicDiv" style="width:100%;height:100%;" v-resize="onResize" @mousedown="onStartDragging" @mousemove="onDragging">
			</div>
								
	</div>

</template>

<script>

	import WebGLImpl from "./AMCImplementation_WebGL.js"
	import GraphicImpl from "./AMCImplementation_Graphic.js"

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
				  this.GraphicInstance.updateTransform ();
				  this.GraphicInstance.RenderScene (true);
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
				
				
				updateGraphic: function () {
					
					if (this.glInstance && this.GraphicInstance && this.module && this.module.items)  {
					
						this.GraphicInstance.BeginUpdate ();
					
						for (var item of this.module.items) {
							if (item.imageuuid && item.name) {
								this.GraphicInstance.AddSVGElement (this.Application.getImageURL (item.imageuuid), item.name, item.z);
								this.GraphicInstance.SetElementTranslation (item.name, item.x, item.y);
								this.GraphicInstance.SetElementScaling (item.name, item.scalex, item.scaley);
								
							}
						}								

						this.GraphicInstance.EndUpdate ();
					
					}
				
				}				
				
				
		},
		
		
		created () {
			
			this.glInstance = this.Application.retrieveWebGLInstance (module.uuid);
			if (!this.glInstance) {
				this.glInstance = new WebGLImpl ();
				this.Application.storeWebGLInstance (this.glInstance);
				
				this.GraphicInstance = new GraphicImpl (this.glInstance, this.module.showgrid);
				
				this.updateGraphic ();
								
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
						
						this.GraphicInstance.CenterOnRectangle (this.module.viewminx, this.module.viewminy, this.module.viewmaxx, this.module.viewmaxy);
						
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