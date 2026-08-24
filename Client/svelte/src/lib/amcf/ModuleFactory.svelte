<script lang="ts">
	import ModuleParagraph from './modules/ModuleParagraph.svelte';
	import ModuleContent from './modules/ModuleContent.svelte';
	import ModuleGrid from './modules/ModuleGrid.svelte';
	import ModuleForm from './modules/ModuleForm.svelte';
	import ModuleButtonGroup from './modules/ModuleButtonGroup.svelte';
	import ModuleImage from './modules/ModuleImage.svelte';
	import ModuleSeparator from './modules/ModuleSeparator.svelte';
	import ModuleStatusBanner from './modules/ModuleStatusBanner.svelte';
	import ModuleTabs from './modules/ModuleTabs.svelte';
	import ModuleParameterList from './modules/ModuleParameterList.svelte';
	import ModuleStatusList from './modules/ModuleStatusList.svelte';
	import ModuleTogglePanel from './modules/ModuleTogglePanel.svelte';
	import ModuleLogs from './modules/ModuleLogs.svelte';
	import ModuleAlertList from './modules/ModuleAlertList.svelte';
	import ModuleUpload from './modules/ModuleUpload.svelte';
	import ModuleBuildList from './modules/ModuleBuildList.svelte';
	import ModuleExecutionList from './modules/ModuleExecutionList.svelte';
	import ModuleConfigurationList from './modules/ModuleConfigurationList.svelte';
	import ModuleChart from './modules/ModuleChart.svelte';
	import ModuleVideoStream from './modules/ModuleVideoStream.svelte';
	import ModuleLayerView from './modules/ModuleLayerView.svelte';
	import ModuleGLScene from './modules/ModuleGLScene.svelte';
	import ModuleGraphic from './modules/ModuleGraphic.svelte';
	import ModuleWorkflow from './modules/ModuleWorkflow.svelte';
	import ModuleStateMachineGraph from './modules/ModuleStateMachineGraph.svelte';
	import ModuleHistoryBrowser from './modules/ModuleHistoryBrowser.svelte';
	import ModuleFallback from './modules/ModuleFallback.svelte';

	let { module, app }: { module: any; app: any } = $props();

	const componentMap: Record<string, any> = {
		paragraph: ModuleParagraph,
		content: ModuleContent,
		grid: ModuleGrid,
		form: ModuleForm,
		buttongroup: ModuleButtonGroup,
		image: ModuleImage,
		separator: ModuleSeparator,
		statusbanner: ModuleStatusBanner,
		tabs: ModuleTabs,
		parameterlist: ModuleParameterList,
		statuslist: ModuleStatusList,
		togglepanel: ModuleTogglePanel,
		logs: ModuleLogs,
		alertlist: ModuleAlertList,
		upload: ModuleUpload,
		buildlist: ModuleBuildList,
		executionlist: ModuleExecutionList,
		configurationlist: ModuleConfigurationList,
		chart: ModuleChart,
		videostream: ModuleVideoStream,
		layerview: ModuleLayerView,
		glscene: ModuleGLScene,
		graphic: ModuleGraphic,
		workflow: ModuleWorkflow,
		statemachinegraph: ModuleStateMachineGraph,
		journalhistory: ModuleHistoryBrowser,
	};

	let Component = $derived(componentMap[module?.type] || ModuleFallback);
</script>

{#if module}
	<Component {module} {app} />
{/if}
