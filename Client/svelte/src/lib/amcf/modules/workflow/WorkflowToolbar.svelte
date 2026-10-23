<script lang="ts">
	import { Button } from '$lib/components/ui/button';
	import { Separator } from '$lib/components/ui/separator';

	let {
		canUndo = false,
		workflowName = '',
		onaddparallel,
		onundo,
		onnamchange,
	}: {
		canUndo?: boolean;
		workflowName?: string;
		onaddparallel?: () => void;
		onundo?: () => void;
		onnamchange?: (name: string) => void;
	} = $props();
</script>

<div class="flex items-center gap-2 rounded-md border bg-card/50 px-3 py-2">
	<input
		type="text"
		value={workflowName}
		oninput={(e) => onnamchange?.(e.currentTarget.value)}
		class="bg-transparent text-sm font-semibold text-foreground border-none outline-none w-48 placeholder:text-muted-foreground/40"
		placeholder="Workflow name..."
	/>

	<Separator orientation="vertical" class="h-5" />

	<Button variant="outline" size="sm" onclick={onaddparallel}>
		+ Parallel
	</Button>

	<div class="flex-1"></div>

	<Button variant="ghost" size="sm" disabled={!canUndo} onclick={onundo}>
		↩ Undo
	</Button>
</div>
