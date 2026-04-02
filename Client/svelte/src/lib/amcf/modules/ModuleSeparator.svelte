<script lang="ts">
	import { Separator } from '$lib/components/ui/separator/index.js';
	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module }: { module: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let orientation = $derived.by(() => {
		poll.v;
		return module.orientation === 'vertical' ? 'vertical' as const : 'horizontal' as const;
	});
</script>

{#if visible}
	<Separator {orientation} class="my-2" />
{/if}
