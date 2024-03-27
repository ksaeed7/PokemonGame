#include "db.h"

using namespace std;


db::db(const std::string& dbPath) {

	create_database(dbPath);
	validateUsersDB();
	initializePokemonCards();

	/*if (sqlite3_open(dbPath.c_str(), &connection) != SQLITE_OK) {
		// Handle the error as you see fit, e.g., throw an exception.
		//throw std::runtime_error("Failed to open database");
	}*/
}

db::~db() {
	sqlite3_close(connection);
}


void db::create_database(string database_name)
{
	std::lock_guard<std::mutex> lock(dbMutex);  // Lock for the duration of this function

	// Create a connection to the database.


	if (sqlite3_open((database_name + ".db").c_str(), &connection) != SQLITE_OK) {
		fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(connection));
		cout << "Database failed to open" << endl;
		exit(1);  // Exit program since we can't proceed without DB
	}

	char* errMsg = 0;
	int rc;

	rc = sqlite3_exec(connection, "PRAGMA synchronous = FULL", 0, 0, &errMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to set PRAGMA synchronous: %s\n", errMsg);
		sqlite3_free(errMsg);
		sqlite3_close(connection);
		exit(1);
	}

	cout << "Database connection successful" << endl;

	sqlite3_stmt* stmt;

	rc = sqlite3_prepare_v2(connection, "SELECT name FROM sqlite_master WHERE type='table' AND name='Users'", -1, &stmt, NULL);
	if (rc != SQLITE_OK || sqlite3_step(stmt) != SQLITE_ROW) {
		rc = sqlite3_prepare_v2(connection, users_table_sql.c_str(), -1, &stmt, NULL);
		if (rc != SQLITE_OK || sqlite3_step(stmt) != SQLITE_DONE) {
			fprintf(stderr, "Failed to create Users table: %s\n", sqlite3_errmsg(connection));
			sqlite3_finalize(stmt);
			sqlite3_close(connection);
			exit(1);
		}
	}
	sqlite3_finalize(stmt);

	rc = sqlite3_prepare_v2(connection, "SELECT name FROM sqlite_master WHERE type='table' AND name='Pokemon_cards'", -1, &stmt, NULL);
	if (rc != SQLITE_OK || sqlite3_step(stmt) != SQLITE_ROW) {
		rc = sqlite3_prepare_v2(connection, pokemon_cards_table_sql.c_str(), -1, &stmt, NULL);
		if (rc != SQLITE_OK || sqlite3_step(stmt) != SQLITE_DONE) {
			fprintf(stderr, "Failed to create Pokemon_cards table: %s\n", sqlite3_errmsg(connection));
			sqlite3_finalize(stmt);
			sqlite3_close(connection);
			exit(1);
		}
	}
	sqlite3_finalize(stmt);
}


void db::validateUsersDB()
{
	// Check if there are any users in the database. If not, create one.
	char* errMsg = NULL;
	sqlite3_stmt* stmt;
	string check_users_sql = "SELECT COUNT(*) FROM Users";

	if (sqlite3_prepare_v2(connection, check_users_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(connection));
		cout << errMsg << endl;
	}
	else {
		if (sqlite3_step(stmt) != SQLITE_ROW) {
			fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(connection));
			cout << errMsg << endl;
		}
	}

	int userCount = sqlite3_column_int(stmt, 0);

	if (userCount == 0)
	{
		addUser("Jamal", "Saeed", "Jsaeed", "Popcorn", true);
		addUser("Ahmed", "Alshemmam", "Shemam", "Popcorn", true);
		addUser("Mohamed", "Qassem", "Mqassem", "Popcorn", true);
		addUser("Root", "", "Root", "Root01", true);
		addUser("Mary", "LittleLamb", "Mary", "Mary01", true);
		addUser("John", "Doe", "John", "John01", true);
		addUser("Moe", "K", "Moe", "Moe01", false);
		//addUser("Root", "Root", "Admin", "password", true);
	}
	else
	{
		cout << "UserDB is active!" << endl;
	}
}

