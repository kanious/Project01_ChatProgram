#ifndef _GAMEDB_H_
#define _GAMEDB_H_

#include <string>

namespace sql
{
	class Driver;
	class Connection;
	class ResultSet;
	class Statement;
	class PreparedStatement;
}

class LoginDB
{
private:
	sql::Driver*				m_pDriver;
	sql::Connection*			m_pConnection;
	sql::ResultSet*				m_pResultSet;
	sql::Statement*				m_pStatement;
	sql::PreparedStatement*		m_pEmail_Select;
	sql::PreparedStatement*		m_pUser_Insert;
	sql::PreparedStatement*		m_pAuth_Insert;

public:
	explicit LoginDB();
	~LoginDB();

public:
	bool Connect(std::string hostName, std::string userName, std::string password);
	void Disconnect();
	void Test();
	void CreateUserAccount();
	void AddUser();
	void GetAllUserInfo();
	void GetUserIdFromName();

private:
};

#endif //_GAMEDB_H_