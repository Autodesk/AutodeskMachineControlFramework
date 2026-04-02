<script lang="ts">
	import ModuleFactory from '../ModuleFactory.svelte';
	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let cssstyle = $derived.by(() => { poll.v; return module.cssstyle || ''; });
	let sections = $derived.by(() => { poll.v; return [...(module.sections || [])]; });
</script>

{#if visible}
	<div class="w-full h-full" style={cssstyle}>
		{#each sections as section (section.uuid)}
			<div style="min-height:0;height:100%;{section.cssstyle || ''}">
				<ModuleFactory module={section} {app} />
			</div>
		{/each}
	</div>
{/if}
