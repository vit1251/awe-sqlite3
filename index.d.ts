
export class Database {

  constructor();

  /**
   * Open database
   *
   */
  async Open(path: string): Promise<void>;

  /**
   * Exec INSERT query
   */
  async Exec(query: string): Promise<void>;

  /**
   * Close database
   *
   */
  async Close(): Promise<void>;

}
