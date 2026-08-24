<script lang="ts">
	import { flip } from 'svelte/animate';
	import { dndzone } from 'svelte-dnd-action';
	import type {
		Workflow,
		SequenceItem,
		StepCatalogEntry,
		WorkflowStep as WStep,
		ParallelGroup,
	} from './workflowTypes';
	import { isParallelGroup } from './workflowTypes';
	import {
		createWorkflowState,
		addStep,
		removeStep,
		addParallelGroup,
		addBranch,
		removeBranch,
		updateStepParams,
		selectStep,
		undo,
		findStepById,
		getCatalogEntry,
		type WorkflowState,
	} from './workflowStore';
	import WorkflowStepComp from './WorkflowStep.svelte';
	import WorkflowParallelGroup from './WorkflowParallelGroup.svelte';
	import WorkflowPalette from './WorkflowPalette.svelte';
	import WorkflowToolbar from './WorkflowToolbar.svelte';
	import WorkflowStepEditor from './WorkflowStepEditor.svelte';

	let {
		initialWorkflow,
		catalog,
	}: {
		initialWorkflow: Workflow;
		catalog: StepCatalogEntry[];
	} = $props();

	// Intentionally snapshot props once; the editor owns its own mutable copy.
	const snapshotWorkflow = JSON.parse(JSON.stringify(initialWorkflow)) as Workflow;
	const snapshotCatalog = [...catalog];

	let state: WorkflowState = $state(createWorkflowState(snapshotWorkflow, snapshotCatalog));

	const flipDurationMs = 200;

	let selectedStep = $derived(
		state.selectedStepId ? findStepById(state, state.selectedStepId) : null,
	);
	let selectedCatalog = $derived(
		selectedStep ? getCatalogEntry(state, selectedStep.stepRef) : undefined,
	);

	function handleSequenceConsider(e: CustomEvent<{ items: SequenceItem[] }>) {
		state.workflow.sequence = e.detail.items;
	}

	function handleSequenceFinalize(e: CustomEvent<{ items: SequenceItem[] }>) {
		state.workflow.sequence = e.detail.items;
	}

	function handleSelect(id: string) {
		selectStep(state, state.selectedStepId === id ? null : id);
	}

	function handleRemove(id: string) {
		removeStep(state, id);
	}

	function handleAddFromPalette(stepRef: string) {
		addStep(state, stepRef);
	}

	function handleAddParallel() {
		addParallelGroup(state, 2);
	}

	function handleUndo() {
		undo(state);
	}

	function handleBranchUpdate(groupId: string, branchIndex: number, items: any[]) {
		const group = state.workflow.sequence.find(
			(s) => s.id === groupId && isParallelGroup(s),
		) as ParallelGroup | undefined;
		if (group) {
			group.branches[branchIndex] = items;
		}
	}

	function handleRemoveGroup(groupId: string) {
		const idx = state.workflow.sequence.findIndex((s) => s.id === groupId);
		if (idx !== -1) {
			state.workflow.sequence.splice(idx, 1);
		}
	}

	function handleAddBranch(groupId: string) {
		addBranch(state, groupId);
	}

	function handleRemoveBranch(groupId: string, branchIndex: number) {
		removeBranch(state, groupId, branchIndex);
	}

	function handleUpdateParams(stepId: string, params: Record<string, unknown>) {
		updateStepParams(state, stepId, params);
	}

	function handleCloseEditor() {
		selectStep(state, null);
	}

	function handleNameChange(name: string) {
		state.workflow.name = name;
	}

	function getCatalogForStep(stepRef: string): StepCatalogEntry | undefined {
		return catalog.find((c) => c.id === stepRef);
	}
</script>

<div class="flex flex-col gap-2 h-full">
	<WorkflowToolbar
		canUndo={state.undoStack.length > 0}
		workflowName={state.workflow.name}
		onaddparallel={handleAddParallel}
		onundo={handleUndo}
		onnamchange={handleNameChange}
	/>

	<div class="flex-1 flex gap-2 min-h-0">
		<!-- Palette -->
		<div class="w-48 shrink-0 rounded-md border bg-card/30 overflow-hidden">
			<WorkflowPalette {catalog} onadd={handleAddFromPalette} />
		</div>

		<!-- Canvas -->
		<div class="flex-1 relative overflow-auto rounded-md border bg-card/20 p-4">
			<div
				class="flex flex-col gap-2 min-h-[120px]"
				use:dndzone={{ items: state.workflow.sequence, flipDurationMs, type: 'workflow-steps' }}
				onconsider={handleSequenceConsider}
				onfinalize={handleSequenceFinalize}
			>
				{#each state.workflow.sequence as item (item.id)}
					<div animate:flip={{ duration: flipDurationMs }}>
						{#if isParallelGroup(item)}
							<WorkflowParallelGroup
								group={item}
								{catalog}
								selectedStepId={state.selectedStepId}
								onselect={handleSelect}
								onremove={handleRemove}
								onbranchupdate={handleBranchUpdate}
								onremovegroup={handleRemoveGroup}
								onaddbranch={handleAddBranch}
								onremovebranch={handleRemoveBranch}
							/>
						{:else}
							<WorkflowStepComp
								step={item}
								catalogEntry={getCatalogForStep(item.stepRef)}
								selected={state.selectedStepId === item.id}
								onselect={handleSelect}
								onremove={handleRemove}
							/>
						{/if}
					</div>
				{/each}
			</div>

			{#if state.workflow.sequence.length === 0}
				<div class="flex items-center justify-center h-full text-muted-foreground/30 text-sm">
					Drag steps from the palette or click to add
				</div>
			{/if}
		</div>

		<!-- Step Editor -->
		{#if state.selectedStepId}
			<div class="w-56 shrink-0 rounded-md border bg-card/30 overflow-hidden">
				<WorkflowStepEditor
					step={selectedStep}
					catalogEntry={selectedCatalog}
					onupdate={handleUpdateParams}
					onclose={handleCloseEditor}
				/>
			</div>
		{/if}
	</div>
</div>
