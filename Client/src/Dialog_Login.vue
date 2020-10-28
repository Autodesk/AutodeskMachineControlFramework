<template>

            <v-row align="center" justify="center">
                <v-col cols="12" sm="8" md="4">
                    <v-card class="elevation-12">
                        <v-toolbar color="primary" dark flat>
                            <v-toolbar-title>{{ Application.AppDefinition.TextApplicationName }}</v-toolbar-title>
                        </v-toolbar>
                        <v-card-text>
							<v-img v-if="Application.AppDefinition.LogoUUID != ''" v-bind:src="Application.getImageURL (Application.AppDefinition.LogoUUID)" v-bind:aspect-ratio="Application.AppDefinition.LogoAspectRatio" contain></v-img>
						
                            <v-form>
                                <v-text-field label="User" name="login" prepend-icon="mdi-account" type="text" v-model="uiLoginUser" autofocus clearable />
                                <v-text-field id="password" label="Password" name="password" prepend-icon="mdi-lock" type="password" clearable v-model="uiLoginPassword" />
                            </v-form>
                        </v-card-text>
                        <v-card-actions>
                            <v-spacer />
                            <v-btn color="primary" v-on:click="uiLogInClick">
                                <v-icon left>mdi-login</v-icon>Login
                            </v-btn>
                        </v-card-actions>
                    </v-card>
                </v-col>
            </v-row>
			
			
</template>

<script>

export default {  

  props: ['Application'],

  mounted() {
	
    },
	
	
   data() {
        return {
			uiLoginUser: "test",
			uiLoginPassword: "test"
			
        };
    },
	
	

  methods: {
  
        uiLogInClick() {
		
			this.Application.requestLogin (this.uiLoginUser, this.uiLoginPassword);			
			this.uiLoginPassword = "";
					
        }	
  }
};
</script>