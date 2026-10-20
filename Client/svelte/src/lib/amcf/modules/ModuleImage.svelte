<script lang="ts">
	import { useModuleTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = useModuleTick(() => module);
	const nullUUID = '00000000-0000-0000-0000-000000000000';

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let imageURL = $derived.by(() => {
		poll.v;
		const uuid = module.imageresource;
		if (!uuid || uuid === nullUUID || !app) return '';
		return app.getImageURL(uuid);
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
