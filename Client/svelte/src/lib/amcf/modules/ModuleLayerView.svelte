<script lang="ts">
	import { onMount, onDestroy, untrack } from 'svelte';
	import { useModuleTick } from '$lib/amcf/poll.svelte';
	import * as Card from '$lib/components/ui/card/index.js';
	import Square from '@lucide/svelte/icons/square';
	import Shapes from '@lucide/svelte/icons/shapes';
	import ZoomIn from '@lucide/svelte/icons/zoom-in';
	import Axis3d from '@lucide/svelte/icons/axis-3d';
	import Tags from '@lucide/svelte/icons/tags';
	import Info from '@lucide/svelte/icons/info';
	import Palette from '@lucide/svelte/icons/palette';
	import ZapOff from '@lucide/svelte/icons/zap-off';
	import Route from '@lucide/svelte/icons/route';
	// @ts-ignore — core JS has no type declarations yet
	import WebGLImpl from '@core/common/AMCImplementation_WebGL.js';
	// @ts-ignore
	import LayerViewImpl from '@core/common/AMCImplementation_LayerView.js';

	const ZOOM_MARGIN = 10;
	// Smaller selections are treated as accidental clicks and do not zoom.
	const MIN_ZOOM_SELECTION_PX = 5;
	const PART_MARKER_PX = 5;
	const NULL_UUID = '00000000-0000-0000-0000-000000000000';
	const UUID_PATTERN = /^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$/i;
	// Scatter plot channel and the columns of it that LayerViewImpl can colour by.
	const POINT_CHANNEL = 'laser';
	const POINT_CHANNEL_COLUMNS = ['laseron', 'power'];
	const POINT_COLOR_MODES = [
		{ mode: 'unicolor', caption: 'Uniform', column: '' },
		{ mode: 'time', caption: 'Timing', column: '' },
		{ mode: 'velocity', caption: 'Velocity', column: '' },
		{ mode: 'laseron', caption: 'Laser On', column: 'laseron' },
		{ mode: 'powerramp', caption: 'Power', column: 'power' }
	];

	let { module, app }: { module: any; app: any } = $props();
	const poll = useModuleTick(() => module);

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let cardstyle = $derived.by(() => { poll.v; return module.cardstyle || 'none'; });
	let isCard = $derived(cardstyle === 'elevated' || cardstyle === 'outlined' || cardstyle === 'tinted');
	let cardTitle = $derived.by(() => { poll.v; return module.title || ''; });
	let cardSubtitle = $derived.by(() => { poll.v; return module.subtitle || ''; });
	let containerEl: HTMLDivElement | undefined = $state(undefined);
	let glInstance: any = $state(null);
	let layerViewer: any = $state(null);
	let initialized = $state(false);

	type CoordinateTransform = readonly [
		angleDegrees: number,
		rotationCenterX: number,
		rotationCenterY: number,
		translationX: number,
		translationY: number
	];
	let platform = $derived.by(() => { poll.v; return module.platform || null; });
	let layerCount = $derived.by(() => { poll.v; return platform?.layercount || 0; });
	let transformAngle = $derived.by(() => { poll.v; return Number(platform?.transformangle) || 0; });
	let rotationCenterX = $derived.by(() => { poll.v; return Number(platform?.rotationcenterx) || 0; });
	let rotationCenterY = $derived.by(() => { poll.v; return Number(platform?.rotationcentery) || 0; });
	let translationX = $derived.by(() => { poll.v; return Number(platform?.translationx) || 0; });
	let translationY = $derived.by(() => { poll.v; return Number(platform?.translationy) || 0; });
	let coordinateTransform = $derived([
		transformAngle,
		rotationCenterX,
		rotationCenterY,
		translationX,
		translationY
	] as CoordinateTransform);
	let labelCaption = $derived.by(() => {
		poll.v;
		return platform?.labelvisible ? String(platform.labelcaption || '') : '';
	});

	// Scatter plot (platform.scatterplotuuid) drawn as points on top of the toolpath.
	let pointsLoaded = $state(false);
	let pointColumns = $state.raw<string[]>([]);
	let pointColorMode = $state('unicolor');
	let showLaserOffPoints = $state(false);
	let toolpathVisible = $state(true);
	let pointLoadCounter = 0;
	let availableColorModes = $derived(
		POINT_COLOR_MODES.filter((entry) => entry.column === '' || pointColumns.includes(entry.column))
	);
	let pointColorCaption = $derived(
		POINT_COLOR_MODES.find((entry) => entry.mode === pointColorMode)?.caption ?? 'Uniform'
	);

	let sliderValue = $state(0);
	let appliedColorTheme = $state('');
	let coordinateSystemOverride: boolean | null = $state(null);
	let coordinateSystemVisible = $derived.by(() => {
		poll.v;
		return coordinateSystemOverride ?? Boolean(platform?.showcoordinatesystem);
	});
	let appliedCoordinateTransform: CoordinateTransform | null = null;
	// While true, the view keeps framing the platform whenever the viewport or the
	// platform geometry changes. Cleared once the user pans or zooms; set again by
	// the "Zoom to Platform" button.
	let autoFrame = true;
	let platformFrameKey = $derived.by(() => {
		poll.v;
		return platform
			? [platform.sizex, platform.sizey, platform.originx, platform.originy, platform.paddingx, platform.paddingy].join('|')
			: '';
	});

	$effect(() => {
		platformFrameKey;
		if (!initialized || !autoFrame) return;
		untrack(() => resetView());
	});

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

	$effect(() => {
		poll.v;
		if (!layerViewer || !initialized) return;

		if (coordinateTransform === appliedCoordinateTransform) return;

		layerViewer.setCoordinateTransform(...coordinateTransform);
		appliedCoordinateTransform = coordinateTransform;
		layerViewer.RenderScene(true);
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
			// Write-only, so effects that pan or zoom do not subscribe to viewVersion.
			layerViewer.onTransformChanged = () => { viewVersion = ++transformChangeCount; };
			layerViewer.toolpathVisible = toolpathVisible;
			layerViewer.showLaserOffPoints = showLaserOffPoints;

			glInstance.setupDOMElement(containerEl);
			layerViewer.updateSize(w, h);
			layerViewer.setCoordinateTransform(...coordinateTransform);
			appliedCoordinateTransform = coordinateTransform;

			if (platform) {
				const plateURL = getBuildPlateURL();
				if (plateURL) {
					layerViewer.SetBuildPlateSVG(plateURL);
				}
				layerViewer.setOrigin(platform.originx || 0, platform.originy || 0);
				centerOnPlatform();

				platform.displayed_layer = 0;
				platform.displayed_build = 0;
				platform.displayed_scatterplot = '';
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
			platform.displayed_build !== platform.builduuid ||
			platform.displayed_partstateversion !== platform.partstateversion) {

			platform.displayed_layer = platform.currentlayer;
			platform.displayed_build = platform.builduuid;
			platform.displayed_partstateversion = platform.partstateversion;
			sliderValue = platform.currentlayer || 0;

			app.axiosPostRequest('/build/toolpath', {
				builduuid: platform.builduuid,
				layerindex: platform.currentlayer
			})
			.then((layerJSON: any) => {
				if (layerViewer) {
					layerViewer.loadLayer(layerJSON.data.segments, layerJSON.data.parts);
					layerViewer.RenderScene(true);
				}
			})
			.catch((err: any) => {
				console.warn('[LayerView] layer load error:', err?.response || err);
				if (layerViewer) layerViewer.RenderScene(true);
			});
		}

		const scatterPlotUUID = typeof platform.scatterplotuuid === 'string' ? platform.scatterplotuuid.toLowerCase() : NULL_UUID;
		if (platform.displayed_scatterplot !== scatterPlotUUID) {
			platform.displayed_scatterplot = scatterPlotUUID;
			loadScatterPlot(scatterPlotUUID);
		}
	}

	function clearScatterPlot() {
		pointsLoaded = false;
		pointColumns = [];
		if (!layerViewer) return;
		layerViewer.clearPointsChannelData(POINT_CHANNEL);
		layerViewer.clearPoints();
		layerViewer.RenderScene(true);
	}

	// Returns the colourable columns of a /pointchanneldata response; header fields are skipped.
	function parsePointChannelData(response: any): [string, Float32Array][] {
		const contentType = String(response?.headers?.['content-type'] || '');
		if (!contentType.includes('application/json')) return [];

		const parsed = JSON.parse(new TextDecoder().decode(response.data));
		if (!parsed || typeof parsed !== 'object') return [];

		const columns: [string, Float32Array][] = [];
		for (const column of POINT_CHANNEL_COLUMNS) {
			const values = parsed[column];
			if (Array.isArray(values) && values.every((value) => typeof value === 'number')) {
				columns.push([column, new Float32Array(values)]);
			}
		}
		return columns;
	}

	async function loadScatterPlot(scatterPlotUUID: string) {
		const loadIndex = ++pointLoadCounter;
		clearScatterPlot();
		if (!layerViewer || scatterPlotUUID === NULL_UUID || !UUID_PATTERN.test(scatterPlotUUID)) return;

		try {
			const pointsResponse = await app.axiosGetArrayBufferRequest('/pointcloud/' + scatterPlotUUID);
			let channelColumns: [string, Float32Array][] = [];
			try {
				const channelResponse = await app.axiosGetArrayBufferRequest('/pointchanneldata/' + scatterPlotUUID + '/' + POINT_CHANNEL);
				channelColumns = parsePointChannelData(channelResponse);
			} catch (err: any) {
				console.warn('[LayerView] point channel data error:', err?.response || err);
			}

			if (loadIndex !== pointLoadCounter || !layerViewer) return;

			layerViewer.loadPoints(new Float32Array(pointsResponse.data));
			for (const [column, values] of channelColumns) {
				layerViewer.loadPointsChannelData(POINT_CHANNEL, column, values);
			}
			pointColumns = channelColumns.map(([column]) => column);
			pointsLoaded = true;

			if (!availableColorModes.some((entry) => entry.mode === pointColorMode)) {
				pointColorMode = 'unicolor';
			}
			layerViewer.showLaserOffPoints = showLaserOffPoints;
			layerViewer.setColorMode(pointColorMode);
		} catch (err: any) {
			console.warn('[LayerView] point cloud error:', err?.response || err);
			if (layerViewer) layerViewer.RenderScene(true);
		}
	}

	function cyclePointColorMode() {
		if (!layerViewer) return;
		const index = availableColorModes.findIndex((entry) => entry.mode === pointColorMode);
		pointColorMode = availableColorModes[(index + 1) % availableColorModes.length].mode;
		layerViewer.setColorMode(pointColorMode);
	}

	function toggleLaserOffPoints() {
		showLaserOffPoints = !showLaserOffPoints;
		if (!layerViewer) return;
		layerViewer.showLaserOffPoints = showLaserOffPoints;
		layerViewer.updateLayerPoints();
		layerViewer.RenderScene(true);
	}

	function toggleToolpath() {
		toolpathVisible = !toolpathVisible;
		if (!layerViewer) return;
		layerViewer.toolpathVisible = toolpathVisible;
		layerViewer.updateLoadedLayer();
		layerViewer.updateTransform();
		layerViewer.RenderScene(true);
	}

	function changeLayer(layer: number) {
		sliderValue = layer;
		if (platform) {
			app.triggerWidgetRequest(platform.uuid, 'changelayer', { targetlayer: layer });
		}
	}

	function onSliderChange(e: Event) {
		changeLayer(parseInt((e.target as HTMLInputElement).value));
	}

	// Clicking the "Layer x / n" badge swaps it for a number field; Enter jumps to the typed
	// layer (clamped to the slider range), Escape or leaving the field cancels.
	let layerJumpOpen = $state(false);
	let layerJumpValue = $state<number | null>(null);

	function openLayerJump() {
		layerJumpValue = sliderValue;
		layerJumpOpen = true;
	}

	function commitLayerJump() {
		if (!layerJumpOpen) return;
		layerJumpOpen = false;
		if (typeof layerJumpValue !== 'number' || !Number.isFinite(layerJumpValue)) return;
		changeLayer(Math.min(Math.max(Math.round(layerJumpValue), 0), layerCount));
	}

	function onLayerJumpKeyDown(event: KeyboardEvent) {
		if (event.key === 'Enter') {
			event.preventDefault();
			commitLayerJump();
		} else if (event.key === 'Escape') {
			event.preventDefault();
			event.stopPropagation();
			layerJumpOpen = false;
		}
	}

	function focusAndSelect(node: HTMLInputElement) {
		node.focus();
		node.select();
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

		autoFrame = false;
		layerViewer.ScaleRelative(Math.pow(1.03, -delta * 1.5), localX, localY);
		layerViewer.RenderScene(true);
	}

	let dragging = false;
	let dragX = 0, dragY = 0;

	// "Custom Zoom": while active, a left-button drag draws a selection rectangle
	// (viewport pixels) instead of panning; releasing it frames that rectangle.
	let zoomSelectMode = $state(false);
	let zoomSelection = $state<{ startX: number; startY: number; endX: number; endY: number } | null>(null);
	let zoomRect = $derived(zoomSelection ? {
		left: Math.min(zoomSelection.startX, zoomSelection.endX),
		top: Math.min(zoomSelection.startY, zoomSelection.endY),
		width: Math.abs(zoomSelection.endX - zoomSelection.startX),
		height: Math.abs(zoomSelection.endY - zoomSelection.startY)
	} : null);
	// Live machine/build-plate coordinates under the cursor (mm), shown bottom-right.
	let mousePosition = $state<{ x: number; y: number } | null>(null);

	// "Properties" hover inspector: when enabled, hovering a hatch/polyline shows
	// a popup with that segment's laser power, speed, profile, etc.
	type SegmentProperties = {
		type?: string;
		laserpower?: number;
		laserspeed?: number;
		profilename?: string;
		partid?: number;
		partname?: string;
		partdisabled?: boolean;
		laserindex?: number;
		lineIndex?: number;
	};
	let propertiesMode = $state(false);
	let hoverSegment = $state<SegmentProperties | null>(null);
	let hoverScreen = $state<{ x: number; y: number }>({ x: 0, y: 0 });
	let pendingHover: { x: number; y: number } | null = null;
	let hoverRAF = 0;

	// "Names": outlines every part of the current layer with its name. The part
	// given by the per-session platform property highlightpartuuid is always
	// outlined and emphasized, even while the toggle is off.
	type PartOverlay = {
		key: string;
		label: string;
		left: number;
		top: number;
		width: number;
		height: number;
		labelY: number;
		markers: { x: number; y: number }[];
		highlighted: boolean;
		disabled: boolean;
	};
	let namesMode = $state(false);
	let viewVersion = $state(0);
	let transformChangeCount = 0;
	let highlightPartUUID = $derived.by(() => {
		poll.v;
		const uuid = typeof platform?.highlightpartuuid === 'string' ? platform.highlightpartuuid.toLowerCase() : '';
		return uuid === NULL_UUID ? '' : uuid;
	});
	let partOverlays = $derived.by((): PartOverlay[] => {
		viewVersion;
		if (!layerViewer || (!namesMode && !highlightPartUUID)) return [];

		const overlays: PartOverlay[] = [];
		for (const box of layerViewer.getPartBoundingBoxes()) {
			const highlighted = highlightPartUUID !== '' && box.uuid === highlightPartUUID;
			if (!namesMode && !highlighted) continue;

			const topLeft = layerViewer.machineToScreen(box.minx, box.maxy);
			const bottomRight = layerViewer.machineToScreen(box.maxx, box.miny);
			if (!topLeft || !bottomRight) continue;

			const left = topLeft.x;
			const top = topLeft.y;
			const right = bottomRight.x;
			const bottom = bottomRight.y;
			const name = box.name || box.uuid.slice(0, 8);
			overlays.push({
				key: box.key,
				label: box.disabled ? `${name} (disabled)` : name,
				left,
				top,
				width: Math.max(right - left, 1),
				height: Math.max(bottom - top, 1),
				// Keep the label readable when the box touches the top edge.
				labelY: top > 14 ? top - 5 : top + 12,
				markers: [
					{ x: left, y: top },
					{ x: right, y: top },
					{ x: left, y: bottom },
					{ x: right, y: bottom },
					{ x: (left + right) / 2, y: (top + bottom) / 2 }
				],
				highlighted,
				disabled: box.disabled === true
			});
		}
		// Draw the highlighted part last so it stays on top of overlapping boxes.
		return overlays.sort((a, b) => Number(a.highlighted) - Number(b.highlighted));
	});

	function updateMousePosition(event: PointerEvent) {
		if (!containerEl || !layerViewer || typeof layerViewer.screenToMachine !== 'function') return;
		const box = containerEl.getBoundingClientRect();
		mousePosition = layerViewer.screenToMachine(event.clientX - box.left, event.clientY - box.top);
	}

	function updateHoverSegment(clientX: number, clientY: number) {
		if (!propertiesMode || dragging || !containerEl || !layerViewer ||
			typeof layerViewer.pickSegmentAtScreenPoint !== 'function') {
			hoverSegment = null;
			return;
		}
		const box = containerEl.getBoundingClientRect();
		const localX = clientX - box.left;
		const localY = clientY - box.top;
		const result: SegmentProperties | null = layerViewer.pickSegmentAtScreenPoint(localX, localY, 6) ?? null;
		hoverSegment = result;
		hoverScreen = { x: localX, y: localY };

		if (typeof layerViewer.setHighlightLine === 'function') {
			layerViewer.setHighlightLine(result ? (result.lineIndex ?? -1) : -1);
		}
	}

	// Segment picking scans every line of the layer, so throttle it to one hit
	// test per animation frame regardless of how fast pointer events arrive.
	function scheduleHoverUpdate(clientX: number, clientY: number) {
		pendingHover = { x: clientX, y: clientY };
		if (hoverRAF) return;
		hoverRAF = requestAnimationFrame(() => {
			hoverRAF = 0;
			if (pendingHover) updateHoverSegment(pendingHover.x, pendingHover.y);
		});
	}

	function clearHover() {
		pendingHover = null;
		if (hoverRAF) {
			cancelAnimationFrame(hoverRAF);
			hoverRAF = 0;
		}
		hoverSegment = null;
		if (layerViewer && typeof layerViewer.clearHighlight === 'function') {
			layerViewer.clearHighlight();
		}
	}

	function togglePropertiesMode() {
		propertiesMode = !propertiesMode;
		if (!propertiesMode) clearHover();
	}

	function toggleZoomSelectMode() {
		zoomSelectMode = !zoomSelectMode;
		zoomSelection = null;
		if (zoomSelectMode) clearHover();
	}

	function localViewportPoint(event: PointerEvent): { x: number; y: number } | null {
		if (!containerEl) return null;
		const box = containerEl.getBoundingClientRect();
		// Pointer capture keeps delivering events outside the canvas, so clamp to its bounds.
		return {
			x: Math.min(Math.max(event.clientX - box.left, 0), box.width),
			y: Math.min(Math.max(event.clientY - box.top, 0), box.height)
		};
	}

	function finishZoomSelection() {
		const rect = zoomRect;
		zoomSelection = null;
		if (!rect || !layerViewer) return;
		if (rect.width < MIN_ZOOM_SELECTION_PX || rect.height < MIN_ZOOM_SELECTION_PX) return;

		const corner1 = layerViewer.screenToMachine(rect.left, rect.top);
		const corner2 = layerViewer.screenToMachine(rect.left + rect.width, rect.top + rect.height);
		if (!corner1 || !corner2) return;

		autoFrame = false;
		layerViewer.CenterOnRectangle(
			Math.min(corner1.x, corner2.x), Math.min(corner1.y, corner2.y),
			Math.max(corner1.x, corner2.x), Math.max(corner1.y, corner2.y)
		);
		layerViewer.RenderScene(true);
		zoomSelectMode = false;
	}

	function onWindowKeyDown(event: KeyboardEvent) {
		if (event.key === 'Escape' && zoomSelectMode) {
			zoomSelectMode = false;
			zoomSelection = null;
		}
	}

	function onPointerDown(event: PointerEvent) {
		if (zoomSelectMode && event.button === 0) {
			const point = localViewportPoint(event);
			if (!point) return;
			clearHover();
			zoomSelection = { startX: point.x, startY: point.y, endX: point.x, endY: point.y };
			(event.target as HTMLElement).setPointerCapture(event.pointerId);
			return;
		}

		if (event.button === 0 || event.button === 1) {
			dragging = true;
			clearHover();
			dragX = event.clientX;
			dragY = event.clientY;
			(event.target as HTMLElement).setPointerCapture(event.pointerId);
		}
	}

	function onPointerMove(event: PointerEvent) {
		updateMousePosition(event);

		if (zoomSelection) {
			const point = localViewportPoint(event);
			if (point) zoomSelection = { ...zoomSelection, endX: point.x, endY: point.y };
			return;
		}

		if (dragging && layerViewer) {
			const dx = event.clientX - dragX;
			const dy = event.clientY - dragY;
			dragX = event.clientX;
			dragY = event.clientY;
			if (dx !== 0 || dy !== 0) autoFrame = false;
			layerViewer.Drag(dx, dy);
			layerViewer.RenderScene(true);
			return;
		}

		if (propertiesMode && !zoomSelectMode) scheduleHoverUpdate(event.clientX, event.clientY);
	}

	function onPointerUp() {
		if (zoomSelection) {
			finishZoomSelection();
			return;
		}
		dragging = false;
	}

	function onPointerCancel() {
		zoomSelection = null;
		dragging = false;
	}

	function onPointerLeave() {
		mousePosition = null;
		clearHover();
	}

	// Frames the build-area rectangle. The origin is the location of machine-zero
	// inside the plate (measured from the lower-left corner), so the plate corners in
	// machine coordinates run from -origin to (size - origin). For origin=(sx/2,sy/2)
	// this yields a view symmetric around zero, e.g. [-100..100] x [-125..125].
	function centerOnPlatform() {
		if (!layerViewer || !platform) return;
		const ox = platform.originx || 0;
		const oy = platform.originy || 0;
		const sx = platform.sizex || 300;
		const sy = platform.sizey || 300;
		// Optional per-axis padding (in mm) that enlarges the reset zoom window,
		// added on top of the fixed ZOOM_MARGIN on every side.
		const px = platform.paddingx || 0;
		const py = platform.paddingy || 0;
		layerViewer.CenterOnRectangle(
			-ox - ZOOM_MARGIN - px, -oy - ZOOM_MARGIN - py,
			(sx - ox) + ZOOM_MARGIN + px, (sy - oy) + ZOOM_MARGIN + py
		);
	}

	function resetView() {
		if (!layerViewer || !platform) return;
		autoFrame = true;
		centerOnPlatform();
		layerViewer.RenderScene(true);
	}

	function fitToPath() {
		if (!layerViewer) return;
		try {
			const bounds = layerViewer.getPathBoundaries?.();
			if (bounds && bounds.radius > 0 && platform) {
				const left = bounds.center.x - bounds.radius + (platform.originx || 0);
				const right = bounds.center.x + bounds.radius + (platform.originx || 0);
				const top = bounds.center.y - bounds.radius + (platform.originy || 0);
				const bottom = bounds.center.y + bounds.radius + (platform.originy || 0);
				autoFrame = false;
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
			platform.displayed_scatterplot = '';
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
					if (autoFrame) centerOnPlatform();
					layerViewer.RenderScene(true);
				}
			}
		});
		if (containerEl) ro.observe(containerEl);

		return () => ro.disconnect();
	});

	onDestroy(() => {
		module.onDataHasChanged = null;
		clearHover();
		pointLoadCounter++;
		if (platform) {
			platform.displayed_layer = 0;
			platform.displayed_build = 0;
			platform.displayed_scatterplot = '';
		}
	});
