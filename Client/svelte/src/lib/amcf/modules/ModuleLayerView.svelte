<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { usePollTick } from '$lib/amcf/poll.svelte';
	// @ts-ignore — core JS has no type declarations yet
	import WebGLImpl from '@core/common/AMCImplementation_WebGL.js';
	// @ts-ignore
	import LayerViewImpl from '@core/common/AMCImplementation_LayerView.js';

	const ZOOM_MARGIN = 10;

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let containerEl: HTMLDivElement | undefined = $state(undefined);
	let glInstance: any = $state(null);
	let layerViewer: any = $state(null);
	let initialized = $state(false);

	let platform = $derived.by(() => { poll.v; return module.platform || null; });
	let layerCount = $derived.by(() => { poll.v; return platform?.layercount || 0; });
	let sliderValue = $state(0);
	let appliedColorTheme = $state('');

	$effect(() => {
		poll.v;
		if (platform) {
			sliderValue = platform.currentlayer || 0;
		}
	});

	function getCurrentColorSet(): Record<string, string> | null {
		if (!platform) return null;
		const isDark = document.documentElement.classList.contains('dark');
		return isDark ? platform.darkcolors : platform.colors;
	}

	function getBuildPlateURL(): string | null {
		if (!platform || !app) return null;
		const isDark = document.documentElement.classList.contains('dark');
		if (isDark && platform.dark_baseimageresource) {
			return app.getImageURL(platform.dark_baseimageresource);
		}
		if (platform.baseimageresource) {
			return app.getImageURL(platform.baseimageresource);
		}
		return null;
	}

	$effect(() => {
		poll.v;
		if (!layerViewer || !platform || !initialized) return;
		const isDark = document.documentElement.classList.contains('dark');
		const themeKey = isDark ? 'dark' : 'light';
		if (themeKey !== appliedColorTheme) {
			appliedColorTheme = themeKey;
			const cs = getCurrentColorSet();
			if (cs) layerViewer.applyColors(cs);
			const plateURL = getBuildPlateURL();
			layerViewer.SetBuildPlateSVG(plateURL);
			layerViewer.RenderScene(true);
		}
	});

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

			layerViewer = new LayerViewImpl(glInstance);

			glInstance.setupDOMElement(containerEl);
			layerViewer.updateSize(w, h);

			if (platform) {
				const plateURL = getBuildPlateURL();
				if (plateURL) {
					layerViewer.SetBuildPlateSVG(plateURL);
				}
				layerViewer.setOrigin(platform.originx || 0, platform.originy || 0);
				layerViewer.CenterOnRectangle(
					-ZOOM_MARGIN, -ZOOM_MARGIN,
					(platform.sizex || 300) + ZOOM_MARGIN,
					(platform.sizey || 300) + ZOOM_MARGIN
				);

				platform.displayed_layer = 0;
				platform.displayed_build = 0;
			}

			const cs = getCurrentColorSet();
			if (cs) {
				layerViewer.applyColors(cs);
				const isDark = document.documentElement.classList.contains('dark');
				appliedColorTheme = isDark ? 'dark' : 'light';
			}

			layerViewer.RenderScene(true);
			initialized = true;
		} catch (e) {
			console.warn('[LayerView] init failed:', e);
		}
	}

	function onDataChanged(sender: any) {
		if (!layerViewer || !platform || !sender) return;
		if (!module.isActive || !module.isActive()) return;
		if (sender.uuid !== module.uuid) return;

		if (platform.displayed_layer !== platform.currentlayer ||
			platform.displayed_build !== platform.builduuid) {

			platform.displayed_layer = platform.currentlayer;
			platform.displayed_build = platform.builduuid;
			sliderValue = platform.currentlayer || 0;

			app.axiosPostRequest('/build/toolpath', {
				builduuid: platform.builduuid,
				layerindex: platform.currentlayer
			})
			.then((layerJSON: any) => {
				if (layerViewer) {
					layerViewer.loadLayer(layerJSON.data.segments);
					layerViewer.RenderScene(true);
				}
			})
			.catch((err: any) => {
				console.warn('[LayerView] layer load error:', err?.response || err);
				if (layerViewer) layerViewer.RenderScene(true);
			});
		}
	}

	function onSliderChange(e: Event) {
		const val = parseInt((e.target as HTMLInputElement).value);
		sliderValue = val;
		if (platform) {
			app.triggerWidgetRequest(platform.uuid, 'changelayer', { targetlayer: val });
		}
	}

	function onWheel(event: WheelEvent) {
		if (!containerEl || !layerViewer) return;
		event.preventDefault();

		let delta = event.deltaY;
		if (delta > 5) delta = 5;
		if (delta < -5) delta = -5;

		const box = containerEl.getBoundingClientRect();
		const localX = event.clientX - box.left;
		const localY = event.clientY - box.top;

		layerViewer.ScaleRelative(Math.pow(1.03, -delta * 1.5), localX, localY);
		layerViewer.RenderScene(true);
	}

	let dragging = false;
	let dragX = 0, dragY = 0;

	function onPointerDown(event: PointerEvent) {
		if (event.button === 0 || event.button === 1) {
			dragging = true;
			dragX = event.clientX;
			dragY = event.clientY;
			(event.target as HTMLElement).setPointerCapture(event.pointerId);
		}
	}

	function onPointerMove(event: PointerEvent) {
		if (!dragging || !layerViewer) return;
		const dx = event.clientX - dragX;
		const dy = event.clientY - dragY;
		dragX = event.clientX;
		dragY = event.clientY;
		layerViewer.Drag(dx, dy);
		layerViewer.RenderScene(true);
	}

	function onPointerUp() {
		dragging = false;
	}

	function resetView() {
		if (!layerViewer || !platform) return;
		layerViewer.CenterOnRectangle(
			-ZOOM_MARGIN, -ZOOM_MARGIN,
			(platform.sizex || 300) + ZOOM_MARGIN,
			(platform.sizey || 300) + ZOOM_MARGIN
		);
		layerViewer.RenderScene(true);
	}

	function fitToPath() {
		if (!layerViewer) return;
		try {
			const bounds = layerViewer.getPathBoundaries?.();
			if (bounds && bounds.radius > 0 && platform) {
				const left = bounds.center.x - bounds.radius + (platform.sizex || 300) / 2;
				const right = bounds.center.x + bounds.radius + (platform.sizex || 300) / 2;
				const top = bounds.center.y - bounds.radius + (platform.sizey || 300) / 2;
				const bottom = bounds.center.y + bounds.radius + (platform.sizey || 300) / 2;
				layerViewer.CenterOnRectangle(left, top, right, bottom);
			} else {
				resetView();
			}
			layerViewer.RenderScene(true);
		} catch { resetView(); }
	}

	onMount(() => {
		module.onDataHasChanged = onDataChanged;

		if (platform) {
			platform.displayed_layer = 0;
			platform.displayed_build = 0;
		}

		requestAnimationFrame(() => {
			ensureInit();
		});

		const ro = new ResizeObserver(() => {
			if (!initialized) {
				ensureInit();
			} else if (layerViewer && containerEl) {
				const w = containerEl.clientWidth, h = containerEl.clientHeight;
				if (w > 0 && h > 0) {
					layerViewer.updateSize(w, h);
					layerViewer.RenderScene(true);
				}
			}
		});
		if (containerEl) ro.observe(containerEl);

		return () => ro.disconnect();
	});

	onDestroy(() => {
		module.onDataHasChanged = null;
		if (platform) {
			platform.displayed_layer = 0;
			platform.displayed_build = 0;
		}
	});
