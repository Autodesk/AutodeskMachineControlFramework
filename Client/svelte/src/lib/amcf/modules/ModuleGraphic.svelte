<script lang="ts">
	import { onMount, onDestroy } from 'svelte';

	import { usePollTick } from '$lib/amcf/poll.svelte';
	// @ts-ignore — core JS has no type declarations yet
	import WebGLImpl from '@core/common/AMCImplementation_WebGL.js';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let containerEl: HTMLDivElement | undefined = $state(undefined);
	let glInstance: any = $state(null);
	let initialized = $state(false);

	onMount(() => {
		if (!containerEl || !app) return;

		try {
			glInstance = app.retrieveWebGLInstance(module.uuid);
			if (!glInstance) {
				glInstance = new WebGLImpl();
				app.storeWebGLInstance(module.uuid, glInstance);
			}
			if (glInstance && containerEl) {
				glInstance.setupDOMElement(containerEl);
				glInstance.setupOrthographicView();
				initialized = true;
			}
		} catch (e) {
			console.warn('[Graphic] WebGL init failed:', e);
		}

		if (initialized && module.items) {
			for (const item of module.items) {
				try {
					const imageURL = app.getImageURL(item.imageuuid);
					glInstance.AddSVGElement(imageURL, item.name, item.z || 0);
					glInstance.SetElementTranslation(item.name, item.x || 0, item.y || 0);
					glInstance.SetElementScaling(item.name, item.scalex || 1, item.scaley || 1);
				} catch {}
			}
		}

		module.onDataHasChanged = () => {
			if (!glInstance || !module.items) return;
			for (const item of module.items) {
				try {
					glInstance.SetElementTranslation(item.name, item.x || 0, item.y || 0);
					glInstance.SetElementScaling(item.name, item.scalex || 1, item.scaley || 1);
				} catch {}
			}
		};

		const ro = new ResizeObserver(() => {
			try { glInstance?.resize(); } catch {}
		});
		ro.observe(containerEl);

		return () => ro.disconnect();
	});

	onDestroy(() => {
		module.onDataHasChanged = null;
	});
</script>

{#if visible}
	<div class="w-full border rounded-md overflow-hidden relative" style="min-height: 300px;">
		<div bind:this={containerEl} class="w-full h-full absolute inset-0"></div>
		{#if !initialized}
			<div class="absolute inset-0 flex items-center justify-center text-sm text-muted-foreground bg-muted">
				Graphic view requires WebGL.
			</div>
		{/if}
	</div>
{/if}
