
import assert from 'assert';
import { Database } from '../index.js';

describe('Awe SQLite3', () => {

  describe('open(123.sqlite3)', () => {
    it('should OK', async () => {
      const db = new Database();
      await db.Open('./1.sqlite3');
    });
  });

  describe('exec', () => {
  
    it('should OK when processing valid SQL query', async () => {
      const db = new Database();
      await db.Open('./exec_1.sqlite3');
      await db.Exec("CREATE TABLE contacts (" +
         "contact_id INTEGER PRIMARY KEY, " +
         "first_name TEXT NOT NULL" +
         ");");
      await db.Close();
    });

    it('should OK when processing 1000 sequential valid SQL query', async () => {
      const db = new Database();
      await db.Open('./exec_2.sqlite3');
      await db.Exec("CREATE TABLE contacts (" +
         "contact_id INTEGER PRIMARY KEY, " +
         "first_name TEXT NOT NULL" +
         ");");
      const resolves = [];
      for (let i = 0; i < 1000; i++) {
        const code = await db.Exec(`INSERT INTO contacts (contact_id, first_name) VALUES (${i}, 'user_${i}')`);
        resolves.push(code);
      }
      console.log(resolves);
      await db.Close();
    });

    it('should OK when processing 1000 parallel valid SQL query', async () => {
      const db = new Database();
      await db.Open('./exec_3.sqlite3');
      await db.Exec("CREATE TABLE contacts (" +
         "contact_id INTEGER PRIMARY KEY, " +
         "first_name TEXT NOT NULL" +
         ");");
      const promises = [];
      for (let i = 0; i < 1000; i++) {
        const query = `INSERT INTO contacts (contact_id, first_name) VALUES (${i}, 'user_${i}')`;
        const promise = db.Exec(query);
        promises.push(promise);
      }
      const resolves = await Promise.all(promises);
      console.log(resolves);
      await db.Close();
    });


  });

  describe('close()', () => {
    it('open should OK', async () => {
      const db = new Database();
      await db.Open('./4.sqlite3');
      await db.Close();
    });
    
    it('not open should OK', async () => {
      const db = new Database();
      await db.Close();
    });
  });

});
