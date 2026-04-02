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
	<component
		v-if="resolvedComponent"
		:is="resolvedComponent"
		:key="module.uuid"
		:module="module"
		:Application="Application"
	/>
</template>

<script>

	import Module_GLScene       from '../modules/AMCModule_GLScene.vue';
	import Module_Graphic       from '../modules/AMCModule_Graphic.vue';
	import Module_LayerView     from '../modules/AMCModule_LayerView.vue';
	import Module_Logs          from '../modules/AMCModule_Logs.vue';
	import Module_ParameterList from '../modules/AMCModule_ParameterList.vue';
	import Module_Form          from '../modules/AMCModule_Form.vue';
	import Module_Chart         from '../modules/AMCModule_Chart.vue';
	import Module_Image         from '../modules/AMCModule_Image.vue';
	import Module_Paragraph     from '../modules/AMCModule_Paragraph.vue';
	import Module_Upload        from '../modules/AMCModule_Upload.vue';
	import Module_BuildList     from '../modules/AMCModule_BuildList.vue';
	import Module_ExecutionList from '../modules/AMCModule_ExecutionList.vue';
	import Module_AlertList     from '../modules/AMCModule_AlertList.vue';
	import Module_ButtonGroup   from '../modules/AMCModule_ButtonGroup.vue';
	import Module_ConfigurationList from '../modules/AMCModule_ConfigurationList.vue';
	import Module_VideoStream   from '../modules/AMCModule_VideoStream.vue';
	import Module_Separator     from '../modules/AMCModule_Separator.vue';
	import Module_StatusBanner  from '../modules/AMCModule_StatusBanner.vue';

	// Container modules use async imports to avoid circular dependencies.
	// (Factory → Content → Factory, Factory → Grid → Factory, etc.)
	const Module_Content = () => import('../modules/AMCModule_Content.vue');
	const Module_Grid    = () => import('../modules/AMCModule_Grid.vue');
	const Module_Tabs    = () => import('../modules/AMCModule_Tabs.vue');

	const componentMap = {
		content:           Module_Content,
		grid:              Module_Grid,
		tabs:              Module_Tabs,
		glscene:           Module_GLScene,
		graphic:           Module_Graphic,
		layerview:         Module_LayerView,
		logs:              Module_Logs,
		parameterlist:     Module_ParameterList,
		form:              Module_Form,
		chart:             Module_Chart,
		image:             Module_Image,
		paragraph:         Module_Paragraph,
		upload:            Module_Upload,
		buildlist:         Module_BuildList,
		executionlist:     Module_ExecutionList,
		alertlist:         Module_AlertList,
		buttongroup:       Module_ButtonGroup,
		configurationlist: Module_ConfigurationList,
		videostream:       Module_VideoStream,
		separator:         Module_Separator,
		statusbanner:      Module_StatusBanner,
	};

	export default {
		name: 'Module_Factory',
		props: ['Application', 'module'],

		computed: {
			resolvedComponent () {
				if (!this.module || !this.module.type)
					return null;
				return componentMap[this.module.type] || null;
			}
		}
	};

</script>
