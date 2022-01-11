<template>
	
	<div v-if="(module.type == 'glscene')" style="width:100%; height:100%; display:block; overflow:hidden;">
		
			<canvas ref="bjsCanvas" style="width:100%;height:100%;" />
		
	</div>

</template>

<script>

	import { Engine, Scene, ArcRotateCamera, Vector3, Vector4, MeshBuilder,  Texture, StandardMaterial, Color3, HemisphericLight } from "@babylonjs/core";
	import "@babylonjs/loaders/glTF";
	import"@babylonjs/loaders/STL";

	export default {
		props: ["Application", "module"],
	  
		components: {
		},
		
		data: () => ({				
			
		}),
		
		mounted() {
		
			MeshBuilder;
		
			const bjsCanvas = this.$refs.bjsCanvas;
			if (bjsCanvas) {
				const engine = new Engine(bjsCanvas);
				const scene = new Scene(engine);
				scene.clearColor = new Color3(1, 1, 1);
				
				const camera = new ArcRotateCamera("camera", -Math.PI / 2, Math.PI / 2.5, 10, new Vector3(0, 0, 0));
				//camera.mode = Camera.ORTHOGRAPHIC_CAMERA 
				camera.setTarget(Vector3.Zero());
				camera.attachControl(bjsCanvas, true);
				camera.panningDistanceLimit = 10;
				camera.lowerRadiusLimit = 3;
				camera.upperRadiusLimit = 30;
				camera.upperAlphaLimit = 1.4;
				camera.upperBetaLimit = 1.4;
				camera.lowerAlphaLimit = 0.3;
				camera.lowerBetaLimit = 0.3;
				camera.allowUpsideDown = false;
				
				var light = new HemisphericLight("light", new Vector3(10, 10, -20), scene);
				light.diffuse = new Color3(8.0, 8.0, 8.0);
				light.intensity = 0.2;

				const groundMat = new StandardMaterial("groundMat");
				groundMat.diffuseColor = new Color3(0.5, 0.5, 0.5);
				
				const boxMat = new StandardMaterial("boxMat");
				//boxMat.diffuseTexture = new Texture("https://assets.babylonjs.com/environments/cubehouse.png")
				boxMat.diffuseTexture = new Texture("/groundtexture.png", scene)
				boxMat.diffuseColor = new Color3(1.5, 1.5, 1.5);
				boxMat.ambientColor = new Color3(0.9, 0.9, 0.9);
				boxMat.ambientTexture = new Texture("/groundtexture.png", scene)
				
				const faceUV = [];
				faceUV[0] = new Vector4(0.5, 0.0, 0.75, 1.0); //rear face
				faceUV[1] = new Vector4(0.0, 0.0, 0.25, 1.0); //front face
				faceUV[2] = new Vector4(0.25, 0, 0.5, 1.0); //right side
				faceUV[3] = new Vector4(0.75, 0, 1.0, 1.0); //left side
				faceUV[4] = new Vector4(0, 0.0, 3, 3.0); //rear face
				faceUV[5] = new Vector4(0, 0.0, 3, 3.0); //rear face

				//const ground = MeshBuilder.CreateGround("ground", {width:100, height:100});
				//ground.material = groundMat;
				
				const box = MeshBuilder.CreateBox("box", {faceUV: faceUV, wrap: true, height: 0.1, width: 20, depth: 20}, scene);
				box.material = boxMat;
				box.position.y = -0.1;
				
				engine.runRenderLoop(() => {
					scene.render();
				});
			}
		}
		
	};
	
</script>