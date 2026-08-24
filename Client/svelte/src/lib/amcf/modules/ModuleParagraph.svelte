<script lang="ts">
	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module }: { module: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let text = $derived.by(() => { poll.v; return module.text || ''; });
	let variant = $derived.by(() => { poll.v; return module.variant || 'body'; });

	const variantClasses: Record<string, string> = {
		h1: 'text-2xl font-bold text-foreground',
		h2: 'text-base font-semibold text-foreground',
		body: 'text-sm text-muted-foreground',
		label: 'text-xs font-semibold text-muted-foreground uppercase tracking-wide',
		subtext: 'text-xs text-muted-foreground/70',
		code: 'text-xs font-mono bg-muted px-2 py-1 rounded-sm border',
	};

	let cls = $derived(variantClasses[variant] || variantClasses.body);
</script>

{#if visible}
	<p class={cls}>{text}</p>
{/if}
