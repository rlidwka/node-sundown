var assert = require('assert');
var binding = require('./build/Release/binding');
//assert.equal('world', binding.hello());
//console.log('binding.hello() =', binding.hello());

assert.equal(typeof(binding), 'object');
assert.equal(typeof(binding.Markdown), 'function');
assert.throws(function() {
	new binding.Markdown();
});
assert.throws(function() {
	new binding.Markdown({}, {}, {});
});
assert.throws(function() {
	new binding.Markdown({}, 1);
});
console.log(new binding.Markdown({}, {}));
