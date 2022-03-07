<template>

<div v-if="(moduleitem.type=='layerview')">

  	<div>Layer View</div>
	
	<v-slider
	    dense
		hint="Layer Index"
		max="300"
		min="1"
		v-model="LayerIndex"
		@change="onLayerChanged"
	></v-slider>

	<v-slider
	    dense
		hint="Scaling"
		max="100"
		min="0"
		v-model="ScaleFactor"
		@change="onScalingChanged"
	></v-slider>

	<div ref="layerviewdiv" style="height:400px" v-resize="onResize" @mousedown="onStartDragging" @mousemove="onDragging" @wheel="onMouseWheel"></div>  

</div>

</template>

<script>

	import LayerViewImpl from "./LayerViewImpl.js"

	export default {
	  props: ["Application", "moduleitem"],
	  
	  mounted() {
		
			this.$nextTick(() => {
			
				var domelement = this.$refs.layerviewdiv;
				if (!domelement) 
					return;
				
				this.LayerViewerInstance = new LayerViewImpl (domelement);
				window.addEventListener('mouseup', this.onStopDragging);
					
			})

		},
		
		
	   data() {
			return {
				LayerViewerInstance: null,
				ScaleFactor: 0,
				LayerIndex: 1,
				dragging: false,
				dragcurrentx: 0,
				dragcurrenty: 0
			};
		},
		
		

	  methods: {
		onResize: function () {
			var domelement = this.$refs.layerviewdiv; 
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
			var builduuid = this.Application.AppState.activeObject;
		
			this.Application.axiosPostRequest ("/build/toolpath", { "builduuid": builduuid, "layerindex": this.LayerIndex })
			  .then(layerJSON => {
				
				this.LayerViewerInstance.loadLayer (layerJSON.data.segments);				
			  
			})
			.catch(err => {
				err;
				//
			});
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
			
			var domelement = this.$refs.layerviewdiv;
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
			
			this.ScaleFactor = this.ScaleFactor - deltaWheel * 5;
			
			this.LayerViewerInstance.SetAbsoluteScaling (Math.pow (1.03, this.ScaleFactor), localX, localY);
	
		}
		
		
	  }
	};
</script>