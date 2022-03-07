
window.LibAMCFWrapper = null;

createLibAMCF ({}).then (function (Module) {
	window.LibAMCFWrapper = new CWrapper (Module);
}); 

