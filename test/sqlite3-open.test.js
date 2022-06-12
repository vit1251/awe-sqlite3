
import assert from 'assert';
import { createRequire } from 'module';
const require = createRequire(import.meta.url);
const awe = require('../build/Release/awe-sqlite3.node');
const { open, exec, close } = awe;


describe('Awe SQLite3', () => {

  describe('open(123.sqlite3)', () => {
    it('should OK', async () => {
      const conn1 = await open('./1.sqlite3');
    });
  });

  describe('exec', () => {
    it('should OK processing on valid SQL query', async () => {
      const conn1 = await open('./2.sqlite3');
      await exec(conn1, "CREATE TABLE contacts (" +
         "contact_id INTEGER PRIMARY KEY, " +
         "first_name TEXT NOT NULL" +
         ");");
      await close(conn1);
    });
  });

  describe('close()', () => {
    it('should OK', async () => {
      const conn2 = await open('./3.sqlite3');
      await close(conn2);
    });
  });

});
