
const aweModule = require("bindings")("awe-sqlite3");
const {open, exec, close} = aweModule;

module.exports = {
    open,
    exec,
    close,
};