</script>

{#if visible}
	<div class="layerview-container">
		<!-- WebGL render target — setupDOMElement sets position:relative on this -->
		<div
			bind:this={containerEl}
			class="layerview-canvas"
			role="img"
			onwheel={onWheel}
			onpointerdown={onPointerDown}
			onpointermove={onPointerMove}
			onpointerup={onPointerUp}
			onpointercancel={onPointerUp}
		></div>

		<!-- Overlaid toolbar -->
		<div class="layerview-toolbar">
			<button class="layerview-btn" onclick={resetView}>Reset View</button>
			<button class="layerview-btn" onclick={fitToPath}>Fit</button>
		</div>

		<!-- Layer info overlay -->
		{#if layerCount > 0}
			<div class="layerview-layer-info">
				Layer {sliderValue} / {layerCount}
			</div>
		{/if}

		<!-- Layer slider -->
		{#if layerCount > 0}
			<div class="layerview-slider-wrap">
				<input
					type="range"
					class="layerview-slider"
					min="0"
					max={layerCount}
					value={sliderValue}
					oninput={onSliderChange}
				/>
			</div>
		{/if}
	</div>
{/if}

<style>
	.layerview-container {
		position: relative;
		width: 100%;
		height: 100%;
		overflow: hidden;
	}
	.layerview-canvas {
		width: 100%;
		height: 100%;
	}
	.layerview-toolbar {
		position: absolute;
		top: 8px;
		left: 8px;
		display: flex;
		gap: 4px;
		z-index: 10;
	}
	.layerview-btn {
		padding: 4px 12px;
		border: none;
		border-radius: 4px;
		background: rgba(0, 0, 0, 0.65);
		color: white;
		font-size: 12px;
		cursor: pointer;
		transition: background-color 0.2s;
	}
	.layerview-btn:hover {
		background-color: rgba(0, 0, 0, 0.85);
	}
	.layerview-layer-info {
		position: absolute;
		top: 8px;
		right: 8px;
		padding: 4px 10px;
		border-radius: 4px;
		background: rgba(0, 0, 0, 0.65);
		color: white;
		font-size: 11px;
		font-variant-numeric: tabular-nums;
		z-index: 10;
	}
	.layerview-slider-wrap {
		position: absolute;
		bottom: 8px;
		left: 8px;
		right: 8px;
		z-index: 10;
	}
	.layerview-slider {
		width: 100%;
		accent-color: var(--primary, #2563eb);
	}
</style>
