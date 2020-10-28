<template>

<div v-if="(moduleitem.type=='parameterlist')">  	
	
	<v-data-table
		:headers="moduleitem.headers"
		:items="moduleitem.entries"
		:items-per-page="moduleitem.entriesperpage"
		class="elevation-1"
		search 
		disable-pagination
		hide-default-footer
		width="100%"
		loadingText="moduleitem.loadingtext">
	</v-data-table>											
	
</div>

</template>

<script>

	export default {
	  props: ["Application", "moduleitem"],
	  
	};
	
</script>