<script lang="ts">
	import { Button } from '$lib/components/ui/button/index.js';
	import ChevronsLeft from '@lucide/svelte/icons/chevrons-left';
	import ChevronLeft from '@lucide/svelte/icons/chevron-left';
	import ChevronRight from '@lucide/svelte/icons/chevron-right';
	import ChevronsRight from '@lucide/svelte/icons/chevrons-right';

	import type { ListPage } from '$lib/amcf/paging';

	let { listPage, onPageChange }: { listPage: ListPage<unknown>; onPageChange: (page: number) => void } = $props();
</script>

{#if listPage.pageCount > 1}
	<div class="flex-shrink-0 flex items-center justify-end gap-1 px-2 py-1 border-t bg-muted/40">
		<span class="mr-2 text-xs text-muted-foreground tabular-nums">
			{listPage.first}–{listPage.last} of {listPage.total}
		</span>
		<Button variant="ghost" size="icon" class="size-7" aria-label="First page" disabled={listPage.page <= 1} onclick={() => onPageChange(1)}>
			<ChevronsLeft class="h-3.5 w-3.5" />
		</Button>
		<Button variant="ghost" size="icon" class="size-7" aria-label="Previous page" disabled={listPage.page <= 1} onclick={() => onPageChange(listPage.page - 1)}>
			<ChevronLeft class="h-3.5 w-3.5" />
		</Button>
		<span class="px-1 text-xs text-muted-foreground tabular-nums">
			{listPage.page} / {listPage.pageCount}
		</span>
		<Button variant="ghost" size="icon" class="size-7" aria-label="Next page" disabled={listPage.page >= listPage.pageCount} onclick={() => onPageChange(listPage.page + 1)}>
			<ChevronRight class="h-3.5 w-3.5" />
		</Button>
		<Button variant="ghost" size="icon" class="size-7" aria-label="Last page" disabled={listPage.page >= listPage.pageCount} onclick={() => onPageChange(listPage.pageCount)}>
			<ChevronsRight class="h-3.5 w-3.5" />
		</Button>
	</div>
{/if}
