<template>
	
	<div v-if="(module.type == 'glscene')" style="width:100%; height:100%; display:block; overflow:hidden;">
		
			<canvas ref="bjsCanvas" style="width:100%;height:100%;" />
		
	</div>

</template>

<script>

	import { Engine, Scene, FreeCamera, Vector3, /*MeshBuilder, SceneLoader, StandardMaterial, Color3,*/ HemisphericLight } from "@babylonjs/core";
	import "@babylonjs/loaders/glTF";
	import "@babylonjs/loaders/STL";

	export default {
		props: ["Application", "module"],
	  
		components: {
		},
		
		data: () => ({				
			
		}),
		
		mounted() {
			const bjsCanvas = this.$refs.bjsCanvas;
			if (bjsCanvas) {
				const engine = new Engine(bjsCanvas);
				const scene = new Scene(engine);
				const camera = new FreeCamera("camera1", new Vector3(0, 10, -20), scene);
				camera.setTarget(Vector3.Zero());
				camera.attachControl(bjsCanvas, true);
				
				new HemisphericLight("light", new Vector3(0, 10, -20), scene);

				//SceneLoader.ImportMesh("", "/data/", "triangle3.glb", scene, function (meshes, particleSystems, skeletons) {
				//});

				engine.runRenderLoop(() => {
					scene.render();
				});
			}
		}
		
	};
	
</script>