#include "LoginDB.h"
#include "jdbc/mysql_driver.h"
#include "jdbc/mysql_connection.h"
#include "jdbc/mysql_error.h"
#include "jdbc/cppconn/statement.h"
#include "jdbc/cppconn/prepared_statement.h"
#include "jdbc/cppconn/resultset.h"
#include "StaticFunc.h"
#include <sstream>

using namespace sql;
using namespace std;

LoginDB::LoginDB()
	: m_pDriver(nullptr), m_pConnection(nullptr), m_pResultSet(nullptr)
	, m_pStatement(nullptr), m_pUser_Insert(nullptr), m_pAuth_Insert(nullptr)
{
}

LoginDB::~LoginDB()
{
}

bool LoginDB::Connect(string hostName, string userName, string password)
{
	//Get.Driver
	printf("  Get Driver . . . ");
	try
	{
		m_pDriver = mysql::get_driver_instance();
	}
	catch (SQLException e)
	{
		cout << "falied: " << e.what() << endl;
		return false;
	}
	if (nullptr == m_pDriver) return false;
	printf("OK!\n");

	//Connect.To.SQL
	printf("  Connect To SQL . . . ");
	try
	{

 		m_pConnection = m_pDriver->connect(hostName, userName, password);
		if (nullptr == m_pConnection)
		{
			cout << "failed" << endl;
			return false;
		}
		m_pConnection->setSchema("authentication_service");
	}
	catch (SQLException e)
	{
		cout << "falied: " << e.what() << endl;
		return false;
	}
	printf("OK!\n");

	//Setting.SQL.instances
	printf("  Preparing SQL Query . . . ");
	try
	{
		m_pStatement = m_pConnection->createStatement();
		stringstream query;
		query << "SELECT * FROM authentication_service.web_auth WHERE email=?";
		m_pEmail_Select = m_pConnection->prepareStatement(query.str());

		stringstream ss;
		ss << "INSERT INTO `authentication_service`.`user` ";
		ss << "(`username`, `last_login`, `creation_date`) ";
		ss << "VALUES (?, now(), now());";
		m_pUser_Insert = m_pConnection->prepareStatement(ss.str());

		ss.str("");
		ss << "INSERT INTO `authentication_service`.`web_auth` ";
		ss << "(`email`, `salt`, `hashed_password`, `user_id`) ";
		ss << "VALUES (?, ?, ?, ?);";
		m_pAuth_Insert = m_pConnection->prepareStatement(ss.str());
	}
	catch (SQLException e)
	{
		cout << "falied: " << e.what() << endl;
		return false;
	}
	printf("OK!\n");

	return true;
}

void LoginDB::Disconnect()
{
	printf("  Disconnect SQL . . . ");
	try
	{
		if (nullptr != m_pConnection)
		{
			m_pConnection->close();
			m_pConnection = nullptr;
		}
	}
	catch (SQLException e)
	{
		cout << "falied: " << e.what() << endl;
		return;
	}

	SafeDelete(m_pResultSet);
	SafeDelete(m_pStatement);
	SafeDelete(m_pEmail_Select);
	SafeDelete(m_pUser_Insert);
	SafeDelete(m_pAuth_Insert);
	printf("OK!\n");
}

void LoginDB::Test()
{
	m_pEmail_Select->setString(1, "kanious17@gmail.com");
	try
	{
		m_pResultSet = m_pEmail_Select->executeQuery();
	}
	catch (SQLException e)
	{
		cout << endl << "Test falied: " << e.what() << endl;
		return;
	}

	if (nullptr == m_pResultSet) return;
	while (m_pResultSet->next())
	{
		string id = m_pResultSet->getString("id");
		string email = m_pResultSet->getString("email");
		string salt = m_pResultSet->getString("salt");
		string pass = m_pResultSet->getString("hashed_password");
		string user_id = m_pResultSet->getString("user_id");
		printf("\n%s\n%s\n%s\n%s\n%s\n", id.c_str(), email.c_str(), salt.c_str()
			, pass.c_str(), user_id.c_str());
	}
}

void LoginDB::CreateUserAccount()
{
}

void LoginDB::AddUser()
{
	//stringstream ss;
	//ss << "INSERT INTO `authentication_service`.`web_auth` ";
	//ss << "(`email`, `salt`, `hashed_password`, `user_id`) ";
	//ss << "VALUES ('kanious17@gmail.com2', 'saltsalt', '@$%#^#$%^daf', '1234');";
	//m_pPreparedStatement = m_pConnection->prepareStatement(ss.str());
	//m_pPreparedStatement->execute();

	//asctime(localtime(&start_time));

	//m_pUser_Insert->setString(1, "newUserName01");
	//m_pUser_Insert->setDateTime(2, "now()");

	//m_pAuth_Insert->setString(1, "new@emailcom");
	//m_pAuth_Insert->setString(2, "newsalt");
	//m_pAuth_Insert->setString(3, "newHashed");
	//m_pAuth_Insert->setBigInt(4, "5678");

	stringstream ss;
	ss << "INSERT INTO `authentication_service`.`user` ";
	ss << "(`username`, `last_login`, `creation_date`) ";
	ss << "VALUES ('heyyy', 'now()', 'now()');";
	//m_pUser_Insert;

	//Execute/ExecuteQuery,ExecuteUpdate 차이점: https://mozi.tistory.com/26
	//쿼리문 설명: https://blog.naver.com/avec_o/222742057644

	try
	{
		if (nullptr == m_pUser_Insert)
			return;
		//m_pUser_Insert->execute();
		m_pResultSet = m_pUser_Insert->executeQuery(ss.str());

		//if (nullptr == m_pAuth_Insert)
		//	return;
		//m_pAuth_Insert->execute();
	}
	catch (SQLException e)
	{
		cout << e.what() << endl;
		return;
	}

	cout << "Insert Success" << endl;

	if (nullptr == m_pResultSet)
		return;

	while (m_pResultSet->next())
	{
		string id = m_pResultSet->getString("id");
		printf("\nID : %s\n\n", id.c_str());
	}
}

void LoginDB::GetAllUserInfo()
{
	if (nullptr == m_pStatement)
		return;

	try
	{
		m_pResultSet = m_pStatement->executeQuery(
			"SELECT * FROM web_auth;"
		);
	}
	catch (SQLException e)
	{
		cout << e.what() << endl;
		return;
	}

	if (nullptr == m_pResultSet)
		return;

	while (m_pResultSet->next())
	{
		string id = m_pResultSet->getString("id");
		string email = m_pResultSet->getString("email");
		string salt = m_pResultSet->getString("salt");
		string hashed_password = m_pResultSet->getString("hashed_password");
		string user_id = m_pResultSet->getString("user_id");

		printf("\n%s\n%s\n%s\n%s\n%s\n\n", id.c_str(), email.c_str()
			, salt.c_str(), hashed_password.c_str(), user_id.c_str());
	}
}

void LoginDB::GetUserIdFromName()
{
	if (nullptr == m_pStatement)
		return;

	try
	{
		m_pResultSet = m_pStatement->executeQuery(
			"SELECT id FROM authentication_service.user WHERE username='newUserName01';"
		);
	}
	catch (SQLException e)
	{
		cout << e.what() << endl;
		return;
	}

	if (nullptr == m_pResultSet) return;
	while (m_pResultSet->next())
	{
		string id = m_pResultSet->getString("id");
		printf("\nID : %s\n\n", id.c_str());
	}
}
