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
	let animFrameId: number = 0;
	let loadedMeshKeys = new Set<string>();

	function ensureInit() {
		if (initialized || !containerEl || !app) return;
		const w = containerEl.clientWidth, h = containerEl.clientHeight;
		if (w === 0 || h === 0) return;

		try {
			glInstance = app.retrieveWebGLInstance(module.uuid);
			if (!glInstance) {
				glInstance = new WebGLImpl();
				app.storeWebGLInstance(module.uuid, glInstance);
			}

			glInstance.setupPerspectiveView(45, w / h, 1, 3000);
			glInstance.setCameraPosition(150, 150, 150);
			glInstance.resizeTo(w, h);
			glInstance.setupDOMElement(containerEl);
			glInstance.addAmbientLight('ambientlight', 0x808080, 3);
			glInstance.setupDemoScene();

			initialized = true;
			animate();
			syncInstances();
		} catch (e) {
			console.warn('[GLScene] WebGL init failed:', e);
		}
	}

	function animate() {
		animFrameId = requestAnimationFrame(animate);
		if (glInstance) glInstance.renderScene();
	}

	function syncInstances() {
		if (!glInstance || !module.scene?.instances) return;

		const currentKeys = new Set<string>();
		for (const inst of module.scene.instances) {
			const key = inst.instancename;
			currentKeys.add(key);

			if (!loadedMeshKeys.has(key)) {
				try {
					glInstance.addMeshElement(key, app, inst.meshuuid);
					loadedMeshKeys.add(key);
				} catch {}
			} else {
				try {
					glInstance.updateMeshElement(key, inst);
				} catch {}
			}
		}

		for (const key of loadedMeshKeys) {
			if (!currentKeys.has(key)) {
				try { glInstance.removeElement(key); } catch {}
				loadedMeshKeys.delete(key);
			}
		}
	}

	onMount(() => {
		if (!containerEl) return;

		ensureInit();

		module.onDataHasChanged = () => syncInstances();

		const ro = new ResizeObserver(() => {
			if (!initialized) {
				ensureInit();
			} else {
				try {
					const w = containerEl!.clientWidth, h = containerEl!.clientHeight;
					if (w > 0 && h > 0) glInstance?.resizeTo(w, h);
				} catch {}
			}
		});
		ro.observe(containerEl);

		return () => ro.disconnect();
	});

	onDestroy(() => {
		module.onDataHasChanged = null;
		if (animFrameId) cancelAnimationFrame(animFrameId);
	});
</script>

{#if visible}
	<div class="w-full border rounded-md overflow-hidden relative" style="min-height: 400px;">
		<div bind:this={containerEl} class="w-full h-full absolute inset-0"></div>
		{#if !initialized}
			<div class="absolute inset-0 flex items-center justify-center text-sm text-muted-foreground bg-muted">
				Initializing scene...
			</div>
		{/if}
	</div>
{/if}
