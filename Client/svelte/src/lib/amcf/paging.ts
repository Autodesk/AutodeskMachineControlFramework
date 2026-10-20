/*
 * Client-side paging of list modules. Only the rows of one page are rendered, so that long
 * lists (e.g. thousands of executions) do not block the browser.
 */
export interface ListPage<T> {
	rows: T[];
	page: number;       // 1-based
	pageCount: number;
	first: number;      // 1-based index of the first row shown
	last: number;       // 1-based index of the last row shown
	total: number;
}

export function paginate<T> (entries: T[], page: number, perPage: number): ListPage<T> {
	const total = entries.length;
	const pageCount = Math.max(1, Math.ceil(total / perPage));
	page = Math.min(page, pageCount);
	const start = (page - 1) * perPage;
	const rows = entries.slice(start, start + perPage);

	return { rows, page, pageCount, first: rows.length > 0 ? start + 1 : 0, last: start + rows.length, total };
}
