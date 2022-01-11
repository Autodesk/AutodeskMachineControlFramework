<template>
	
	<div v-if="(module.type == 'graphic')" style="width:100%; height:100%; display:block; overflow:hidden;">
		<div ref="pixiDiv" style="width:100%; height:100%;"  />
				
	</div>

</template>

<script>

	import * as PIXI from 'pixi.js'

	export default {
		props: ["Application", "module"],
	  
		components: {
		},
		
		data: () => ({				
			
		}),
		
		created() {
				this.pixiApp = new PIXI.Application ( {  width: 400, height: 300, antialias: true, transparent: true });
					let sprite = PIXI.Sprite.from('http://localhost:8080/ui_logo.png');
			sprite.x = 250;
			sprite.width = 900;
			sprite.height = 300;
			this.pixiApp.stage.addChild(sprite);

			let graphics = new PIXI.Graphics() 
			graphics.beginFill(0x1A02D9);
			graphics.drawRect(0, 0, 100, 300);
			graphics.endFill();
			this.pixiApp.stage.addChild(graphics)

			graphics = new PIXI.Graphics() 
			graphics.lineStyle(1, 0x303030, 1);
			for (var x = 0; x < 300; x++) {
				graphics.moveTo(x * 3, 10);
				graphics.lineTo(x * 3, 990);
			}
			for (var y = 0; y < 300; y++) {
				graphics.moveTo(10, y * 3);
				graphics.lineTo(990, y * 3);
			}
			this.pixiApp.stage.addChild(graphics)
		},
		
		mounted() {
			const pixiDiv = this.$refs.pixiDiv;			
			
			/*{
				width: window.innerWidth,
				height: window.innerHeight,
				antialias: true,
				transparent: true,
				view: canvas,
			}) */
			
			pixiDiv.appendChild(this.pixiApp.view);
			this.pixiApp.resizeTo = pixiDiv;

		

		},
		
		onResize () {
			const pixiDiv = this.$refs.pixiDiv;
			if (pixiDiv) {
				console.log ("new width:" + pixiDiv.width);
			}
			
		},		
		
		beforeDestroy() {
			
		}
				
		
	};
	
</script>