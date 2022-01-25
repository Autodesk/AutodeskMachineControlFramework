<template>
	
	<div v-if="(module.type == 'glscene')" style="width:100%; height:100%; display:block; overflow:hidden;">
		
			<div ref="glDiv" style="width:100%;height:100%;"  v-resize="onResize" />
		
	</div>

</template>

<script>

	import WebGLImpl from "./WebGLImpl.js"

	export default {
		props: ["Application", "module"],
	  
		components: {
		},
		
		data: () => ({				
			glInstance: null
		}),
		
		methods: {
			    animate: function () {
					requestAnimationFrame(this.animate);
					
					if (this.mesh) {
						const time = Date.now() * 0.001;

						this.mesh.rotation.y = time * 0.4;
					}

					if (this.glInstance)
						this.glInstance.renderScene ();

				},
				
				onResize: function () {
					var domelement = this.$refs.glDiv; 
					if (!domelement) 
						return;	
						
					if (!this.LayerViewerInstance) 
						return;
						
					this.LayerViewerInstance.updateSize (domelement.clientWidth, domelement.clientHeight);
				},
		},
		
		
		created () {
			this.glInstance = this.Application.retrieveWebGLInstance (module.uuid);
			if (!this.glInstance) {
				this.glInstance = new WebGLImpl ();
				this.Application.storeWebGLInstance (this.glInstance);
								
			}
		},
		
		mounted() {
				
			const glDiv = this.$refs.glDiv;
			if (glDiv && this.glInstance) {						
			
				var width = glDiv.clientWidth;
				var height = glDiv.clientHeight;
				if ((width > 0) && (height > 0)) {
					this.glInstance.setup2DView (width, height, 0.1, 100);					
					this.glInstance.resizeTo (width, height);
					this.glInstance.setupDOMElement (glDiv);														
					this.mesh = this.glInstance.setupDemoScene ();
					this.animate();
				}
				
			}
		}
		
	};
	
</script>