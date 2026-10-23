<script lang="ts">
	import { Separator } from '$lib/components/ui/separator/index.js';
	import { useModuleTick } from '$lib/amcf/poll.svelte';

	let { module }: { module: any } = $props();
	const poll = useModuleTick(() => module);

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let orientation = $derived.by(() => {
		poll.v;
		return module.orientation === 'vertical' ? 'vertical' as const : 'horizontal' as const;
	});
</script>

{#if visible}
	<Separator {orientation} class="my-2" />
{/if}
