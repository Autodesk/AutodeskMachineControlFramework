<script lang="ts">
	import { untrack } from 'svelte';
	import * as Card from '$lib/components/ui/card/index.js';
	import { Button } from '$lib/components/ui/button/index.js';
	import UPlotChart, { type ChartTrace } from '$lib/components/UPlotChart.svelte';
	import VariableSelector from '$lib/components/VariableSelector.svelte';
	import TimeRangeSelector, { type RangeMode } from '$lib/components/TimeRangeSelector.svelte';
	import { usePollTick } from '$lib/amcf/poll.svelte';
	import { getVariables, getJournalInfo, getEnvelope, type JournalVariable } from '$lib/amcf/journalApi';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	const PALETTE = [
		'#6366f1', '#ef4444', '#10b981', '#f59e0b',
		'#3b82f6', '#ec4899', '#14b8a6', '#a855f7'
	];

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let title = $derived.by(() => module.title || module.caption || 'Process History');

	let variables = $state<JournalVariable[]>([]);
	let selected = $state<string[]>([]);
	let mode = $state<RangeMode>('live');
	let liveWindowSeconds = $state(60);

	let xValues = $state<number[]>([]);
	let traces = $state<ChartTrace[]>([]);
	let windowStart = $state(0);
	let windowEnd = $state(0);
	let hoveredIdx = $state<number | null>(null);
	let chartWidth = $state(800);
	let errorMsg = $state('');

	// Non-reactive coordination state (must not drive the reactive graph).
	let variablesLoaded = false;
	let fetching = false;
	let pending = false;

	function paletteColor (name: string): string {
		const idx = selected.indexOf(name);
		if (idx < 0) return '#94a3b8';
		return PALETTE[idx % PALETTE.length];
	}

	function toggleVariable (name: string) {
		if (selected.includes(name)) {
			selected = selected.filter((n) => n !== name);
		} else {
			if (selected.length >= PALETTE.length) return;
			selected = [...selected, name];
		}
	}

	function selectMode (m: RangeMode) {
		mode = m;
	}

	function selectLiveWindow (seconds: number) {
		liveWindowSeconds = seconds;
	}

	function handleZoom (minXSeconds: number, maxXSeconds: number) {
		windowStart = Math.max(0, Math.floor(minXSeconds * 1_000_000));
		windowEnd = Math.floor(maxXSeconds * 1_000_000);
		mode = 'custom';
		scheduleRefresh();
	}

	function handleHover (idx: number | null) {
		hoveredIdx = idx;
	}

	function applyResult (result: Awaited<ReturnType<typeof getEnvelope>>) {
		const available = result.series.filter((s) => s.available && s.buckets.length > 0);
		if (available.length === 0) {
			xValues = [];
			traces = [];
			return;
		}
		const base = available[0].buckets;
		xValues = base.map((b) => b.t / 1_000_000);

		traces = result.series.map((s) => {
			const buckets = s.buckets.length ? s.buckets : base.map(() => ({ t: 0, min: NaN, max: NaN, avg: NaN, last: NaN }));
			return {
				label: s.name,
				color: paletteColor(s.name),
				avg: buckets.map((b) => b.avg),
				min: buckets.map((b) => b.min),
				max: buckets.map((b) => b.max)
			};
		});
	}

	async function doRefresh () {
		if (!app) return;
		try {
			if (!variablesLoaded) {
				variables = await getVariables(app);
				variablesLoaded = true;
			}
			const info = await getJournalInfo(app);

			let start: number;
			let end: number;
			if (mode === 'custom') {
				start = windowStart;
				end = windowEnd;
			} else {
				end = info.currentTime;
				start = mode === 'live' ? Math.max(0, end - liveWindowSeconds * 1_000_000) : 0;
				windowStart = start;
				windowEnd = end;
			}
			if (end <= start) end = start + 1;

			if (selected.length === 0) {
				xValues = [];
				traces = [];
				errorMsg = '';
				return;
			}

			const count = Math.min(Math.max(Math.floor(chartWidth), 100), 4000);
			const result = await getEnvelope(app, selected, start, end, count);
			applyResult(result);
			errorMsg = '';
		} catch (e: any) {
			errorMsg = (e && e.message) ? e.message : 'Failed to load journal data.';
		}
	}

	async function scheduleRefresh () {
		if (fetching) {
			pending = true;
			return;
		}
		fetching = true;
		try {
			await doRefresh();
		} finally {
			fetching = false;
			if (pending) {
				pending = false;
				scheduleRefresh();
			}
		}
	}

	// React to polling, selection, and range mode changes. Window/data writes happen
	// untracked so they never feed back into this effect.
	$effect(() => {
		poll.v;
		selected.join(',');
		mode;
		liveWindowSeconds;
		untrack(() => { scheduleRefresh(); });
	});

	function formatValue (v: number): string {
		if (!isFinite(v)) return '—';
		const abs = Math.abs(v);
		if (abs >= 1000) return v.toFixed(0);
		if (abs >= 1) return v.toFixed(2);
		return v.toFixed(4);
	}

	function legendValue (trace: ChartTrace): string {
		const idx = hoveredIdx !== null && hoveredIdx < trace.avg.length ? hoveredIdx : trace.avg.length - 1;
		if (idx < 0) return '—';
		return formatValue(trace.avg[idx]);
	}

	function windowLabel (): string {
		const startSec = windowStart / 1_000_000;
		const endSec = windowEnd / 1_000_000;
		return `${startSec.toFixed(1)}s – ${endSec.toFixed(1)}s`;
	}