void db::addUser(string first_name, string last_name, string user_name, string password, bool isRoot)
{
	executeWithLock([&] {
		string create_user_sql = "INSERT INTO Users (first_name, last_name, user_name, password, usd_balance, is_root) VALUES (?, ?, ?, ?, 100.0, ?)";
		sqlite3_stmt* stmt;

		if (sqlite3_prepare_v2(connection, create_user_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(connection));
			return;
		}

		// Bind the parameters
		sqlite3_bind_text(stmt, 1, first_name.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, last_name.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 3, user_name.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 4, password.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 5, isRoot ? 1 : 0);

		if (sqlite3_step(stmt) != SQLITE_DONE) {
			fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(connection));
		}
		else {
			cout << user_name << " created successfully!\n";
		}

		sqlite3_finalize(stmt); });
}

std::string db::login(const std::string& username, const std::string& password) {

	return executeWithLock([&] {
		sqlite3_stmt* stmt;
		std::string check_user_sql = "SELECT COUNT(*) FROM Users WHERE user_name = ? AND password = ?";

		sqlite3* connection = getConnection();
		if (sqlite3_prepare_v2(connection, check_user_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			return std::string();
		}

		// Binding parameters to prevent SQL injection
		sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) != SQLITE_ROW) {
			sqlite3_finalize(stmt);
			return std::string();
		}

		int userCount = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);

		if (userCount > 0) {
			return username;
		}
		else {
			return std::string();
		}
		});
}
void db::initializePokemonCards()
{
	char* errMsg = NULL;
	sqlite3_stmt* stmt;
	string check_pokemon_sql = "SELECT COUNT(*) FROM Pokemon_cards";

	if (sqlite3_prepare_v2(connection, check_pokemon_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(connection));
		cout << errMsg << endl;
	}
	else {
		if (sqlite3_step(stmt) != SQLITE_ROW) {
			fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(connection));
			cout << errMsg << endl;
		}
	}

	int pokemonCount = sqlite3_column_int(stmt, 0);

	if (pokemonCount == 0) {
		// No Pokemon cards in the database, insert the provided data.
		string insert_pokemon_data_sql = "INSERT INTO Pokemon_cards (card_name, card_type, rarity, count, owner_id) VALUES "
			"('Pikachu', 'Electric', 'Common', 2, 1), "
			"('Charizard', 'Fire', 'Rare', 1, 1), "
			"('Bulbasaur', 'Grass', 'Common', 50, 1), "
			"('Squirtle', 'Water', 'Uncommon', 30, 1), "
			"('Jigglypuff', 'Normal', 'Common', 3, 1)";

		if (sqlite3_exec(connection, insert_pokemon_data_sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
			fprintf(stderr, "Database error: %s\n", errMsg);
			sqlite3_free(errMsg);
			cout << errMsg << endl;
		}

		printf("Inserted Pokemon data into the database.\n");
	}
	else
	{
		cout << "Pokemon database is already initialized" << endl;
	}
}

bool db::userExists(const std::string& username, const std::string& password, int& user_id, bool& isRootUser, std::string& first_name, std::string& last_name) {
	return executeWithLock([&]() {

		sqlite3_stmt* stmt;
		std::string check_user_sql = "SELECT ID, is_root, first_name, last_name FROM Users WHERE user_name = ? AND password = ?";
		if (sqlite3_prepare_v2(connection, check_user_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			cout << "Error preparing SQL statement: " << sqlite3_errmsg(connection) << endl;
			return false;
		}

		// Bind parameters to avoid SQL injection
		sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			user_id = sqlite3_column_int(stmt, 0);
			isRootUser = sqlite3_column_int(stmt, 1);
			first_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
			last_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
			sqlite3_finalize(stmt);

			cout << "UserID: " << user_id << " signed in" << endl;
			return true;
		}
		else {
			sqlite3_finalize(stmt);
			//cout << "ERROR: Invalid username or password." << endl;
			return false;
		}
		});
}


sqlite3* db::getConnection() const {
	return connection;
}


