# Awe SQLite3

Awe SQlite3 is pure C implementation database connection provide pure async/await 
SQLite3 C API function invokation.

Example usage:

```javascript
import { open, exec, close } from 'awe-sqlite3';

const conn1 = await open('example.sqlite3');
await exec(conn1, "CREATE TABLE contacts (" +
      "contact_id INTEGER PRIMARY KEY, " +
      "first_name TEXT NOT NULL" +
      ");");
const code = await exec(conn1, `INSERT INTO contacts (contact_id, first_name) VALUES (${i}, 'user_${i}')`);
console.log(code);
await close(conn1);
```
