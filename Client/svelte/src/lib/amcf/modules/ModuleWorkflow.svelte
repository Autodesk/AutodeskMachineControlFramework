<script lang="ts">
	import { usePollTick } from '$lib/amcf/poll.svelte';
	import WorkflowEditor from './workflow/WorkflowEditor.svelte';
	import type { Workflow, StepCatalogEntry } from './workflow/workflowTypes';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => {
		poll.v;
		return module.visible !== false;
	});

	let workflow: Workflow = $derived.by(() => {
		poll.v;
		return module.workflowData || module.getDefaultWorkflow();
	});

	let catalog: StepCatalogEntry[] = $derived.by(() => {
		poll.v;
		return module.catalogData || module.getDefaultCatalog();
	});
</script>

{#if visible}
	<div class="w-full" style="min-height: 500px; height: 70vh;">
		{#key workflow.uuid}
			<WorkflowEditor initialWorkflow={workflow} {catalog} />
		{/key}
	</div>
{/if}