</script>

{#if visible}
	<Card.Root class="w-full h-full min-h-0 flex flex-col">
		<Card.Header class="pb-2">
			<div class="flex flex-wrap items-center justify-between gap-2">
				<Card.Title class="text-base">{title}</Card.Title>
				<div class="flex items-center gap-2">
					<TimeRangeSelector
						{mode}
						{liveWindowSeconds}
						onSelectMode={selectMode}
						onSelectLiveWindow={selectLiveWindow}
					/>
					{#if mode === 'custom'}
						<Button size="sm" variant="outline" onclick={() => selectMode('live')}>Reset</Button>
					{/if}
				</div>
			</div>
		</Card.Header>

		<Card.Content class="flex-1 min-h-0 flex gap-3">
			<div class="w-64 shrink-0 hidden md:block">
				<VariableSelector
					{variables}
					{selected}
					colorFor={paletteColor}
					onToggle={toggleVariable}
				/>
			</div>

			<div class="flex-1 min-w-0 flex flex-col gap-2">
				{#if errorMsg}
					<div class="text-sm text-red-500 border border-red-500/30 rounded p-2 bg-red-500/5">{errorMsg}</div>
				{/if}

				<div class="flex-1 min-h-0 border rounded-md p-2 relative" bind:clientWidth={chartWidth}>
					{#if selected.length === 0}
						<div class="absolute inset-0 flex items-center justify-center text-sm text-muted-foreground">
							Select one or more variables to plot their history.
						</div>
					{:else if xValues.length === 0}
						<div class="absolute inset-0 flex items-center justify-center text-sm text-muted-foreground">
							No recorded data in this time range yet.
						</div>
					{:else}
						<UPlotChart
							{xValues}
							{traces}
							height={340}
							onZoom={handleZoom}
							onHover={handleHover}
						/>
					{/if}
				</div>

				<div class="flex flex-wrap items-center gap-x-4 gap-y-1 text-xs">
					<span class="text-muted-foreground">Window: {windowLabel()}</span>
					{#each traces as trace (trace.label)}
						<span class="inline-flex items-center gap-1.5">
							<span class="size-2.5 rounded-sm" style={`background:${trace.color};`}></span>
							<span class="font-medium">{trace.label}</span>
							<span class="tabular-nums text-muted-foreground">{legendValue(trace)}</span>
						</span>
					{/each}
				</div>
			</div>
		</Card.Content>
	</Card.Root>
{/if}
