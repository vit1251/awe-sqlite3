
import assert from 'assert';
import { createRequire } from 'module';
const require = createRequire(import.meta.url);
const awe = require('../build/Release/awe-sqlite3.node');
const { open, close } = awe;


describe('Awe SQLite3', () => {

  describe('open(123.sqlite3)', () => {
    it('should return 1', async () => {
      const conn1 = await open('123.sqlite3');
      console.log(conn1);
      assert.equal(1, conn1);
    });
  });

  describe('close()', () => {
    it('should return 1', async () => {
      const conn2 = await open('123.sqlite3');
      console.log(conn2);
      await close(conn2);
    });
  });

});
