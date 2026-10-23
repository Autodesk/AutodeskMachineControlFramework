<script lang="ts">
	import type { Snippet } from 'svelte';
	import { provideDisplayed, useDisplayed } from '$lib/amcf/display';

	let { active, children }: { active: boolean; children: Snippet } = $props();

	const parentDisplayed = useDisplayed();
	provideDisplayed(() => parentDisplayed() && active);
</script>

{@render children()}
