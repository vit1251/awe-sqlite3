
import assert from 'assert';
import { createRequire } from 'module';
const require = createRequire(import.meta.url);
const awe = require('../build/Release/awe-sqlite3.node');
const { open } = awe;


describe('Awe SQLite3', () => {
  describe('open(123.sqlite3)', () => {
    it('should return 1', async () => {
      const conn = await open('123.sqlite3');
      console.log(conn);
      assert.equal(1, conn);
    });
  });
});