void db::buyCard(string card_name, string card_type, string rarity, double price_per_card, int num_cards_to_buy, int buyerID, int& errorCode, string& errorMsg) {
	executeWithLock([&] {
		errorCode = 0;
		errorMsg = "";
		// Check if user exists
		string user_exists_sql = "SELECT COUNT(*) FROM Users WHERE ID = ?";
		sqlite3_stmt* stmt;
		if (sqlite3_prepare_v2(connection, user_exists_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(connection));
			return;
		}
		sqlite3_bind_int(stmt, 1, buyerID);
		if (sqlite3_step(stmt) != SQLITE_ROW) {
			fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(connection));
			errorCode = 403;
			errorMsg = "Database error";
			return;
		}
		int userExists = sqlite3_column_int(stmt, 0);
		if (userExists == 0) {
			cout << "User " << buyerID << " does not exist." << endl;
			errorCode = 403;
			errorMsg = "User does not exist";
			return;
		}
		sqlite3_finalize(stmt);

		// Check user's balance
		string balance_sql = "SELECT usd_balance FROM Users WHERE ID = ?";
		if (sqlite3_prepare_v2(connection, balance_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(connection));
			return;
		}
		sqlite3_bind_int(stmt, 1, buyerID);
		if (sqlite3_step(stmt) != SQLITE_ROW) {
			fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(connection));
			return;
		}
		double balance = sqlite3_column_double(stmt, 0);
		sqlite3_finalize(stmt);

		// Check if user has enough money
		double total_cost = price_per_card * num_cards_to_buy;
		if (balance < total_cost) {
			cout << "User " << buyerID << " does not have enough balance to buy the cards." << endl;
			errorCode = 403;
			errorMsg = "User does not have enough balance to buy the cards";
			return;
		}

		// Update user's balance
		string update_balance_sql = "UPDATE Users SET usd_balance = ? WHERE ID = ?";
		if (sqlite3_prepare_v2(connection, update_balance_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(connection));
			return;
		}
		sqlite3_bind_double(stmt, 1, balance - total_cost);
		sqlite3_bind_int(stmt, 2, buyerID);
		if (sqlite3_step(stmt) != SQLITE_DONE) {
			fprintf(stderr, "Failed to update user's balance: %s\n", sqlite3_errmsg(connection));
			return;
		}
		sqlite3_finalize(stmt);

		// Check if user already owns the card
		string card_exists_sql = "SELECT COUNT(*) FROM Pokemon_cards WHERE card_name = ? AND card_type = ? AND rarity = ? AND owner_id = ?";
		if (sqlite3_prepare_v2(connection, card_exists_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(connection));
			return;
		}
		sqlite3_bind_text(stmt, 1, card_name.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, card_type.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 3, rarity.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 4, buyerID);
		if (sqlite3_step(stmt) != SQLITE_ROW) {
			fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(connection));
			return;
		}
		int cardExists = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);

		if (cardExists > 0) {
			// Update card count
			string update_card_sql = "UPDATE Pokemon_cards SET count = count + ? WHERE card_name = ? AND card_type = ? AND rarity = ? AND owner_id = ?";
			if (sqlite3_prepare_v2(connection, update_card_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
				fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(connection));
				return;
			}
			sqlite3_bind_int(stmt, 1, num_cards_to_buy);
			sqlite3_bind_text(stmt, 2, card_name.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_text(stmt, 3, card_type.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_text(stmt, 4, rarity.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_int(stmt, 5, buyerID);
			if (sqlite3_step(stmt) != SQLITE_DONE) {
				fprintf(stderr, "Failed to update card count: %s\n", sqlite3_errmsg(connection));
				return;
			}
			sqlite3_finalize(stmt);
		}
		else {
			// Insert new card entry
			string insert_card_sql = "INSERT INTO Pokemon_cards (card_name, card_type, rarity, count, owner_id) VALUES (?, ?, ?, ?, ?)";
			if (sqlite3_prepare_v2(connection, insert_card_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
				fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(connection));
				return;
			}
			sqlite3_bind_text(stmt, 1, card_name.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_text(stmt, 2, card_type.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_text(stmt, 3, rarity.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_int(stmt, 4, num_cards_to_buy);
			sqlite3_bind_int(stmt, 5, buyerID);
			if (sqlite3_step(stmt) != SQLITE_DONE) {
				fprintf(stderr, "Failed to insert new card entry: %s\n", sqlite3_errmsg(connection));
				return;
			}
			sqlite3_finalize(stmt);
		}

		// Query the new card count
		string card_count_sql = "SELECT count FROM Pokemon_cards WHERE card_name = ? AND card_type = ? AND rarity = ? AND owner_id = ?";
		if (sqlite3_prepare_v2(connection, card_count_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(connection));
			return;
		}
		sqlite3_bind_text(stmt, 1, card_name.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, card_type.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 3, rarity.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 4, buyerID);
		if (sqlite3_step(stmt) != SQLITE_ROW) {
			fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(connection));
			return;
		}
		int new_card_count = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
		errorCode = 200;
		errorMsg = "OK!";
		cout << "BOUGHT: New Balance: " << new_card_count << " " << card_name << ". User USD Balance $ " << balance - total_cost << endl;
		});
}

void db::sellCard(string card_name, int quantity, double price, int ownerID, int& errorCode, string& errorMsg) {
	executeWithLock([&] {
		errorCode = 0;
		errorMsg = "";

		// Check if user exists
		string user_exists_sql = "SELECT COUNT(*) FROM Users WHERE ID = ?";
		sqlite3_stmt* stmt;
		if (sqlite3_prepare_v2(connection, user_exists_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(connection));
			errorCode = 403;
			errorMsg = "User not found.";
			return;
		}
		sqlite3_bind_int(stmt, 1, ownerID);
		if (sqlite3_step(stmt) != SQLITE_ROW) {
			fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(connection));
			errorCode = 403;
			errorMsg = "User not found.";
			return;
		}
		int userExists = sqlite3_column_int(stmt, 0);
		if (userExists == 0) {
			cout << "User " << ownerID << " does not exist." << endl;
			errorCode = 403;
			errorMsg = "User not found.";
			return;
		}
		sqlite3_finalize(stmt);

		// Check if user owns the card
		string card_exists_sql = "SELECT count FROM Pokemon_cards WHERE card_name = ? AND owner_id = ?";
		if (sqlite3_prepare_v2(connection, card_exists_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(connection));
			errorCode = 403;
			errorMsg = "Card not found.";
			return;
		}
		sqlite3_bind_text(stmt, 1, card_name.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 2, ownerID);
		if (sqlite3_step(stmt) != SQLITE_ROW) {
			fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(connection));
			errorCode = 403;
			errorMsg = "Card not found.";
			return;
		}
		int cardCount = sqlite3_column_int(stmt, 0);
		if (cardCount < quantity) {
			cout << "Not enough cards to sell." << endl;
			errorCode = 404;
			errorMsg = "Not enough cards to sell.";
			return;
		}
		sqlite3_finalize(stmt);

		// Update card count
		string update_card_sql = "UPDATE Pokemon_cards SET count = count - ? WHERE card_name = ? AND owner_id = ?";
		if (sqlite3_prepare_v2(connection, update_card_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(connection));
			errorCode = 403;
			errorMsg = "Failed to update card count.";
			return;
		}
		sqlite3_bind_int(stmt, 1, quantity);
		sqlite3_bind_text(stmt, 2, card_name.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 3, ownerID);
		if (sqlite3_step(stmt) != SQLITE_DONE) {
			fprintf(stderr, "Failed to update card count: %s\n", sqlite3_errmsg(connection));
			errorCode = 403;
			errorMsg = "Failed to update card count.";
			return;
		}
		sqlite3_finalize(stmt);

		// Update user's balance
		string update_balance_sql = "UPDATE Users SET usd_balance = usd_balance + ? WHERE ID = ?";
		if (sqlite3_prepare_v2(connection, update_balance_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(connection));
			errorCode = 404;
			errorMsg = "Failed to update user balance.";
			return;
		}
		sqlite3_bind_double(stmt, 1, price * quantity);
		sqlite3_bind_int(stmt, 2, ownerID);
		if (sqlite3_step(stmt) != SQLITE_DONE) {
			fprintf(stderr, "Failed to update user's balance: %s\n", sqlite3_errmsg(connection));
			errorCode = 404;
			errorMsg = "Failed to update user balance.";
			return;
		}
		sqlite3_finalize(stmt);

		// Query the new card count and user balance
		string card_count_sql = "SELECT count FROM Pokemon_cards WHERE card_name = ? AND owner_id = ?";
		if (sqlite3_prepare_v2(connection, card_count_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(connection));
			return;
		}
		sqlite3_bind_text(stmt, 1, card_name.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 2, ownerID);
		if (sqlite3_step(stmt) != SQLITE_ROW) {
			fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(connection));
			return;
		}
		int new_card_count = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);

		string balance_sql = "SELECT usd_balance FROM Users WHERE ID = ?";
		if (sqlite3_prepare_v2(connection, balance_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(connection));
			return;
		}
		sqlite3_bind_int(stmt, 1, ownerID);
		if (sqlite3_step(stmt) != SQLITE_ROW) {
			fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(connection));
			return;
		}
		double new_balance = sqlite3_column_double(stmt, 0);
		sqlite3_finalize(stmt);

		errorCode = 200;
		errorMsg = "OK!";
		cout << "SOLD: New Balance: " << new_card_count << " " << card_name << ". User's balance USD $ " << new_balance << endl;
		});
}


