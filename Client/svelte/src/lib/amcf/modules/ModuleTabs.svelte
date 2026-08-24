<script lang="ts">
	import ModuleFactory from '../ModuleFactory.svelte';
	import * as Tabs from '$lib/components/ui/tabs/index.js';
	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let tabs = $derived.by(() => { poll.v; return [...(module.tabs || [])]; });
	let activeTab = $state('');

	$effect(() => {
		if (activeTab === '' && tabs.length > 0) {
			activeTab = tabs[0].uuid;
		}
	});
</script>

{#if visible && tabs.length > 0}
	<Tabs.Root bind:value={activeTab} class="flex flex-col h-full min-h-0 w-full">
		<Tabs.List>
			{#each tabs as tab (tab.uuid)}
				<Tabs.Trigger value={tab.uuid}>{tab.caption || tab.name}</Tabs.Trigger>
			{/each}
		</Tabs.List>
		{#each tabs as tab (tab.uuid)}
			<Tabs.Content value={tab.uuid} class="flex-1 min-h-0 overflow-auto flex flex-col pt-2">
				<ModuleFactory module={tab} {app} />
			</Tabs.Content>
		{/each}
	</Tabs.Root>
{/if}
