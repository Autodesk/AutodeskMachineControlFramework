<template>

<div v-if="(moduleitem.type=='buildlist')">  	

	<v-data-table
		:headers="moduleitem.headers"
		:items="moduleitem.entries"
		:items-per-page="moduleitem.entriesperpage"
		class="elevation-1"
		disable-pagination
		hide-default-footer
		search 
		width="100%"
		loadingText="moduleitem.loadingtext"
		@click:row="uiModuleBuildListClick"
		>													
										
	</v-data-table>		

</div>

</template>

<script>

export default {
  props: ["Application", "moduleitem"],
  
  methods: {	
  
	uiModuleBuildListClick: function (item) {
		if (item.detailpage != "") {
			this.Application.changePage (String (item.detailpage) + ":" + String (item.buildUUID));
		}
	},
			
  }
};
</script>