import type {
	Workflow,
	SequenceItem,
	WorkflowStep,
	ParallelGroup,
	StepCatalogEntry,
} from './workflowTypes';
import { isParallelGroup } from './workflowTypes';

let nextId = 1;
function uid(): string {
	return `wf_${Date.now()}_${nextId++}`;
}

export interface WorkflowState {
	workflow: Workflow;
	selectedStepId: string | null;
	catalog: StepCatalogEntry[];
	undoStack: string[];
}

function snapshot(wf: Workflow): string {
	return JSON.stringify(wf);
}

function pushUndo(state: WorkflowState): void {
	state.undoStack.push(snapshot(state.workflow));
	if (state.undoStack.length > 50) {
		state.undoStack.shift();
	}
}

export function createWorkflowState(
	workflow: Workflow,
	catalog: StepCatalogEntry[],
): WorkflowState {
	return {
		workflow,
		selectedStepId: null,
		catalog,
		undoStack: [],
	};
}

export function addStep(
	state: WorkflowState,
	stepRef: string,
	index?: number,
): void {
	pushUndo(state);
	const entry = state.catalog.find((c) => c.id === stepRef);
	const defaultParams: Record<string, unknown> = {};
	if (entry?.params) {
		for (const p of entry.params) {
			if (p.default !== undefined) defaultParams[p.name] = p.default;
		}
	}
	const step: WorkflowStep = { id: uid(), stepRef, params: defaultParams };
	const seq = state.workflow.sequence;
	if (index !== undefined && index >= 0 && index <= seq.length) {
		seq.splice(index, 0, step);
	} else {
		seq.push(step);
	}
}

export function removeStep(state: WorkflowState, stepId: string): void {
	pushUndo(state);
	const seq = state.workflow.sequence;
	const idx = seq.findIndex((s) => s.id === stepId);
	if (idx !== -1) {
		seq.splice(idx, 1);
		if (state.selectedStepId === stepId) state.selectedStepId = null;
		return;
	}
	for (const item of seq) {
		if (isParallelGroup(item)) {
			for (const branch of item.branches) {
				const bi = branch.findIndex((s) => s.id === stepId);
				if (bi !== -1) {
					branch.splice(bi, 1);
					if (state.selectedStepId === stepId) state.selectedStepId = null;
					return;
				}
			}
		}
	}
}

export function moveStep(
	state: WorkflowState,
	fromIndex: number,
	toIndex: number,
): void {
	pushUndo(state);
	const seq = state.workflow.sequence;
	if (fromIndex < 0 || fromIndex >= seq.length) return;
	const [item] = seq.splice(fromIndex, 1);
	seq.splice(toIndex, 0, item);
}

export function moveBranchStep(
	state: WorkflowState,
	groupId: string,
	branchIndex: number,
	fromIndex: number,
	toIndex: number,
): void {
	pushUndo(state);
	const group = state.workflow.sequence.find(
		(s) => s.id === groupId && isParallelGroup(s),
	) as ParallelGroup | undefined;
	if (!group) return;
	const branch = group.branches[branchIndex];
	if (!branch) return;
	if (fromIndex < 0 || fromIndex >= branch.length) return;
	const [item] = branch.splice(fromIndex, 1);
	branch.splice(toIndex, 0, item);
}

export function addParallelGroup(
	state: WorkflowState,
	branchCount: number = 2,
	index?: number,
): void {
	pushUndo(state);
	const branches: WorkflowStep[][] = [];
	for (let i = 0; i < branchCount; i++) branches.push([]);
	const group: ParallelGroup = { id: uid(), type: 'parallel', branches };
	const seq = state.workflow.sequence;
	if (index !== undefined && index >= 0 && index <= seq.length) {
		seq.splice(index, 0, group);
	} else {
		seq.push(group);
	}
}

export function addBranch(state: WorkflowState, groupId: string): void {
	pushUndo(state);
	const group = state.workflow.sequence.find(
		(s) => s.id === groupId && isParallelGroup(s),
	) as ParallelGroup | undefined;
	if (group) {
		group.branches.push([]);
	}
}

export function removeBranch(
	state: WorkflowState,
	groupId: string,
	branchIndex: number,
): void {
	pushUndo(state);
	const group = state.workflow.sequence.find(
		(s) => s.id === groupId && isParallelGroup(s),
	) as ParallelGroup | undefined;
	if (!group || group.branches.length <= 1) return;
	group.branches.splice(branchIndex, 1);
}

export function updateStepParams(
	state: WorkflowState,
	stepId: string,
	params: Record<string, unknown>,
): void {
	pushUndo(state);
	const step = findStepById(state, stepId);
	if (step) {
		step.params = { ...(step.params || {}), ...params };
	}
}

export function selectStep(state: WorkflowState, stepId: string | null): void {
	state.selectedStepId = stepId;
}

export function undo(state: WorkflowState): void {
	if (state.undoStack.length === 0) return;
	const prev = state.undoStack.pop()!;
	state.workflow = JSON.parse(prev);
	state.selectedStepId = null;
}

export function findStepById(
	state: WorkflowState,
	stepId: string,
): WorkflowStep | null {
	for (const item of state.workflow.sequence) {
		if (!isParallelGroup(item) && item.id === stepId) return item;
		if (isParallelGroup(item)) {
			for (const branch of item.branches) {
				const found = branch.find((s) => s.id === stepId);
				if (found) return found;
			}
		}
	}
	return null;
}

export function getCatalogEntry(
	state: WorkflowState,
	stepRef: string,
): StepCatalogEntry | undefined {
	return state.catalog.find((c) => c.id === stepRef);
}
