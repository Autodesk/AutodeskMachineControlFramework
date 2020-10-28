<template>

        
	 <v-row align="center" justify="center">
		<v-col cols="12" sm="8" md="4">
			<v-card class="elevation-12">
				<v-toolbar color="primary" dark flat>
					<v-toolbar-title>Fatal error</v-toolbar-title>
				</v-toolbar>
				<v-card-text>
					Could not connect to server. Please try again later!
					
					{{ Application.AppState.currentError }}
				</v-card-text>
				<v-card-actions>
					<v-spacer />
					<v-btn color="primary" v-on:click="uiReloadPageClick">
						<v-icon left>mdi-refresh</v-icon>Reload page
					</v-btn>
				</v-card-actions>
			</v-card>
		</v-col>
	</v-row>		
			
</template>

<script>

export default {  

  props: ['Application'],
	
  methods: {

	uiReloadPageClick: function () {
		this.Application.setStatus ("initial");
		this.Application.retrieveConfiguration ();
	}
  }
};
</script>