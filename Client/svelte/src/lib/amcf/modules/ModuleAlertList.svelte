<script lang="ts">
	import * as Table from '$lib/components/ui/table/index.js';
	import { ScrollArea } from '$lib/components/ui/scroll-area/index.js';
	import { Badge } from '$lib/components/ui/badge/index.js';
	import { Button } from '$lib/components/ui/button/index.js';
	import CircleCheck from '@lucide/svelte/icons/circle-check';
	import CircleX from '@lucide/svelte/icons/circle-x';
	import TriangleAlert from '@lucide/svelte/icons/triangle-alert';
	import Info from '@lucide/svelte/icons/info';

	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let entries = $derived.by(() => { poll.v; return [...(module.entries || [])]; });

	function severityVariant (level: string): 'default' | 'destructive' | 'secondary' | 'outline' {
		const l = (level || '').toLowerCase();
		if (l === 'error' || l === 'critical' || l === 'fatal') return 'destructive';
		if (l === 'warning') return 'default';
		return 'secondary';
	}

	function formatTimestamp (ts: string): string {
		if (!ts) return '';
		try {
			const d = new Date(ts.replace(' UTC', 'Z').replace('Z UTC', 'Z'));
			return d.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });
		} catch { return ts; }
	}

	function acknowledge (alert: any) {
		if (module.selectevent && app) {
			const formvalues: Record<string, string> = {};
			if (module.selectionvalueuuid)
				formvalues[module.selectionvalueuuid] = alert.alertuuid;
			app.triggerUIEvent(module.selectevent, module.uuid, formvalues);
		}
	}
</script>

{#if visible}
	<div class="w-full border rounded-md overflow-hidden">
		<ScrollArea class="max-h-[500px]">
			<Table.Root>
				<Table.Header class="sticky top-0 bg-muted z-10">
					<Table.Row>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[90px]">Time</Table.Head>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider">Alert</Table.Head>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[100px]">Level</Table.Head>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[60px] text-center">Active</Table.Head>
					</Table.Row>
				</Table.Header>
				<Table.Body>
					{#if entries.length === 0}
						<Table.Row>
							<Table.Cell colspan={4} class="text-center py-8 text-muted-foreground text-sm">
								No alerts.
							</Table.Cell>
						</Table.Row>
					{:else}
						{#each entries as alert, idx (alert.alertuuid || idx)}
							<Table.Row
								class="hover:bg-muted/50 transition-colors cursor-pointer"
								onclick={() => acknowledge(alert)}
							>
								<Table.Cell class="text-xs font-mono py-1.5 text-muted-foreground whitespace-nowrap">
									{formatTimestamp(alert.alerttimestamp)}
								</Table.Cell>
								<Table.Cell class="text-sm py-1.5">
									<div>{alert.alertcaption}</div>
									{#if alert.alertcontext}
										<div class="text-xs text-muted-foreground">{alert.alertcontext}</div>
									{/if}
								</Table.Cell>
								<Table.Cell class="py-1.5">
									<Badge variant={severityVariant(alert.severity || alert.alertlevel)}>
										{alert.severity || alert.alertlevel || 'info'}
									</Badge>
								</Table.Cell>
								<Table.Cell class="text-center py-1.5">
									{#if alert.alertactive}
										<CircleX class="h-4 w-4 text-destructive inline-block" />
									{:else}
										<CircleCheck class="h-4 w-4 text-green-600 inline-block" />
									{/if}
								</Table.Cell>
							</Table.Row>
						{/each}
					{/if}
				</Table.Body>
			</Table.Root>
		</ScrollArea>
	</div>
{/if}
