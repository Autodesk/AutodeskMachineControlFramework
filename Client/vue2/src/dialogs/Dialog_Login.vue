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

	<!-- ============================================================ -->
	<!-- CLASSIC — original two-step card with toolbar header          -->
	<!-- ============================================================ -->
	<v-row v-if="loginStyle === 'classic'" align="center" justify="center">
		<v-col cols="12" sm="8" md="4">
			<v-card class="elevation-12">
				<v-toolbar color="primary" dark flat>
					<v-toolbar-title>{{ Application.AppDefinition.TextApplicationName }}</v-toolbar-title>
				</v-toolbar>
				<v-card-text>
					<v-img v-if="Application.AppDefinition.LogoUUID != ''" v-bind:src="Application.getImageURL(Application.AppDefinition.LogoUUID)" v-bind:aspect-ratio="Application.AppDefinition.LogoAspectRatio" contain></v-img>

					<div class="text-right" style="width:90%; margin:auto;">

						<div class="text-center">
							<p style="padding:20px">{{ Application.AppDefinition.LoginWelcomeMessage }}</p>
						</div>

						<v-sheet v-if="uiUserSheet">
							<v-text-field ref="edit_login" label="User name" prepend-icon="mdi-account" type="text" v-model="uiLoginUser" autofocus clearable @keydown.enter="uiNextClick" />
							<v-btn v-on:click="uiNextClick" class="ma-1" color="primary" v-bind:disabled="!uiLoginUser" plain>
								<v-icon left>mdi-arrow-right-thick</v-icon>Next
							</v-btn>
						</v-sheet>
						<v-sheet v-if="!uiUserSheet">
							<v-text-field ref="edit_password" label="Password" prepend-icon="mdi-lock" type="password" clearable v-model="uiLoginPassword" @keydown.enter="uiLoginClick" />
							<v-btn class="ma-1" color="primary" plain v-on:click="uiBackClick">
								<v-icon left>mdi-arrow-left-thick</v-icon>Back
							</v-btn>
							<v-btn v-on:click="uiLoginClick" class="ma-1" color="primary" v-bind:disabled="!uiLoginPassword" plain>
								<v-icon left>mdi-login</v-icon>Login
							</v-btn>
						</v-sheet>
					</div>

					<p style="padding:20px"></p>
				</v-card-text>
			</v-card>
		</v-col>
	</v-row>

	<!-- ============================================================ -->
	<!-- INDUSTRIAL — frosted dark glass card over background image    -->
	<!-- ============================================================ -->
	<v-row v-else-if="loginStyle === 'industrial'" align="center" justify="center" class="login-fullscreen">
		<v-col cols="12" sm="8" md="5" lg="4" xl="3">
			<div class="login-glass login-glass--dark">
				<div class="login-glass__header">
					<v-img v-if="Application.AppDefinition.LogoUUID != ''" v-bind:src="Application.getImageURL(Application.AppDefinition.LogoUUID)" v-bind:aspect-ratio="Application.AppDefinition.LogoAspectRatio" contain max-height="48" class="mb-4"></v-img>
					<h1 class="login-glass__title">{{ Application.AppDefinition.TextApplicationName }}</h1>
					<p v-if="loginSubtitle" class="login-glass__subtitle">{{ loginSubtitle }}</p>
					<p v-if="Application.AppDefinition.LoginWelcomeMessage" class="login-glass__welcome">{{ Application.AppDefinition.LoginWelcomeMessage }}</p>
				</div>
				<div class="login-glass__body">
					<v-text-field ref="edit_login" label="User name" prepend-icon="mdi-account" type="text" v-model="uiLoginUser" dark autofocus clearable @keydown.enter="onIndustrialEnter" />
					<v-text-field ref="edit_password" label="Password" prepend-icon="mdi-lock" type="password" v-model="uiLoginPassword" dark clearable @keydown.enter="onIndustrialEnter" />
					<v-btn block large color="primary" :disabled="!uiLoginUser || !uiLoginPassword" @click="uiDirectLoginClick" class="mt-4">
						<v-icon left>mdi-login</v-icon>Sign In
					</v-btn>
				</div>
			</div>
		</v-col>
	</v-row>

	<!-- ============================================================ -->
	<!-- LUMINOUS — frosted white/light glass card over background     -->
	<!-- ============================================================ -->
	<v-row v-else-if="loginStyle === 'luminous'" align="center" justify="center" class="login-fullscreen">
		<v-col cols="12" sm="8" md="5" lg="4" xl="3">
			<div class="login-glass login-glass--light">
				<div class="login-glass__header">
					<v-img v-if="Application.AppDefinition.LogoUUID != ''" v-bind:src="Application.getImageURL(Application.AppDefinition.LogoUUID)" v-bind:aspect-ratio="Application.AppDefinition.LogoAspectRatio" contain max-height="48" class="mb-4"></v-img>
					<h1 class="login-glass__title">{{ Application.AppDefinition.TextApplicationName }}</h1>
					<p v-if="loginSubtitle" class="login-glass__subtitle">{{ loginSubtitle }}</p>
					<p v-if="Application.AppDefinition.LoginWelcomeMessage" class="login-glass__welcome">{{ Application.AppDefinition.LoginWelcomeMessage }}</p>
				</div>
				<div class="login-glass__body">
					<v-text-field ref="edit_login" label="User name" prepend-icon="mdi-account" type="text" v-model="uiLoginUser" autofocus clearable @keydown.enter="onIndustrialEnter" />
					<v-text-field ref="edit_password" label="Password" prepend-icon="mdi-lock" type="password" v-model="uiLoginPassword" clearable @keydown.enter="onIndustrialEnter" />
					<v-btn block large color="primary" :disabled="!uiLoginUser || !uiLoginPassword" @click="uiDirectLoginClick" class="mt-4">
						<v-icon left>mdi-login</v-icon>Sign In
					</v-btn>
				</div>
			</div>
		</v-col>
	</v-row>

	<!-- ============================================================ -->
	<!-- SPLIT — left branded panel + right form panel                 -->
	<!-- ============================================================ -->
	<v-row v-else-if="loginStyle === 'split'" no-gutters class="login-fullscreen login-split">
		<v-col cols="12" md="5" class="login-split__brand d-none d-md-flex" :style="splitBrandStyle">
			<div class="login-split__brand-inner">
				<v-img v-if="Application.AppDefinition.LogoUUID != ''" v-bind:src="Application.getImageURL(Application.AppDefinition.LogoUUID)" v-bind:aspect-ratio="Application.AppDefinition.LogoAspectRatio" contain max-height="56" class="mb-6"></v-img>
				<h1 class="login-split__brand-title">{{ Application.AppDefinition.TextApplicationName }}</h1>
				<p v-if="loginSubtitle" class="login-split__brand-subtitle">{{ loginSubtitle }}</p>
				<p v-if="Application.AppDefinition.LoginWelcomeMessage" class="login-split__brand-welcome">{{ Application.AppDefinition.LoginWelcomeMessage }}</p>
				<v-img v-if="panelImageURL" :src="panelImageURL" contain max-height="200" class="mt-8 login-split__panel-image"></v-img>
			</div>
		</v-col>
		<v-col cols="12" md="7" class="login-split__form">
			<div class="login-split__form-inner">
				<v-img v-if="Application.AppDefinition.LogoUUID != ''" v-bind:src="Application.getImageURL(Application.AppDefinition.LogoUUID)" v-bind:aspect-ratio="Application.AppDefinition.LogoAspectRatio" contain max-height="48" class="mb-4 d-md-none"></v-img>
				<h1 class="login-glass__title" style="color: #1E1E1E;">Sign In</h1>
				<p class="login-glass__subtitle" style="color: #5F6368;">Enter your credentials to continue</p>
				<div class="login-glass__body mt-6">
					<v-text-field ref="edit_login" label="User name" prepend-icon="mdi-account" type="text" v-model="uiLoginUser" autofocus clearable @keydown.enter="onIndustrialEnter" />
					<v-text-field ref="edit_password" label="Password" prepend-icon="mdi-lock" type="password" v-model="uiLoginPassword" clearable @keydown.enter="onIndustrialEnter" />
					<v-btn block large color="primary" :disabled="!uiLoginUser || !uiLoginPassword" @click="uiDirectLoginClick" class="mt-4">
						<v-icon left>mdi-login</v-icon>Sign In
					</v-btn>
				</div>
			</div>
		</v-col>
	</v-row>

	<!-- Fallback — same as classic -->
	<v-row v-else align="center" justify="center">
		<v-col cols="12" sm="8" md="4">
			<v-card class="elevation-12">
				<v-toolbar color="primary" dark flat>
					<v-toolbar-title>{{ Application.AppDefinition.TextApplicationName }}</v-toolbar-title>
				</v-toolbar>
				<v-card-text>
					<v-img v-if="Application.AppDefinition.LogoUUID != ''" v-bind:src="Application.getImageURL(Application.AppDefinition.LogoUUID)" v-bind:aspect-ratio="Application.AppDefinition.LogoAspectRatio" contain></v-img>
					<div class="text-right" style="width:90%; margin:auto;">
						<div class="text-center">
							<p style="padding:20px">{{ Application.AppDefinition.LoginWelcomeMessage }}</p>
						</div>
						<v-sheet v-if="uiUserSheet">
							<v-text-field ref="edit_login" label="User name" prepend-icon="mdi-account" type="text" v-model="uiLoginUser" autofocus clearable @keydown.enter="uiNextClick" />
							<v-btn v-on:click="uiNextClick" class="ma-1" color="primary" v-bind:disabled="!uiLoginUser" plain>
								<v-icon left>mdi-arrow-right-thick</v-icon>Next
							</v-btn>
						</v-sheet>
						<v-sheet v-if="!uiUserSheet">
							<v-text-field ref="edit_password" label="Password" prepend-icon="mdi-lock" type="password" clearable v-model="uiLoginPassword" @keydown.enter="uiLoginClick" />
							<v-btn class="ma-1" color="primary" plain v-on:click="uiBackClick">
								<v-icon left>mdi-arrow-left-thick</v-icon>Back
							</v-btn>
							<v-btn v-on:click="uiLoginClick" class="ma-1" color="primary" v-bind:disabled="!uiLoginPassword" plain>
								<v-icon left>mdi-login</v-icon>Login
							</v-btn>
						</v-sheet>
					</div>
					<p style="padding:20px"></p>
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

	computed: {
		loginStyle () {
			return (this.Application.AppDefinition.LoginStyle || "classic").toLowerCase();
		},

		loginSubtitle () {
			return this.Application.AppDefinition.LoginSubtitle || "";
		},

		panelImageURL () {
			var uuid = this.Application.AppDefinition.LoginPanelUUID;
			if (uuid) {
				return this.Application.getImageURL(uuid);
			}
			return "";
		},

		splitBrandStyle () {
			var colors = this.Application.AppDefinition.Colors || {};
			var primary = colors.primary || "#C8102E";
			return {
				backgroundColor: primary,
			};
		},
	},

	methods: {

		uiResetClick () {
			this.uiLoginUser = "";
			this.uiLoginPassword = "";
			setTimeout(() => {
				if (this.$refs.edit_login) {
					this.$refs.edit_login.focus();
				}
			});
		},

		uiLoginClick () {
			if (this.uiLoginUser && this.uiLoginPassword) {
				setTimeout(() => {
					this.Application.requestLogin(this.uiLoginUser, this.uiLoginPassword);
					this.uiLoginPassword = "";
				});
			}
		},

		uiDirectLoginClick () {
			if (this.uiLoginUser && this.uiLoginPassword) {
				this.Application.requestLogin(this.uiLoginUser, this.uiLoginPassword);
				this.uiLoginPassword = "";
			}
		},

		uiNextClick () {
			this.uiUserSheet = false;
			setTimeout(() => {
				if (this.$refs.edit_password) {
					this.$refs.edit_password.focus();
				}
			});
		},

		uiBackClick () {
			this.uiUserSheet = true;
			this.uiLoginPassword = "";
			setTimeout(() => {
				if (this.$refs.edit_login) {
					this.$refs.edit_login.focus();
				}
			});
		},

		onIndustrialEnter () {
			if (this.uiLoginUser && this.uiLoginPassword) {
				this.uiDirectLoginClick();
			}
		},
	}
};
</script>

