<script lang="ts">
	import type { WorkflowStep as WStep, StepCatalogEntry } from './workflowTypes';
	import { Badge } from '$lib/components/ui/badge';

	let {
		step,
		catalogEntry,
		selected = false,
		onselect,
		onremove,
	}: {
		step: WStep;
		catalogEntry: StepCatalogEntry | undefined;
		selected?: boolean;
		onselect?: (id: string) => void;
		onremove?: (id: string) => void;
	} = $props();

	let label = $derived(catalogEntry?.label ?? step.stepRef);
	let category = $derived(catalogEntry?.category ?? 'functionblock');
	let color = $derived(catalogEntry?.color ?? 'blue');

	const colorMap: Record<string, { bg: string; border: string; text: string; badge: string }> = {
		blue: {
			bg: 'bg-blue-500/10',
			border: 'border-blue-500/40',
			text: 'text-blue-200',
			badge: 'bg-blue-600 text-white',
		},
		amber: {
			bg: 'bg-amber-500/10',
			border: 'border-amber-500/40',
			text: 'text-amber-200',
			badge: 'bg-amber-600 text-white',
		},
		green: {
			bg: 'bg-green-500/10',
			border: 'border-green-500/40',
			text: 'text-green-200',
			badge: 'bg-green-600 text-white',
		},
		red: {
			bg: 'bg-red-500/10',
			border: 'border-red-500/40',
			text: 'text-red-200',
			badge: 'bg-red-600 text-white',
		},
	};

	let colors = $derived(colorMap[color] || colorMap.blue);

	function handleClick(e: MouseEvent) {
		e.stopPropagation();
		onselect?.(step.id);
	}

	function handleRemove(e: MouseEvent) {
		e.stopPropagation();
		onremove?.(step.id);
	}

	function handleKeydown(e: KeyboardEvent) {
		if (e.key === 'Enter' || e.key === ' ') {
			e.preventDefault();
			onselect?.(step.id);
		}
	}
</script>

<!-- svelte-ignore a11y_no_static_element_interactions -->
<div
	class="group flex items-center gap-2 rounded-md border px-3 py-2 transition-all cursor-grab active:cursor-grabbing
		{colors.bg} {colors.border}
		{selected ? 'ring-2 ring-primary shadow-md' : 'hover:shadow-sm'}"
	onclick={handleClick}
	onkeydown={handleKeydown}
	role="button"
	tabindex="0"
	data-step-id={step.id}
>
	<span class="text-muted-foreground/50 select-none" title="Drag to reorder">⠿</span>

	<Badge class="{colors.badge} text-[10px] px-1.5 py-0 leading-tight uppercase">
		{category === 'functionblock' ? 'FB' : 'TPL'}
	</Badge>

	<span class="flex-1 text-sm font-medium {colors.text}">{label}</span>

	{#if step.params && Object.keys(step.params).length > 0}
		<span class="text-xs text-muted-foreground/60 tabular-nums">
			{Object.entries(step.params)
				.map(([k, v]) => `${k}=${v}`)
				.join(', ')}
		</span>
	{/if}

	<button
		class="opacity-0 group-hover:opacity-100 text-muted-foreground/40 hover:text-destructive transition-opacity text-sm px-1"
		onclick={handleRemove}
		title="Remove step"
	>
		✕
	</button>
</div>
