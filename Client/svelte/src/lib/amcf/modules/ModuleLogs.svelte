<script lang="ts">
	import { onMount, tick as svelteTick } from 'svelte';
	import * as Table from '$lib/components/ui/table/index.js';
	import { Button } from '$lib/components/ui/button/index.js';
	import ArrowDownToLine from '@lucide/svelte/icons/arrow-down-to-line';

	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let scrollContainer: HTMLDivElement | undefined = $state(undefined);
	let autoScroll = $state(true);
	let prevLength = 0;

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let items = $derived.by(() => { poll.v; return [...(module.DisplayItems || [])]; });

	$effect(() => {
		if (items.length !== prevLength && autoScroll && scrollContainer) {
			prevLength = items.length;
			svelteTick().then(() => {
				if (scrollContainer) {
					scrollContainer.scrollTop = scrollContainer.scrollHeight;
				}
			});
		}
	});

	function scrollToBottom () {
		if (scrollContainer) {
			scrollContainer.scrollTop = scrollContainer.scrollHeight;
			autoScroll = true;
		}
	}

	function onScroll () {
		if (!scrollContainer) return;
		const { scrollTop, scrollHeight, clientHeight } = scrollContainer;
		autoScroll = scrollHeight - scrollTop - clientHeight < 40;
	}
</script>

{#if visible}
	<div class="w-full flex-1 min-h-0 flex flex-col border rounded-md overflow-hidden">
		<div class="flex-shrink-0 flex items-center justify-between px-3 py-1.5 bg-muted border-b">
			<span class="text-xs text-muted-foreground font-medium">
				{items.length} entries
			</span>
			<Button variant="ghost" size="sm" class="h-7 px-2" onclick={scrollToBottom}>
				<ArrowDownToLine class="h-3.5 w-3.5" />
			</Button>
		</div>

		<div
			bind:this={scrollContainer}
			onscroll={onScroll}
			class="flex-1 min-h-0 overflow-auto"
		>
			<Table.Root>
				<Table.Header class="sticky top-0 bg-muted z-10">
					<Table.Row>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[110px]">Time</Table.Head>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider w-[130px]">Subsystem</Table.Head>
						<Table.Head class="text-xs font-semibold uppercase tracking-wider">Message</Table.Head>
					</Table.Row>
				</Table.Header>
				<Table.Body>
					{#if items.length === 0}
						<Table.Row>
							<Table.Cell colspan={3} class="text-center py-8 text-muted-foreground text-sm">
								No log entries yet.
							</Table.Cell>
						</Table.Row>
					{:else}
						{#each items as entry (entry.logIndex)}
							<Table.Row class="hover:bg-muted/50 transition-colors">
								<Table.Cell class="text-xs font-mono py-1 text-muted-foreground whitespace-nowrap">{entry.logTime}</Table.Cell>
								<Table.Cell class="text-xs py-1 text-muted-foreground">{entry.logSubsystem}</Table.Cell>
								<Table.Cell class="text-xs py-1">{entry.logText}</Table.Cell>
							</Table.Row>
						{/each}
					{/if}
				</Table.Body>
			</Table.Root>
		</div>
	</div>
{/if}
