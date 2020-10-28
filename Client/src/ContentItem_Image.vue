<template>

<div v-if="(moduleitem.type=='image')">  	
	<v-img v-bind:src="Application.getImageURL (moduleitem.uuid)" v-bind:aspect-ratio="moduleitem.aspectratio" v-bind:max-width="moduleitem.maxwidth" v-bind:max-height="moduleitem.maxheight" contain></v-img>
</div>

</template>

<script>

	export default {
	  props: ["Application", "moduleitem"]
	  
	};
	
</script>