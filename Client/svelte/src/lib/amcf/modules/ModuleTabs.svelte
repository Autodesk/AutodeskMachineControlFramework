<script lang="ts">
	import ModuleFactory from '../ModuleFactory.svelte';
	import DisplayScope from '../DisplayScope.svelte';
	import * as Tabs from '$lib/components/ui/tabs/index.js';
	import * as Card from '$lib/components/ui/card/index.js';
	import { useModuleTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = useModuleTick(() => module);

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	// Only show tabs whose (dynamic) visibility is not explicitly false. This lets
	// a tab be shown/hidden at runtime via a sync:visible expression in the config.
	let tabs = $derived.by(() => {
		poll.v;
		return [...(module.tabs || [])].filter((t: any) => t.visible !== false);
	});
	let cardstyle = $derived.by(() => { poll.v; return module.cardstyle || 'none'; });
	let isCard = $derived(cardstyle === 'elevated' || cardstyle === 'outlined' || cardstyle === 'tinted');
	let title = $derived.by(() => { poll.v; return module.title || ''; });
	let subtitle = $derived.by(() => { poll.v; return module.subtitle || ''; });
	let activeTab = $state('');

	// Keep the active tab valid: pick the first visible tab initially and whenever
	// the currently-active tab becomes hidden (e.g. it was replaced by another).
	$effect(() => {
		if (tabs.length === 0) return;
		if (!tabs.some((t: any) => t.uuid === activeTab)) {
			activeTab = tabs[0].uuid;
		}
	});
</script>

{#snippet tabsBody()}
	<Tabs.Root bind:value={activeTab} class="flex flex-col h-full min-h-0 w-full">
		<Tabs.List>
			{#each tabs as tab (tab.uuid)}
				<Tabs.Trigger value={tab.uuid}>{tab.caption || tab.name}</Tabs.Trigger>
			{/each}
		</Tabs.List>
		{#each tabs as tab (tab.uuid)}
			<Tabs.Content value={tab.uuid} class="flex-1 min-h-0 overflow-auto flex flex-col pt-2">
				<!-- Inactive tab content stays mounted (only hidden), so tell nested modules whether they are shown. -->
				<DisplayScope active={activeTab === tab.uuid}>
					<ModuleFactory module={tab} {app} />
				</DisplayScope>
			</Tabs.Content>
		{/each}
	</Tabs.Root>
{/snippet}

{#if visible && tabs.length > 0}
	{#if isCard}
		<Card.Root class="flex flex-col h-full min-h-0">
			{#if title}
				<Card.Header class="pb-1">
					<Card.Title>{title}</Card.Title>
					{#if subtitle}
						<Card.Description>{subtitle}</Card.Description>
					{/if}
				</Card.Header>
			{/if}
			<Card.Content class="flex-1 min-h-0 overflow-auto flex flex-col gap-2">
				{@render tabsBody()}
			</Card.Content>
		</Card.Root>
	{:else}
		{@render tabsBody()}
	{/if}
{/if}
