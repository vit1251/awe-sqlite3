# Awe SQLite3

Awe SQlite3 is pure C implementation database connection provide pure async/await 
SQLite3 C API function invokation.

## Example usage

```javascript
import { Database } from 'awe-sqlite3';

const db = new Database();
await db.Open('example.sqlite3');
await db.Exec("CREATE TABLE contacts (" +
      "contact_id INTEGER PRIMARY KEY, " +
      "first_name TEXT NOT NULL" +
      ");");
await db.Exec(`INSERT INTO contacts (contact_id, first_name) VALUES (${i}, 'user_${i}')`);
await db.Close();
```

## Benchmark

| Module              | Operation | Count | Duration    |
| ------------------- | --------- | ----- | ----------- |
| sqlite (seq)        | INSERT    |  1000 | 1.654 sec.  |
| awe-sqlite v1 (sim) | INSERT    |  1000 | 1.290 sec.  |
| awe-sqlite v2 (seq) | INSERT    |  1000 | 1.338 sec.  |
| awe-sqlite v2 (sim) | INSERT    |  1000 | 1.217 sec.  |

Execute on Node v22.x