std::vector<Card> db::getCardsByOwnerId(int owner_id, bool isRootUser) {
	std::vector<Card> cards;
	executeWithLock([&]() {

		std::string query_sql = "SELECT p.ID, p.card_name, p.card_type, p.rarity, p.count, p.owner_id, u.first_name "
			"FROM Pokemon_cards p "
			"JOIN Users u ON p.owner_id = u.ID ";
		if (!isRootUser) {
			query_sql += " WHERE p.owner_id = ?";
		}

		sqlite3_stmt* stmt;
		if (sqlite3_prepare_v2(connection, query_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			// Handle error
			return;
		}

		if (!isRootUser) {
			sqlite3_bind_int(stmt, 1, owner_id);
		}

		while (sqlite3_step(stmt) == SQLITE_ROW) {
			int id = sqlite3_column_int(stmt, 0);
			std::string name((const char*)sqlite3_column_text(stmt, 1));
			std::string type((const char*)sqlite3_column_text(stmt, 2));
			std::string rarity((const char*)sqlite3_column_text(stmt, 3));
			int count = sqlite3_column_int(stmt, 4);
			int owner_id = sqlite3_column_int(stmt, 5);
			std::string owner_name((const char*)sqlite3_column_text(stmt, 6));

			cards.emplace_back(id, name, type, rarity, count, owner_id, owner_name);
		}
		//cout << "Length of cards " << cards.size()<<endl;
		sqlite3_finalize(stmt);
		});
	return cards;
}

bool db::getBalanceByOwnerId(int owner_id, double& balance, std::string& first_name, std::string& last_name, int& errorCode, std::string& errorMsg) {
	balance = 0.0;
	first_name = "";
	last_name = "";
	errorCode = 0;
	errorMsg = "";

	return executeWithLock([&]() {
		std::string query_sql = "SELECT usd_balance, first_name, last_name "
			"FROM Users "
			"WHERE ID = ?";

		sqlite3_stmt* stmt;
		if (sqlite3_prepare_v2(connection, query_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			errorMsg = "Error preparing SQL statement: " + std::string(sqlite3_errmsg(connection));
			errorCode = sqlite3_errcode(connection);
			return false;
		}

		sqlite3_bind_int(stmt, 1, owner_id);

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			balance = sqlite3_column_double(stmt, 0);
			first_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
			last_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		}
		else {
			errorMsg = "User not found.";
			errorCode = 400;  // You can use a custom error code or sqlite3_errcode(connection) as per your requirement.
			sqlite3_finalize(stmt);
			return false;
		}

		sqlite3_finalize(stmt);
		return true;
		});
}
std::vector<Card> db::lookup(const std::string& query, int& errorCode, std::string& errorMsg) {
	std::vector<Card> cards;

	executeWithLock([&]() {
		//cout << "HERE" << endl;
		// Prepare the SQL query
		/*
		"SELECT p.ID, p.card_name, p.card_type, p.rarity, p.count, p.owner_id, u.first_name "
			"FROM Pokemon_cards p "
			*/
		std::string sqlQuery = "SELECT Pokemon_cards.ID, card_name, card_type, rarity, count, owner_id, Users.first_name || ' ' || Users.last_name AS owner_name "
			"FROM Pokemon_cards "
			"JOIN Users ON Pokemon_cards.owner_id = Users.ID "
			"WHERE LOWER(card_name) LIKE LOWER(?1) OR LOWER(card_type) LIKE LOWER(?1) OR LOWER(rarity) LIKE LOWER(?1)";

		//sqlite3_bind_int(stmt, 1, owner_id);
		sqlite3_stmt* stmt;
		if (sqlite3_prepare_v2(connection, sqlQuery.c_str(), -1, &stmt, 0) != SQLITE_OK) {

			errorCode = sqlite3_errcode(connection);
			errorMsg = sqlite3_errmsg(connection);
			cout << errorMsg << errorCode << endl;
			return;
		}


		// Bind the query string to the SQL query sqlite3_bind_text(stmt, 1, first_name.c_str(), -1, SQLITE_STATIC);

		string bind = "%" + query + "%";
		//cout << bind;
		sqlite3_bind_text(stmt, 1, bind.c_str(), -1, SQLITE_STATIC);

		// Execute the SQL query and create Card objects from the results
		while (sqlite3_step(stmt) == SQLITE_ROW) {

			int id = sqlite3_column_int(stmt, 0);
			const char* name = (const char*)sqlite3_column_text(stmt, 1);
			const char* type = (const char*)sqlite3_column_text(stmt, 2);
			const char* rarity = (const char*)sqlite3_column_text(stmt, 3);
			int count = sqlite3_column_int(stmt, 4);
			int owner_id = sqlite3_column_int(stmt, 5);
			const char* owner_name = (const char*)sqlite3_column_text(stmt, 6);
			cout << owner_id << endl;
			cards.emplace_back(id, name, type, rarity, count, owner_id, owner_name);
		}

		if (sqlite3_finalize(stmt) != SQLITE_OK) {
			errorCode = sqlite3_errcode(connection);
			errorMsg = sqlite3_errmsg(connection);
			cout << errorMsg << errorCode << endl;
			return;
		}
		});

	// Check if any records were found
	if (cards.empty()) {
		errorCode = 404;
		errorMsg = "Your search did not match any records.";
	}
	else {
		errorCode = 200;
		errorMsg = "Success";
	}

	return cards;
}


double db::deposit(int owner_id, double amount, int& errorCode, std::string& errorMsg) {
	errorCode = 0;
	errorMsg = "";

	if (amount <= 0) {
		errorMsg = "Invalid deposit amount.";
		errorCode = 400;  // Bad Request
		return -1.0;  // You might want to define a constant for invalid balance.
	}
	
	return executeWithLock([&]() {
		// Get the current balance
		double current_balance;
		std::string first_name, last_name;
		string query_sql = "SELECT usd_balance, first_name, last_name "
			"FROM Users "
			"WHERE ID = ?";

		sqlite3_stmt* stmt;
		if (sqlite3_prepare_v2(connection, query_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			errorMsg = "Error preparing SQL statement: " + std::string(sqlite3_errmsg(connection));
			errorCode = sqlite3_errcode(connection);
			return -1.0;
		}

		sqlite3_bind_int(stmt, 1, owner_id);

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			current_balance = sqlite3_column_double(stmt, 0);
			first_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
			last_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		}
		else {
			errorMsg = "User not found.";
			errorCode = 400;  // You can use a custom error code or sqlite3_errcode(connection) as per your requirement.
			sqlite3_finalize(stmt);
			return -1.0;
		}

		//cout << current_balance << " IS balance" << endl;
		// Update the balance
		double updated_balance = current_balance + amount;

		// Update the balance in the database
		std::string update_sql = "UPDATE Users SET usd_balance = ? WHERE ID = ?";
		
		if (sqlite3_prepare_v2(connection, update_sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
			errorMsg = "Error preparing SQL statement: " + std::string(sqlite3_errmsg(connection));
			errorCode = sqlite3_errcode(connection);
			return -1.0;
		}

		sqlite3_bind_double(stmt, 1, updated_balance);
		sqlite3_bind_int(stmt, 2, owner_id);

		if (sqlite3_step(stmt) != SQLITE_DONE) {
			errorMsg = "Error updating balance: " + std::string(sqlite3_errmsg(connection));
			errorCode = sqlite3_errcode(connection);
			sqlite3_finalize(stmt);
			return -1.0;
		}

		sqlite3_finalize(stmt);
		errorCode = 200;
		errorMsg = "OK!";
		return updated_balance;
		});
}