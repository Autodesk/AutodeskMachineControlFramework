<script lang="ts">
	import { flip } from 'svelte/animate';
	import { dndzone } from 'svelte-dnd-action';
	import type { ParallelGroup, StepCatalogEntry } from './workflowTypes';
	import WorkflowStep from './WorkflowStep.svelte';

	let {
		group,
		catalog,
		selectedStepId = null,
		onselect,
		onremove,
		onbranchupdate,
		onremovegroup,
		onaddbranch,
		onremovebranch,
	}: {
		group: ParallelGroup;
		catalog: StepCatalogEntry[];
		selectedStepId?: string | null;
		onselect?: (id: string) => void;
		onremove?: (id: string) => void;
		onbranchupdate?: (groupId: string, branchIndex: number, items: any[]) => void;
		onremovegroup?: (groupId: string) => void;
		onaddbranch?: (groupId: string) => void;
		onremovebranch?: (groupId: string, branchIndex: number) => void;
	} = $props();

	const flipDurationMs = 200;

	function getCatalog(stepRef: string): StepCatalogEntry | undefined {
		return catalog.find((c) => c.id === stepRef);
	}

	function handleBranchConsider(branchIndex: number, e: CustomEvent<{ items: any[] }>) {
		onbranchupdate?.(group.id, branchIndex, e.detail.items);
	}

	function handleBranchFinalize(branchIndex: number, e: CustomEvent<{ items: any[] }>) {
		onbranchupdate?.(group.id, branchIndex, e.detail.items);
	}
</script>

<div class="relative rounded-lg border border-dashed border-muted-foreground/30 bg-muted/20 p-3">
	<div class="mb-2 flex items-center justify-between">
		<span class="text-xs font-semibold uppercase tracking-wider text-muted-foreground/60">
			Parallel
		</span>
		<div class="flex gap-1">
			<button
				class="text-xs text-muted-foreground/50 hover:text-foreground transition-colors px-1"
				onclick={() => onaddbranch?.(group.id)}
				title="Add branch"
			>+ Branch</button>
			<button
				class="text-xs text-muted-foreground/50 hover:text-destructive transition-colors px-1"
				onclick={() => onremovegroup?.(group.id)}
				title="Remove parallel group"
			>✕</button>
		</div>
	</div>

	<div class="grid gap-2" style="grid-template-columns: repeat({group.branches.length}, 1fr);">
		{#each group.branches as branch, branchIndex}
			<div class="flex flex-col gap-1">
				<div class="flex items-center justify-between mb-1">
					<span class="text-[10px] text-muted-foreground/40 uppercase">
						Branch {branchIndex + 1}
					</span>
					{#if group.branches.length > 1}
						<button
							class="text-[10px] text-muted-foreground/30 hover:text-destructive transition-colors"
							onclick={() => onremovebranch?.(group.id, branchIndex)}
							title="Remove branch"
						>✕</button>
					{/if}
				</div>
				<div
					class="min-h-[48px] flex flex-col gap-1 rounded border border-dashed border-muted-foreground/20 p-1"
					use:dndzone={{ items: branch, flipDurationMs, type: 'workflow-steps' }}
					onconsider={(e) => handleBranchConsider(branchIndex, e)}
					onfinalize={(e) => handleBranchFinalize(branchIndex, e)}
				>
					{#each branch as step (step.id)}
						<div animate:flip={{ duration: flipDurationMs }}>
							<WorkflowStep
								{step}
								catalogEntry={getCatalog(step.stepRef)}
								selected={selectedStepId === step.id}
								{onselect}
								{onremove}
							/>
						</div>
					{/each}
				</div>
			</div>
		{/each}
	</div>
</div>
