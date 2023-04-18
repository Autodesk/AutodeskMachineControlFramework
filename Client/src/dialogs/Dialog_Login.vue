<!--

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
	* Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
	* Neither the name of the Autodesk Inc. nor the
	  names of its contributors may be used to endorse or promote products
	  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

!-->

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
								<p style="padding:20px">{{ Application.AppDefinition.LoginWelcomeMessage }}
								</p>
							</div>

							<v-sheet v-if="uiUserSheet">
														
								<v-text-field ref="edit_login" label="User name" prepend-icon="mdi-account" type="text" v-model="uiLoginUser" autofocus clearable @keydown.enter="uiNextClick" />
						
								<v-btn v-on:click="uiNextClick"
								class="ma-1"
								color="primary"
								v-bind:disabled="!uiLoginUser"
								plain>
									<v-icon left>mdi-arrow-right-thick</v-icon>Next
								</v-btn>
							</v-sheet>
							<v-sheet v-if="!uiUserSheet">
														
								<v-text-field ref="edit_password" label="Password" prepend-icon="mdi-lock" type="password" clearable v-model="uiLoginPassword" @keydown.enter="uiLoginClick" />

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