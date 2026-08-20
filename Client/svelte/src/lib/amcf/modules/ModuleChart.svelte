<script lang="ts">
	import UPlotChart, { type ChartTrace } from '$lib/components/UPlotChart.svelte';
	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });

	// module.chartData is populated by the shared AMCModule_Chart model from
	// /api/dataseries/{uuid} as an array of [timestampSeconds, value] pairs.
	let chartData = $derived.by(() => { poll.v; return (module.chartData || []) as [number, number][]; });
	let hasData = $derived(chartData.length > 0);

	let color = $derived(module.color || '#6366f1');
	let label = $derived(module.title || module.caption || 'Value');

	let xValues = $derived.by(() => chartData.map((p) => p[0]));
	let traces = $derived.by<ChartTrace[]>(() => {
		if (chartData.length === 0) return [];
		return [{ label, color, avg: chartData.map((p) => p[1]) }];
	});
</script>

{#if visible}
	<div class="w-full relative" style="min-height: 300px;">
		{#if !hasData}
			<div class="absolute inset-0 flex items-center justify-center text-sm text-muted-foreground">
				No chart data available.
			</div>
		{:else}
			<UPlotChart {xValues} {traces} height={300} />
		{/if}
	</div>
{/if}
