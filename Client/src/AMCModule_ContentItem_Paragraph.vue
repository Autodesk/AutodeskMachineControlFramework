<template>

<div v-if="(moduleitem.type=='paragraph')">  	
	{{ moduleitem.text }}
</div>

</template>

<script>

	export default {

	  props: ["Application", "moduleitem"]  
	  	  
	};
	
	
</script>