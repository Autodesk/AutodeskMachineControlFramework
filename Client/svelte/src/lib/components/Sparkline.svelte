<script lang="ts">
	// A tiny inline sparkline for previewing a variable's recent history,
	// e.g. next to a value in a parameter list.
	let {
		values = [],
		color = '#6366f1',
		width = 96,
		height = 24
	}: {
		values: number[];
		color?: string;
		width?: number;
		height?: number;
	} = $props();

	let path = $derived.by(() => {
		if (!values || values.length < 2) return '';
		let min = Infinity;
		let max = -Infinity;
		for (const v of values) {
			if (v < min) min = v;
			if (v > max) max = v;
		}
		if (min === max) { min -= 1; max += 1; }
		const range = max - min;
		const step = width / (values.length - 1);
		return values
			.map((v, i) => {
				const x = i * step;
				const y = height - ((v - min) / range) * height;
				return `${i === 0 ? 'M' : 'L'}${x.toFixed(1)},${y.toFixed(1)}`;
			})
			.join(' ');
	});
</script>

{#if path}
	<svg {width} {height} viewBox={`0 0 ${width} ${height}`} class="overflow-visible">
		<path d={path} fill="none" stroke={color} stroke-width="1.5" />
	</svg>
{:else}
	<span class="text-xs text-muted-foreground">—</span>
{/if}