<style scoped>

/* ------------------------------------------------------------------ */
/* Shared full-screen layout                                          */
/* ------------------------------------------------------------------ */
.login-fullscreen {
	width: 100%;
	height: 100%;
	margin: 0;
}

/* ------------------------------------------------------------------ */
/* Glass card — shared by industrial & luminous                       */
/* ------------------------------------------------------------------ */
.login-glass {
	border-radius: 16px;
	padding: 40px 36px;
	backdrop-filter: blur(18px) saturate(1.4);
	-webkit-backdrop-filter: blur(18px) saturate(1.4);
}

.login-glass--dark {
	background: rgba(20, 20, 30, 0.72);
	border: 1px solid rgba(255, 255, 255, 0.12);
	color: #ffffff;
}

.login-glass--light {
	background: rgba(255, 255, 255, 0.78);
	border: 1px solid rgba(0, 0, 0, 0.08);
	color: #212121;
	box-shadow: 0 8px 32px rgba(0, 0, 0, 0.12);
}

.login-glass__header {
	text-align: center;
	margin-bottom: 28px;
}

.login-glass__title {
	font-size: 1.6rem;
	font-weight: 600;
	letter-spacing: 0.02em;
	margin-bottom: 4px;
}

.login-glass__subtitle {
	font-size: 0.95rem;
	opacity: 0.75;
	margin-bottom: 4px;
}

