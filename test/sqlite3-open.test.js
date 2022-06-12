
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
  
    it('should OK when processing valid SQL query', async () => {
      const conn1 = await open('./exec_1.sqlite3');
      await exec(conn1, "CREATE TABLE contacts (" +
         "contact_id INTEGER PRIMARY KEY, " +
         "first_name TEXT NOT NULL" +
         ");");
      await close(conn1);
    });

    it('should OK when processing 100 sequential valid SQL query', async () => {
      const conn1 = await open('./exec_2.sqlite3');
      await exec(conn1, "CREATE TABLE contacts (" +
         "contact_id INTEGER PRIMARY KEY, " +
         "first_name TEXT NOT NULL" +
         ");");
      for (let i = 0; i < 100; i++) {
        const code = await exec(conn1, `INSERT INTO contacts (contact_id, first_name) VALUES (${i}, 'user_${i}')`);
        console.log(code);
      }
      await close(conn1);
    });

    it('should OK when processing 100 parallel valid SQL query', async () => {
      const conn1 = await open('./exec_3.sqlite3');
      await exec(conn1, "CREATE TABLE contacts (" +
         "contact_id INTEGER PRIMARY KEY, " +
         "first_name TEXT NOT NULL" +
         ");");
      const promises = [];
      for (let i = 0; i < 100; i++) {
        const query = `INSERT INTO contacts (contact_id, first_name) VALUES (${i}, 'user_${i}')`;
        const promise = exec(conn1, query);
        promises.push(promise);
      }
      const resolves = await Promise.all(promises);
      console.log(resolves);
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
