<script lang="ts">
	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let imageURL = $derived.by(() => {
		poll.v;
		return module.imageresourceuuid && app
			? app.getImageURL(module.imageresourceuuid)
			: '';
	});
	let maxheight = $derived.by(() => { poll.v; return module.maxheight || 400; });
	let aspectratio = $derived.by(() => { poll.v; return module.aspectratio || 'auto'; });
</script>

{#if visible && imageURL}
	<div class="w-full flex justify-center">
		<img
			src={imageURL}
			alt=""
			class="object-contain max-w-full"
			style="max-height: {maxheight}px; aspect-ratio: {aspectratio}"
		/>
	</div>
{/if}
