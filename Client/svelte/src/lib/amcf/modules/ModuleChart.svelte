<script lang="ts">
	import { onMount, onDestroy } from 'svelte';

	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let hasData = $derived.by(() => { poll.v; return module.chartData && module.chartData.length > 0; });
	let canvasEl: HTMLCanvasElement | undefined = $state(undefined);
	let containerEl: HTMLDivElement | undefined = $state(undefined);

	function drawChart () {
		if (!canvasEl || !containerEl) return;
		const data: [number, number][] = module.chartData || [];
		if (data.length === 0) return;

		const rect = containerEl.getBoundingClientRect();
		const w = rect.width;
		const h = rect.height || 300;
		const dpr = window.devicePixelRatio || 1;

		canvasEl.width = w * dpr;
		canvasEl.height = h * dpr;
		canvasEl.style.width = `${w}px`;
		canvasEl.style.height = `${h}px`;

		const ctx = canvasEl.getContext('2d');
		if (!ctx) return;
		ctx.scale(dpr, dpr);

		const pad = { top: 20, right: 20, bottom: 30, left: 60 };
		const plotW = w - pad.left - pad.right;
		const plotH = h - pad.top - pad.bottom;

		let xMin = Infinity, xMax = -Infinity, yMin = Infinity, yMax = -Infinity;
		for (const [x, y] of data) {
			if (x < xMin) xMin = x;
			if (x > xMax) xMax = x;
			if (y < yMin) yMin = y;
			if (y > yMax) yMax = y;
		}
		if (xMin === xMax) { xMin -= 1; xMax += 1; }
		if (yMin === yMax) { yMin -= 1; yMax += 1; }

		const xRange = xMax - xMin;
		const yRange = yMax - yMin;
		const toX = (v: number) => pad.left + ((v - xMin) / xRange) * plotW;
		const toY = (v: number) => pad.top + plotH - ((v - yMin) / yRange) * plotH;

		ctx.clearRect(0, 0, w, h);

		// Grid and axes
		ctx.strokeStyle = '#e0e0e0';
		ctx.lineWidth = 0.5;
		const gridLines = 5;
		for (let i = 0; i <= gridLines; i++) {
			const y = pad.top + (plotH / gridLines) * i;
			ctx.beginPath(); ctx.moveTo(pad.left, y); ctx.lineTo(w - pad.right, y); ctx.stroke();
		}

		// Axis labels
		ctx.fillStyle = '#888';
		ctx.font = '10px sans-serif';
		ctx.textAlign = 'right';
		for (let i = 0; i <= gridLines; i++) {
			const val = yMax - (yRange / gridLines) * i;
			const y = pad.top + (plotH / gridLines) * i;
			ctx.fillText(val.toFixed(2), pad.left - 6, y + 3);
		}

		ctx.textAlign = 'center';
		for (let i = 0; i <= gridLines; i++) {
			const val = xMin + (xRange / gridLines) * i;
			const x = pad.left + (plotW / gridLines) * i;
			ctx.fillText(val.toFixed(1), x, h - pad.bottom + 16);
		}

		// Line
		ctx.beginPath();
		ctx.strokeStyle = 'hsl(var(--primary))';
		ctx.lineWidth = 1.5;
		for (let i = 0; i < data.length; i++) {
			const px = toX(data[i][0]);
			const py = toY(data[i][1]);
			if (i === 0) ctx.moveTo(px, py);
			else ctx.lineTo(px, py);
		}
		ctx.stroke();
	}

	let resizeObserver: ResizeObserver | null = null;

	onMount(() => {
		module.onChartDataUpdated = () => {
			poll.v++;
			drawChart();
		};
		drawChart();

		resizeObserver = new ResizeObserver(() => drawChart());
		if (containerEl) resizeObserver.observe(containerEl);
	});

	onDestroy(() => {
		module.onChartDataUpdated = null;
		if (resizeObserver) resizeObserver.disconnect();
	});
</script>

{#if visible}
	<div bind:this={containerEl} class="w-full relative" style="min-height: 300px;">
		{#if !hasData}
			<div class="absolute inset-0 flex items-center justify-center text-sm text-muted-foreground">
				No chart data available.
			</div>
		{/if}
		<canvas bind:this={canvasEl} class="w-full h-full"></canvas>
	</div>
{/if}
