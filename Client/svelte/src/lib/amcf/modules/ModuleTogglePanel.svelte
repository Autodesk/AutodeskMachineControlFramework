<script lang="ts">
	import { Switch } from '$lib/components/ui/switch/index.js';
	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let columns = $derived.by(() => { poll.v; return module.columns || 3; });
	let toggles = $derived.by(() => { poll.v; return [...(module.toggles || [])]; });

	function isTruthy(value: string): boolean {
		const v = (value ?? '').toString().trim().toLowerCase();
		return !(v === '' || v === '0' || v === 'false' || v === 'off' || v === 'no');
	}

	function stateLabel(toggle: any): string {
		const on = isTruthy(toggle.value);
		switch (toggle.mode) {
			case 'yesno': return on ? 'Yes' : 'No';
			case 'openclosed': return on ? 'Opened' : 'Closed';
			case 'leftright': return on ? 'Right' : 'Left';
			default: return on ? 'On' : 'Off';
		}
	}

	function onToggle(toggle: any) {
		if (!app || !toggle.event) return;
		// Send the intended new state explicitly. Every toggle in a panel resolves to
		// the same sender path, so the handler cannot read a per-toggle value; passing
		// it as an event parameter ("value") makes the handler reliable (the old
		// inversion-of-read-back approach failed for e.g. driver-backed laser signals).
		const newState = !isTruthy(toggle.value);
		app.triggerUIEvent(toggle.event, toggle.uuid, {}, undefined, { value: newState ? '1' : '0' });
	}
</script>

{#if visible}
	<div class="grid gap-2 w-full" style="grid-template-columns: repeat({columns}, minmax(0, 1fr));">
		{#each toggles as toggle (toggle.uuid)}
			<div class="flex items-center justify-between gap-2 rounded-md border border-border/60 px-3 py-2">
				<div class="flex flex-col min-w-0">
					<span class="text-sm truncate">{toggle.label}</span>
					<span class="text-xs text-muted-foreground">{stateLabel(toggle)}</span>
				</div>
				<Switch
					checked={isTruthy(toggle.value)}
					onCheckedChange={() => onToggle(toggle)}
				/>
			</div>
		{/each}
	</div>
{/if}
