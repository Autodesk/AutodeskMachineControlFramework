<template>

	<div v-if="(module.type == 'logs')" style="width:100%; height:100%; padding:10px;">

		<v-data-table			
			:headers="Headers"
			:items="module.DisplayItems"
			:items-per-page="25"
			class="elevation-1"
			dense
			disable-sort
			loadingText="Loading... Please wait"
			loading="!logsAvailable"
			style="margin-bottom:20px"
		>
		</v-data-table>

	</div>	

</template>

<script>

	export default {
		props: ["Application", "module"],
	  
		components: {
		},
		
		data: () => ({
			Headers: [
				{ text: 'Time', value: 'logTime', width: "50pt", fixed: true },
				{ text: 'Text', value: 'logText' },
			],
			
			logsAvailable: false,
			tab: null
		})
		
	};
	
</script>