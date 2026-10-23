<script lang="ts">
	import type { WorkflowStep, StepCatalogEntry } from './workflowTypes';
	import { Input } from '$lib/components/ui/input';
	import { Label } from '$lib/components/ui/label';
	import { Separator } from '$lib/components/ui/separator';
	import { Button } from '$lib/components/ui/button';

	let {
		step,
		catalogEntry,
		onupdate,
		onclose,
	}: {
		step: WorkflowStep | null;
		catalogEntry: StepCatalogEntry | undefined;
		onupdate?: (stepId: string, params: Record<string, unknown>) => void;
		onclose?: () => void;
	} = $props();

	function handleParamChange(paramName: string, value: string, type: string) {
		if (!step) return;
		let parsed: unknown = value;
		if (type === 'int') parsed = parseInt(value, 10) || 0;
		else if (type === 'float') parsed = parseFloat(value) || 0;
		else if (type === 'bool') parsed = value === 'true';
		onupdate?.(step.id, { [paramName]: parsed });
	}
</script>

{#if step && catalogEntry}
	<div class="flex flex-col h-full">
		<div class="flex items-center justify-between px-3 py-2">
			<h3 class="text-xs font-semibold uppercase tracking-wider text-muted-foreground/60">
				Properties
			</h3>
			<button
				class="text-xs text-muted-foreground/40 hover:text-foreground transition-colors"
				onclick={onclose}
			>✕</button>
		</div>
		<Separator />
		<div class="flex flex-col gap-3 px-3 py-3">
			<div>
				<Label class="text-[10px] uppercase text-muted-foreground/50">Step Type</Label>
				<p class="text-sm font-medium text-foreground">{catalogEntry.label}</p>
			</div>
			<div>
				<Label class="text-[10px] uppercase text-muted-foreground/50">Category</Label>
				<p class="text-xs text-muted-foreground">{catalogEntry.category}</p>
			</div>

			{#if catalogEntry.params && catalogEntry.params.length > 0}
				<Separator />
				<span class="text-[10px] uppercase tracking-wider text-muted-foreground/40">
					Parameters
				</span>
				{#each catalogEntry.params as param}
					<div class="flex flex-col gap-1">
						<Label class="text-xs text-muted-foreground">
							{param.name}
							{#if param.unit}
								<span class="text-muted-foreground/40">({param.unit})</span>
							{/if}
						</Label>
						<Input
							type={param.type === 'int' || param.type === 'float' ? 'number' : 'text'}
							value={String(step.params?.[param.name] ?? param.default ?? '')}
							oninput={(e) => handleParamChange(param.name, e.currentTarget.value, param.type)}
							class="h-8 text-sm"
						/>
					</div>
				{/each}
			{:else}
				<p class="text-xs text-muted-foreground/50 italic">No configurable parameters</p>
			{/if}
		</div>
	</div>
{:else}
	<div class="flex items-center justify-center h-full px-4">
		<p class="text-xs text-muted-foreground/40 text-center">
			Select a step to view its properties
		</p>
	</div>
{/if}
