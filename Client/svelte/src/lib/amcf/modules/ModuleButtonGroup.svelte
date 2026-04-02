<script lang="ts">
	import { Button } from '$lib/components/ui/button/index.js';

	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let cssstyle = $derived.by(() => { poll.v; return module.cssstyle || ''; });
	let buttoncssstyle = $derived.by(() => { poll.v; return module.buttoncssstyle || ''; });
	let buttons = $derived.by(() => { poll.v; return [...(module.buttons || [])]; });

	function handleClick (btn: any) {
		if (!app) return;
		if (btn.event) {
			const formvalues = (btn.eventformvalues && app.assembleFormValues)
				? app.assembleFormValues(btn.eventformvalues)
				: {};
			app.triggerUIEvent(btn.event, btn.uuid, formvalues);
		}
		if (btn.targetpage)
			app.changePage(btn.targetpage);
	}
</script>

{#if visible}
	<div class="flex gap-2 w-full" style={cssstyle}>
		{#each buttons as btn (btn.uuid)}
			<Button
				variant="outline"
				style={buttoncssstyle}
				disabled={btn.disabled}
				onclick={() => handleClick(btn)}
			>
				{btn.caption}
			</Button>
		{/each}
	</div>
{/if}
