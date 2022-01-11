<template>
	
	<div v-if="(module.type == 'glscene')" style="width:100%; height:100%; display:block; overflow:hidden;">
		
			<div ref="glDiv" style="width:100%;height:100%;" />
		
	</div>

</template>

<script>

	import WebGLImpl from "./WebGLImpl.js"

	export default {
		props: ["Application", "module"],
	  
		components: {
		},
		
		data: () => ({				
			glInstance: null,
		}),
		
		methods: {
			    animate: function () {
					requestAnimationFrame(this.animate);

					if (this.glInstance)
						this.glInstance.renderScene ();

				},
		},
		
		
		created () {
			this.glInstance = this.Application.retrieveWebGLInstance (module.uuid);
			if (!this.glInstance) {
				this.glInstance = new WebGLImpl ();
				this.Application.storeWebGLInstance (this.glInstance);
				
				this.glInstance.add2DGridGeometry (1800, 1000, 50);
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
					this.animate();
				}
				
			}
		}
		
	};
	
</script>