<script lang="ts">
	import ModuleFactory from '../ModuleFactory.svelte';
	import * as Card from '$lib/components/ui/card/index.js';
	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let isCard = $derived.by(() => {
		poll.v;
		return module.cardstyle === 'elevated' ||
			module.cardstyle === 'outlined' ||
			module.cardstyle === 'tinted';
	});
	let isGroupbox = $derived.by(() => { poll.v; return module.cardstyle === 'groupbox'; });
	let title = $derived.by(() => { poll.v; return module.title || ''; });
	let subtitle = $derived.by(() => { poll.v; return module.subtitle || ''; });
	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let modules = $derived.by(() => { poll.v; return [...(module.modules || [])]; });
</script>

{#if visible && isGroupbox}
	<fieldset class="relative mt-2.5 rounded-lg border border-border px-3.5 pb-3 pt-4 bg-card">
		{#if title}
			<legend class="px-2 text-sm font-semibold">{title}</legend>
		{/if}
		{#if subtitle}
			<p class="text-sm text-muted-foreground mb-2">{subtitle}</p>
		{/if}
		<div class="flex flex-col gap-2 w-full">
			{#each modules as child (child.uuid)}
				<ModuleFactory module={child} {app} />
			{/each}
		</div>
	</fieldset>
{:else if visible && isCard}
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
			{#each modules as child (child.uuid)}
				<ModuleFactory module={child} {app} />
			{/each}
		</Card.Content>
	</Card.Root>
{:else if visible}
	<div class="flex flex-col h-full w-full gap-1">
		{#if title}
			<h3 class="text-base font-semibold text-foreground px-1">{title}</h3>
		{/if}
		{#if subtitle}
			<p class="text-sm text-muted-foreground px-1">{subtitle}</p>
		{/if}
		<div class="flex-1 min-h-0 flex flex-col gap-2 w-full">
			{#each modules as child (child.uuid)}
				<ModuleFactory module={child} {app} />
			{/each}
		</div>
	</div>
{/if}