.login-glass__welcome {
	font-size: 0.85rem;
	opacity: 0.6;
	margin-top: 8px;
	line-height: 1.5;
}

.login-glass__body {
	max-width: 360px;
	margin: 0 auto;
}

/* Dark-glass text-field overrides */
.login-glass--dark >>> .v-input__slot {
	border-color: rgba(255, 255, 255, 0.25) !important;
}

.login-glass--dark >>> .v-label,
.login-glass--dark >>> .v-icon {
	color: rgba(255, 255, 255, 0.7) !important;
}

.login-glass--dark >>> .v-text-field input {
	color: #ffffff !important;
}

/* ------------------------------------------------------------------ */
/* Split layout                                                       */
/* ------------------------------------------------------------------ */
.login-split {
	height: 100%;
}

.login-split__brand {
	display: flex;
	align-items: center;
	justify-content: center;
	color: #ffffff;
	position: relative;
	overflow: hidden;
}

.login-split__brand-inner {
	position: relative;
	z-index: 1;
	text-align: center;
	padding: 40px;
	max-width: 420px;
}

.login-split__brand-title {
	font-size: 2rem;
	font-weight: 700;
	letter-spacing: 0.02em;
	margin-bottom: 8px;
}

.login-split__brand-subtitle {
	font-size: 1.1rem;
	opacity: 0.85;
	margin-bottom: 4px;
}

.login-split__brand-welcome {
	font-size: 0.9rem;
	opacity: 0.7;
	line-height: 1.5;
}

.login-split__panel-image {
	opacity: 0.9;
}

.login-split__form {
	display: flex;
	align-items: center;
	justify-content: center;
	background: rgba(255, 255, 255, 0.92);
	backdrop-filter: blur(18px) saturate(1.4);
	-webkit-backdrop-filter: blur(18px) saturate(1.4);
	border-left: 1px solid rgba(0, 0, 0, 0.08);
	box-shadow: -4px 0 32px rgba(0, 0, 0, 0.06);
}

.login-split__form-inner {
	width: 100%;
	max-width: 400px;
	padding: 48px 32px;
	text-align: center;
}

</style>
