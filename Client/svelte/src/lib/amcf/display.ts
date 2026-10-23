import { getContext, setContext } from 'svelte';

const DISPLAYED_KEY = Symbol('amcf-displayed');

// Reactive getter telling a module whether it is actually shown, e.g. false inside an
// inactive tab. Containers that hide mounted content provide a narrower getter.
export type DisplayedGetter = () => boolean;

export function provideDisplayed (getter: DisplayedGetter): void {
	setContext(DISPLAYED_KEY, getter);
}

export function useDisplayed (): DisplayedGetter {
	return getContext<DisplayedGetter>(DISPLAYED_KEY) ?? (() => true);
}
