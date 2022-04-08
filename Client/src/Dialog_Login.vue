<template>

            <v-row align="center" justify="center">
                <v-col cols="12" sm="8" md="4">
                    <v-card class="elevation-12">
                        <v-toolbar color="primary" dark flat>
                            <v-toolbar-title>{{ Application.AppDefinition.TextApplicationName }}</v-toolbar-title>
                        </v-toolbar>
                        <v-card-text>
							<v-img v-if="Application.AppDefinition.LogoUUID != ''" v-bind:src="Application.getImageURL (Application.AppDefinition.LogoUUID)" v-bind:aspect-ratio="Application.AppDefinition.LogoAspectRatio" contain></v-img>							
														
							<div class="text-right" style="width:90%; margin:auto; ">

							<div class="text-center">
								<p style="padding:20px">Welcome to the Autodesk Machine Control Framework.
								This is SLM Test Machine 1. Please enter your account credential to log into the system.
								</p>
							</div>

							<v-sheet v-if="uiUserSheet">
														
								<v-form>
									<v-text-field ref="edit_login" label="User name" prepend-icon="mdi-account" type="text" v-model="uiLoginUser" autofocus clearable @keydown.enter="uiNextClick" />
								</v-form>
						
								<v-btn v-on:click="uiNextClick"
								class="ma-1"
								color="primary"
								v-bind:disabled="!uiLoginUser"
								plain>
									<v-icon left>mdi-arrow-right-thick</v-icon>Next
								</v-btn>
							</v-sheet>
							<v-sheet v-if="!uiUserSheet">
														
								<v-form>
									<v-text-field ref="edit_password" label="Password" prepend-icon="mdi-lock" type="password" clearable v-model="uiLoginPassword" @keydown.enter="uiLoginClick" />
								</v-form>
								<v-btn class="ma-1"
								color="primary"
								plain v-on:click="uiBackClick">
									<v-icon left>mdi-arrow-left-thick</v-icon>Back
								</v-btn>
								<v-btn v-on:click="uiLoginClick"
								class="ma-1"
								color="primary"
								v-bind:disabled="!uiLoginPassword"
								plain>
									<v-icon left>mdi-login</v-icon>Login
								</v-btn>
							</v-sheet>
							</div>

								<p style="padding:20px">
								</p>
                        </v-card-text>
                    </v-card>
					
                </v-col>
            </v-row>
			
			
</template>

<script>

export default {  

  props: ['Application'],
	
  data() {
        return {
			uiLoginUser: "",
			uiLoginPassword: "",
			uiUserSheet: true,
			
        };
  },
	
	

  methods: {
  
		uiResetClick () {
			this.uiLoginUser = "";
			this.uiLoginPassword = "";
			setTimeout(() => {
				this.$refs.edit_login.focus()
			})			
		},
  
        uiLoginClick() {
			if (this.uiLoginUser && this.uiLoginPassword) {
				setTimeout(() => {
					this.Application.requestLogin (this.uiLoginUser, this.uiLoginPassword);			
					this.uiLoginPassword = "";
				});
			}
        },
		
		uiNextClick() {
			this.uiUserSheet = false;
			setTimeout(() => {
				this.$refs.edit_password.focus()
			})
		},
		
		uiBackClick() {
			this.uiUserSheet = true;
			this.uiLoginPassword = "";
			setTimeout(() => {
				this.$refs.edit_login.focus()
			})
		}
  }
};
</script>