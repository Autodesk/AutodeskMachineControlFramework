<script lang="ts">
	import { Button } from '$lib/components/ui/button/index.js';
	import Upload from '@lucide/svelte/icons/upload';
	import FileUp from '@lucide/svelte/icons/file-up';
	import X from '@lucide/svelte/icons/x';

	import { usePollTick } from '$lib/amcf/poll.svelte';

	let { module, app }: { module: any; app: any } = $props();
	const poll = usePollTick();

	let visible = $derived.by(() => { poll.v; return module.visible !== false; });
	let dragOver = $state(false);
	let uploading = $state(false);
	let statusMessage = $state('');

	let acceptedTypes = $derived(module.acceptedtypes || (module.uploadclass === 'image' ? '.png,.jpg' : '.3mf'));
	let caption = $derived(module.uploadcaption || `Select ${module.uploadclass === 'image' ? 'image' : '3MF'} files to upload`);

	function handleFiles (files: FileList | null) {
		if (!files || files.length === 0 || !app) return;
		const file = files[0];
		uploading = true;
		statusMessage = `Uploading ${file.name}...`;

		if (typeof module.state.setChosenFile === 'function')
			module.state.setChosenFile(file);
		else
			module.state.chosenFile = file;

		module.state._onComplete = () => {
			statusMessage = `${file.name} uploaded successfully.`;
			uploading = false;
		};
		module.state._onError = () => {
			statusMessage = `Upload failed.`;
			uploading = false;
		};

		try {
			if (module.uploadclass === 'image')
				app.performImageUpload(module.state, module.uploadsuccessevent, module.uploadfailureevent);
			else
				app.performJobUpload(module.state, module.uploadsuccessevent, module.uploadfailureevent);
		} catch (e) {
			statusMessage = `Upload failed.`;
			uploading = false;
		}
	}

	function onDrop (e: DragEvent) {
		e.preventDefault();
		dragOver = false;
		handleFiles(e.dataTransfer?.files ?? null);
	}

	function onDragOver (e: DragEvent) {
		e.preventDefault();
		dragOver = true;
	}

	function onDragLeave () {
		dragOver = false;
	}

	function onFileInput (e: Event) {
		const target = e.target as HTMLInputElement;
		handleFiles(target.files);
		target.value = '';
	}
</script>

{#if visible}
	<div class="w-full">
		<div
			role="button"
			tabindex="0"
			class="relative border-2 border-dashed rounded-lg p-8 text-center transition-colors cursor-pointer
				{dragOver ? 'border-primary bg-primary/5' : 'border-border hover:border-primary/50'}"
			ondrop={onDrop}
			ondragover={onDragOver}
			ondragleave={onDragLeave}
			onclick={() => document.getElementById(`upload-${module.uuid}`)?.click()}
			onkeydown={(e) => e.key === 'Enter' && document.getElementById(`upload-${module.uuid}`)?.click()}
		>
			<input
				id={`upload-${module.uuid}`}
				type="file"
				accept={acceptedTypes}
				class="hidden"
				onchange={onFileInput}
			/>

			{#if uploading}
				<div class="flex flex-col items-center gap-2">
					<div class="animate-spin h-8 w-8 border-2 border-primary border-t-transparent rounded-full"></div>
					<p class="text-sm text-muted-foreground">{statusMessage}</p>
				</div>
			{:else}
				<div class="flex flex-col items-center gap-2">
					<Upload class="h-8 w-8 text-muted-foreground" />
					<p class="text-sm text-muted-foreground">{caption}</p>
					{#if statusMessage}
						<p class="text-xs text-green-600">{statusMessage}</p>
					{/if}
				</div>
			{/if}
		</div>
	</div>
{/if}
