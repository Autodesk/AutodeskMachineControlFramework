<script lang="ts">
	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let streamURL = $derived.by(() => {
		poll.v;
		return module.streamresource && app
			? app.getStreamURL(module.streamresource)
			: '';
	});

	let imgStyle = $derived.by(() => {
		poll.v;
		let s = '';
		if (module.maxwidth > 0) s += `max-width: ${module.maxwidth}px; `;
		if (module.maxheight > 0) s += `max-height: ${module.maxheight}px; `;
		return s;
	});
</script>

{#if visible}
	<div class="w-full flex justify-center">
		{#if streamURL}
			<img
				src={streamURL}
				alt={module.caption || 'Video Stream'}
				class="rounded border object-contain"
				style={imgStyle}
			/>
		{:else}
			<div class="w-full h-48 bg-muted rounded border flex items-center justify-center text-sm text-muted-foreground">
				No stream available.
			</div>
		{/if}
	</div>
{/if}
