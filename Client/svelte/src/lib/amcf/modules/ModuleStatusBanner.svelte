<script lang="ts">
	import * as Alert from '$lib/components/ui/alert/index.js';
	import CircleCheck from '@lucide/svelte/icons/circle-check';
	import AlertTriangle from '@lucide/svelte/icons/alert-triangle';
	import CircleAlert from '@lucide/svelte/icons/circle-alert';
	import Info from '@lucide/svelte/icons/info';
	import Archive from '@lucide/svelte/icons/archive';

	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module }: { module: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let severity = $derived.by(() => { poll.v; return (module.severity || 'ok').toLowerCase(); });
	let bannerTitle = $derived.by(() => { poll.v; return module.title || ''; });
	let bannerDesc = $derived.by(() => { poll.v; return module.description || ''; });

	const iconMap: Record<string, any> = {
		ok: CircleCheck,
		warning: AlertTriangle,
		error: CircleAlert,
		info: Info,
		archive: Archive,
	};

	const variantMap: Record<string, string> = {
		ok: 'border-green-600/25 bg-green-50 text-green-800',
		warning: 'border-amber-500/25 bg-amber-50 text-amber-800',
		error: 'border-destructive/25 bg-red-50 text-red-800',
		info: 'border-blue-500/25 bg-blue-50 text-blue-800',
		archive: 'border-muted-foreground/25 bg-muted text-muted-foreground',
	};

	let Icon = $derived(iconMap[severity] || Info);
	let cls = $derived(variantMap[severity] || variantMap.ok);
</script>

{#if visible}
	<Alert.Root class={cls}>
		<Icon class="h-4 w-4" />
		{#if bannerTitle}
			<Alert.Title>{bannerTitle}</Alert.Title>
		{/if}
		{#if bannerDesc}
			<Alert.Description>{bannerDesc}</Alert.Description>
		{/if}
	</Alert.Root>
{/if}