</script>

<svelte:window onkeydown={onWindowKeyDown} />

{#snippet layerViewBody()}
	<div class="layerview-container">
		<!-- WebGL render target — setupDOMElement sets position:relative on this -->
		<div
			bind:this={containerEl}
			class="layerview-canvas"
			class:zoom-select={zoomSelectMode}
			role="img"
			onwheel={onWheel}
			onpointerdown={onPointerDown}
			onpointermove={onPointerMove}
			onpointerup={onPointerUp}
			onpointercancel={onPointerCancel}
			onpointerleave={onPointerLeave}
		></div>

		{#if partOverlays.length > 0}
			<svg class="layerview-part-overlay" aria-hidden="true">
				{#each partOverlays as part (part.key)}
					<g
						class="layerview-part"
						class:highlighted={part.highlighted}
						class:dimmed={highlightPartUUID !== '' && !part.highlighted}
						class:disabled={part.disabled}
					>
						<rect class="layerview-part-box" x={part.left} y={part.top} width={part.width} height={part.height} />
						{#each part.markers as marker, markerIndex (markerIndex)}
							<rect
								class="layerview-part-marker"
								x={marker.x - PART_MARKER_PX / 2}
								y={marker.y - PART_MARKER_PX / 2}
								width={PART_MARKER_PX}
								height={PART_MARKER_PX}
							/>
						{/each}
						<text class="layerview-part-label" x={part.left} y={part.labelY}>{part.label}</text>
					</g>
				{/each}
			</svg>
		{/if}

		{#if zoomRect}
			<div
				class="layerview-zoom-selection"
				style={`left: ${zoomRect.left}px; top: ${zoomRect.top}px; width: ${zoomRect.width}px; height: ${zoomRect.height}px;`}
			></div>
		{/if}

		<!-- Overlaid toolbar -->
		<div class="layerview-toolbar">
			<button class="layerview-btn" onclick={resetView} title="Frame the build platform" aria-label="Zoom to platform">
				<Square size={16} />
				<span>Zoom to Platform</span>
			</button>
			<button class="layerview-btn" onclick={fitToPath} title="Frame the parts" aria-label="Zoom to parts">
				<Shapes size={16} />
				<span>Zoom to Parts</span>
			</button>
			<button
				class="layerview-btn"
				onclick={toggleZoomSelectMode}
				title="Drag a rectangle to zoom into it (Esc to cancel)"
				aria-label="Custom zoom: select a rectangle"
				aria-pressed={zoomSelectMode}
			>
				<ZoomIn size={16} />
				<span>Custom Zoom</span>
			</button>
			<button
				class="layerview-btn"
				onclick={() => coordinateSystemOverride = !coordinateSystemVisible}
				title="Toggle coordinate axes"
				aria-label="Toggle coordinate axes"
				aria-pressed={coordinateSystemVisible}
			>
				<Axis3d size={16} />
				<span>Axes</span>
			</button>
			<button
				class="layerview-btn"
				onclick={() => namesMode = !namesMode}
				title="Show part outlines and names"
				aria-label="Toggle part outlines and names"
				aria-pressed={namesMode}
			>
				<Tags size={16} />
				<span>Names</span>
			</button>
			<button
				class="layerview-btn"
				onclick={togglePropertiesMode}
				title="Show segment properties on hover"
				aria-label="Toggle segment properties inspector"
				aria-pressed={propertiesMode}
			>
				<Info size={16} />
				<span>Properties</span>
			</button>
			{#if pointsLoaded}
				<button
					class="layerview-btn"
					onclick={cyclePointColorMode}
					title="Change how the recorded points are coloured"
					aria-label={`Point colours: ${pointColorCaption}. Click to change`}
				>
					<Palette size={16} />
					<span>{pointColorCaption}</span>
				</button>
				{#if pointColumns.includes('laseron')}
					<button
						class="layerview-btn"
						onclick={toggleLaserOffPoints}
						title="Also show points recorded while the laser was off"
						aria-label="Toggle points with laser off"
						aria-pressed={showLaserOffPoints}
					>
						<ZapOff size={16} />
						<span>Laser Off</span>
					</button>
				{/if}
				<button
					class="layerview-btn"
					onclick={toggleToolpath}
					title="Show or hide the toolpath below the recorded points"
					aria-label="Toggle toolpath"
					aria-pressed={toolpathVisible}
				>
					<Route size={16} />
					<span>Toolpath</span>
				</button>
			{/if}
		</div>

		{#if labelCaption}
			<div class="layerview-label">{labelCaption}</div>
		{/if}

		<!-- Layer info overlay -->
		{#if layerCount > 0}
			{#if layerJumpOpen}
				<div class="layerview-layer-info layerview-layer-jump">
					<label for="layerjump-{module.uuid}">Layer</label>
					<input
						id="layerjump-{module.uuid}"
						type="number"
						inputmode="numeric"
						min="0"
						max={layerCount}
						step="1"
						bind:value={layerJumpValue}
						onkeydown={onLayerJumpKeyDown}
						onblur={() => (layerJumpOpen = false)}
						{@attach focusAndSelect}
					/>
					<span>/ {layerCount}</span>
				</div>
			{:else}
				<button
					type="button"
					class="layerview-layer-info layerview-layer-info-button"
					onclick={openLayerJump}
					title="Click to jump to a layer"
					aria-label={`Layer ${sliderValue} of ${layerCount}. Click to jump to a layer`}
				>
					Layer {sliderValue} / {layerCount}
				</button>
			{/if}
		{/if}

		{#if coordinateSystemVisible}
			<svg
				class="layerview-coordinate-indicator"
				viewBox="0 0 64 64"
				role="img"
				aria-label={`Machine coordinate axes, rotated ${transformAngle} degrees`}
			>
				<g transform={`translate(32 32) rotate(${-transformAngle})`}>
					<line class="coordinate-axis-x" x1="0" y1="0" x2="24" y2="0" />
					<polygon class="coordinate-axis-x" points="24,0 18,-3 18,3" />
					<text
						class="coordinate-label-x"
						x="27"
						y="4"
						transform={`rotate(${transformAngle} 27 4)`}
					>X</text>

					<line class="coordinate-axis-y" x1="0" y1="0" x2="0" y2="-24" />
					<polygon class="coordinate-axis-y" points="0,-24 -3,-18 3,-18" />
					<text
						class="coordinate-label-y"
						x="4"
						y="-24"
						transform={`rotate(${transformAngle} 4 -24)`}
					>Y</text>
				</g>
			</svg>
		{/if}

		<!-- Live cursor position readout (machine coordinates, mm) -->
		{#if mousePosition}
			<div class="layerview-mouse-pos">
				X: {mousePosition.x.toFixed(2)} &middot; Y: {mousePosition.y.toFixed(2)} mm
			</div>
		{/if}

		<!-- Segment property inspector popup (Properties toggle) -->
		{#if propertiesMode && hoverSegment}
			<div
				class="layerview-segment-popup"
				style={`left: ${hoverScreen.x + 14}px; top: ${hoverScreen.y + 14}px;`}
			>
				{#if hoverSegment.profilename}
					<div class="layerview-segment-popup-title">{hoverSegment.profilename}</div>
				{/if}
				<dl class="layerview-segment-popup-list">
					<dt>Laser power</dt>
					<dd>{Number(hoverSegment.laserpower ?? 0).toLocaleString()} W</dd>
					<dt>Laser speed</dt>
					<dd>{Number(hoverSegment.laserspeed ?? 0).toLocaleString()} mm/s</dd>
					{#if hoverSegment.type}
						<dt>Type</dt>
						<dd>{hoverSegment.type}</dd>
					{/if}
					{#if hoverSegment.laserindex !== undefined && hoverSegment.laserindex !== null}
						<dt>Laser</dt>
						<dd>#{hoverSegment.laserindex}</dd>
					{/if}
					{#if hoverSegment.partname}
						<dt>Part</dt>
						<dd>{hoverSegment.partname}{hoverSegment.partdisabled ? ' (disabled)' : ''}</dd>
					{/if}
					{#if hoverSegment.partid !== undefined && hoverSegment.partid !== null}
						<dt>Part ID</dt>
						<dd>{hoverSegment.partid}</dd>
					{/if}
				</dl>
			</div>
		{/if}

		<!-- Layer slider (vertical) -->
		{#if layerCount > 0}
			<div class="layerview-slider-wrap">
				<input
					type="range"
					class="layerview-slider"
					min="0"
					max={layerCount}
					value={sliderValue}
					oninput={onSliderChange}
					aria-label="Layer"
					aria-orientation="vertical"
				/>
			</div>
		{/if}
	</div>
{/snippet}

{#if visible}
	{#if isCard}
		<Card.Root class="flex flex-col h-full min-h-0">
			{#if cardTitle}
				<Card.Header class="pb-1">
					<Card.Title>{cardTitle}</Card.Title>
					{#if cardSubtitle}
						<Card.Description>{cardSubtitle}</Card.Description>
					{/if}
				</Card.Header>
			{/if}
			<Card.Content class="flex-1 min-h-0 overflow-hidden flex flex-col">
				{@render layerViewBody()}
			</Card.Content>
		</Card.Root>
	{:else}
		{@render layerViewBody()}
	{/if}
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
		cursor: crosshair;
	}
	.layerview-canvas.zoom-select {
		cursor: zoom-in;
	}
	.layerview-zoom-selection {
		position: absolute;
		border: 1px dashed var(--primary, #2563eb);
		background: color-mix(in srgb, var(--primary, #2563eb) 15%, transparent);
		pointer-events: none;
		z-index: 9;
	}
	.layerview-part-overlay {
		position: absolute;
		inset: 0;
		width: 100%;
		height: 100%;
		overflow: hidden;
		pointer-events: none;
		z-index: 8;
	}
	.layerview-part-box {
		fill: none;
		stroke: var(--foreground, #333333);
		stroke-width: 1;
		stroke-dasharray: 4 3;
	}
	.layerview-part-marker {
		fill: #ef4444;
	}
	.layerview-part-label {
		font-size: 10px;
		font-weight: 600;
		fill: var(--foreground, #333333);
		/* Halo keeps the label legible on top of dense hatching. */
		paint-order: stroke;
		stroke: var(--background, #ffffff);
		stroke-width: 3px;
		stroke-linejoin: round;
	}
	.layerview-part.highlighted .layerview-part-box {
		stroke: var(--primary, #2563eb);
		stroke-width: 2;
		stroke-dasharray: none;
		fill: color-mix(in srgb, var(--primary, #2563eb) 12%, transparent);
	}
	.layerview-part.highlighted .layerview-part-marker {
		fill: var(--primary, #2563eb);
	}
	.layerview-part.highlighted .layerview-part-label {
		fill: var(--primary, #2563eb);
		font-size: 11px;
	}
	.layerview-part.dimmed {
		opacity: 0.45;
	}
	.layerview-part.disabled .layerview-part-box {
		stroke: var(--muted-foreground, #888888);
	}
	.layerview-part.disabled .layerview-part-marker {
		fill: var(--muted-foreground, #888888);
	}
	.layerview-part.disabled .layerview-part-label {
		fill: var(--destructive, #dc2626);
		text-decoration: line-through;
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
		display: inline-flex;
		flex-direction: column;
		align-items: center;
		justify-content: center;
		gap: 3px;
		width: 64px;
		height: 56px;
		padding: 6px 4px;
		border: none;
		border-radius: 4px;
		background: rgba(0, 0, 0, 0.65);
		color: white;
		font-size: 11px;
		cursor: pointer;
		transition: background-color 0.2s;
	}
	/* Two-word captions such as "Zoom to Platform" wrap onto two centered lines. */
	.layerview-btn span {
		line-height: 1.15;
		text-align: center;
	}
	.layerview-btn:hover {
		background-color: rgba(0, 0, 0, 0.85);
	}
	.layerview-btn[aria-pressed='true'] {
		background-color: var(--primary, #2563eb);
		box-shadow: 0 0 0 1px rgba(255, 255, 255, 0.65);
	}
	.layerview-label {
		position: absolute;
		/* Below the toolbar (top 8px + 56px buttons). */
		top: 72px;
		left: 8px;
		max-width: calc(100% - 56px);
		padding: 4px 10px;
		border-radius: 4px;
		background: rgba(0, 0, 0, 0.65);
		color: white;
		font-size: 11px;
		font-variant-numeric: tabular-nums;
		overflow: hidden;
		text-overflow: ellipsis;
		white-space: nowrap;
		pointer-events: none;
		z-index: 10;
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
	.layerview-layer-info-button {
		border: none;
		cursor: pointer;
	}
	.layerview-layer-info-button:hover {
		background: rgba(0, 0, 0, 0.85);
	}
	.layerview-layer-jump {
		display: flex;
		align-items: center;
		gap: 6px;
		padding: 2px 6px 2px 10px;
	}
	.layerview-layer-jump input {
		width: 64px;
		padding: 1px 4px;
		border: 1px solid rgba(255, 255, 255, 0.5);
		border-radius: 3px;
		background: rgba(255, 255, 255, 0.95);
		color: #111111;
		font-size: 11px;
		font-variant-numeric: tabular-nums;
		text-align: right;
	}
	.layerview-coordinate-indicator {
		position: absolute;
		left: 8px;
		bottom: 8px;
		width: 64px;
		height: 64px;
		pointer-events: none;
		z-index: 9;
	}
	.coordinate-axis-x {
		fill: #ef4444;
		stroke: #ef4444;
		stroke-width: 2;
	}
	.coordinate-axis-y {
		fill: #22c55e;
		stroke: #22c55e;
		stroke-width: 2;
	}
	.coordinate-label-x,
	.coordinate-label-y {
		font-size: 11px;
		font-weight: 600;
		text-anchor: middle;
		stroke: none;
	}
	.coordinate-label-x {
		fill: #ef4444;
	}
	.coordinate-label-y {
		fill: #22c55e;
	}
	.layerview-mouse-pos {
		position: absolute;
		/* Shifted left so it clears the vertical layer slider on the right edge. */
		right: 40px;
		bottom: 8px;
		padding: 4px 10px;
		border-radius: 4px;
		background: rgba(0, 0, 0, 0.65);
		color: white;
		font-size: 11px;
		font-variant-numeric: tabular-nums;
		pointer-events: none;
		z-index: 10;
	}
	.layerview-segment-popup {
		position: absolute;
		min-width: 150px;
		max-width: 240px;
		padding: 8px 10px;
		border-radius: 6px;
		background: rgba(0, 0, 0, 0.82);
		color: white;
		font-size: 11px;
		line-height: 1.35;
		pointer-events: none;
		z-index: 20;
		box-shadow: 0 4px 12px rgba(0, 0, 0, 0.35);
	}
	.layerview-segment-popup-title {
		font-weight: 600;
		margin-bottom: 4px;
		overflow: hidden;
		text-overflow: ellipsis;
		white-space: nowrap;
	}
	.layerview-segment-popup-list {
		display: grid;
		grid-template-columns: auto auto;
		gap: 2px 12px;
		margin: 0;
	}
	.layerview-segment-popup-list dt {
		color: rgba(255, 255, 255, 0.65);
	}
	.layerview-segment-popup-list dd {
		margin: 0;
		text-align: right;
		font-variant-numeric: tabular-nums;
	}
	.layerview-slider-wrap {
		position: absolute;
		right: 8px;
		/* Anchor between the layer-info badge (top-right) and the bottom edge
		   so the slider spans nearly the full height of the view. */
		top: 44px;
		bottom: 16px;
		display: flex;
		align-items: center;
		justify-content: center;
		z-index: 10;
	}
	.layerview-slider {
		writing-mode: vertical-lr;
		direction: rtl;
		height: 100%;
		width: 20px;
		accent-color: var(--primary, #2563eb);
	}
</style>
