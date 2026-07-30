<script lang="ts">
	import { Button } from '$lib/components/ui/button/index.js';
	import MdiIcon from '$lib/amcf/MdiIcon.svelte';

	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let cssstyle = $derived.by(() => { poll.v; return module.cssstyle || ''; });
	let buttoncssstyle = $derived.by(() => { poll.v; return module.buttoncssstyle || ''; });
	let buttons = $derived.by(() => { poll.v; return [...(module.buttons || [])]; });
	let isToolbar = $derived.by(() => { poll.v; return module.buttondistribution === 'toolbar'; });

	function handleClick (btn: any) {
		if (!app) return;
		if (btn.event) {
			const formvalues = (btn.eventformvalues && app.assembleFormValues)
				? app.assembleFormValues(btn.eventformvalues)
				: {};
			app.triggerUIEvent(btn.event, btn.uuid, formvalues);
		}
		if (btn.targetpage)
			app.changePage(btn.targetpage);
	}

	function widthClass (btn: any): string {
		if (btn.width === 'narrow') return 'w-16 shrink-0';
		if (btn.width === 'fixed') return 'w-28 shrink-0';
		return '';
	}

	function iconResourceURL (btn: any): string {
		return (btn.iconresource && app) ? app.getImageURL(btn.iconresource) : '';
	}
</script>

{#if visible}
	<div class="flex gap-2 w-full {isToolbar ? 'items-stretch' : ''}" style={cssstyle}>
		{#each buttons as btn (btn.uuid)}
			{#if btn.kind === 'spring'}
				<div class="flex-1 min-w-0"></div>
			{:else if btn.kind === 'spacer'}
				<div class="w-6 shrink-0"></div>
			{:else}
				{@const hasIcon = !!(btn.icon || btn.iconresource)}
				<Button
					variant={btn.variant === 'primary' ? 'default' : 'outline'}
					style={buttoncssstyle}
					disabled={btn.disabled}
					onclick={() => handleClick(btn)}
					class="{widthClass(btn)} {hasIcon ? 'h-auto flex-col gap-1 py-2' : ''}"
				>
					{#if btn.iconresource}
						<!-- Render the packaged SVG as a mask tinted with the button's
						     text colour (currentColor), so the icon adapts to light/dark
						     themes and to the primary/outline button variants. -->
						<span
							class="h-6 w-6 shrink-0"
							style="background-color: currentColor; -webkit-mask: url('{iconResourceURL(btn)}') no-repeat center / contain; mask: url('{iconResourceURL(btn)}') no-repeat center / contain;"
							aria-hidden="true"
						></span>
					{:else if btn.icon}
						<MdiIcon icon={btn.icon} class="h-5 w-5" />
					{/if}
					{#if btn.caption}
						<span class="text-xs">{btn.caption}</span>
					{/if}
				</Button>
			{/if}
		{/each}
	</div>
{/if}
