export interface StepParam {
	name: string;
	type: 'int' | 'float' | 'string' | 'bool';
	unit?: string;
	default?: unknown;
}

export interface StepCatalogEntry {
	id: string;
	label: string;
	category: 'functionblock' | 'template';
	color: string;
	params?: StepParam[];
}

export interface WorkflowStep {
	id: string;
	stepRef: string;
	params?: Record<string, unknown>;
}

export interface ParallelGroup {
	id: string;
	type: 'parallel';
	branches: WorkflowStep[][];
}

export type SequenceItem = WorkflowStep | ParallelGroup;

export function isParallelGroup(item: SequenceItem): item is ParallelGroup {
	return (item as ParallelGroup).type === 'parallel';
}

export interface Workflow {
	uuid: string;
	name: string;
	sequence: SequenceItem[];
}
