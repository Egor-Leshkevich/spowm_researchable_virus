#pragma once

#include "Email.h"

#define MAX_LEN 258 
#define ENCODED_LEN 341
#define BOUNDARY 5
#define END_LINE 8
#define ATTACH_TYPE 10
#define ATTACH_TRANSFER 11
#define ATTACH_DEPOSITION 12
#define END_OF_TRANSMISSION_BOUNDARY 15
#define END_OF_TRANSMISSION 16

#define DIR_CHAR '\\'


static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp); //This callback function gets called by libcurl when it needs to read data to send it to the peer.
																																//for exp, when u need send data to server	

const  char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

vector<string> global_vec;

struct upload_status {
	int lines_read;
};

static const char *email_template[] = {
	
	"User-Agent:  Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.138 Safari/537.36 OPR/68.0.3618.125 (Edition Campaign 70)\r\n",
	"MIME-Version: 1.0\r\n",
	"Content-Type: multipart/mixed;\r\n",
	" boundary=\"------------030203080101020302070708\"\r\n",
	"\r\nThis is a multi-part message in MIME format.\r\n",
	"--------------030203080101020302070708\r\n",
	"Content-Type: text/plain; charset=utf-8; format=flowed\r\n",
	"Content-Transfer-Encoding: 7bit\r\n",
	
	"\r\n", 
	"--------------030203080101020302070708\r\n",
												  
												  "Content-Type: application/octet-stream\r\n",
												  "Content-Transfer-Encoding: base64\r\n",
												  "Content-Disposition: attachment;\r\n",
												  
												  "\r\n",
												 
												  "\r\n",
												  "--------------030203080101020302070708--\r\n", 
												  "\r\n.\r\n",
												  NULL
};

Email::Email()
{
	
}

void Email::setTo(const string to)
{
	this->to = to;
}

void Email::setFrom(const string from)
{
	this->from = from;
}

void Email::setSubject(const string subject)
{
	this->subject = subject;
}

void Email::setBody(const string body)
{
	this->body = body;
}

void Email::setSMTP_username(const string user)
{
	this->smtp_user = user;
}

void Email::setSMTP_password(const string pass)
{
	this->smtp_password = pass;
}

void Email::setSMTP_host(const string hostname)
{
	this->smtp_host = hostname;
}

void Email::addAttachment(const string file_path)
{
	FILE *fp = NULL;
	char buffer[MAX_LEN + 1] = { 0 };
	char tempBuffer[MAX_LEN] = { 0 };
	char *filename = NULL;
	unsigned int fileSize = 0;
	unsigned int bytesCopied = 0;
	int bytesToCopy = 0;
	int bytesRead = 0;

	fp = fopen(file_path.c_str(), "rb");
	if (fp) {
		// get the file size
		fseek(fp, 0, SEEK_END);
		fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// copy the attachment header
		string type(email_template[ATTACH_TYPE]);
		this->attachments.push_back(type);

		string encodingType(email_template[ATTACH_TRANSFER]);
		this->attachments.push_back(encodingType);

		string deposition(email_template[ATTACH_DEPOSITION]);
		this->attachments.push_back(deposition);

		// extract the filename from the path
		filename = (char *)strrchr(file_path.c_str(), DIR_CHAR);

		if (filename != NULL) {
			filename += 1;
			// push the filename
			snprintf(tempBuffer, MAX_LEN, "  filename=\"%s\"\r\n", filename);

			string filename(tempBuffer);
			this->attachments.push_back(filename);

			string endLine(email_template[END_LINE]);
			this->attachments.push_back(endLine);

			
			while (bytesCopied < fileSize) {
				//how many bytes to read
				if (bytesCopied + MAX_LEN > fileSize) {
					bytesToCopy = fileSize - bytesCopied;
				}

				else {
					bytesToCopy = MAX_LEN;
				}

				// read from the file
				memset(buffer, 0, MAX_LEN + 1);
				bytesRead = fread(buffer, sizeof(char), bytesToCopy, fp);
				string buf = buffer;
				
				string line(buffer);
				line += endLine;

				this->attachments.push_back(line);

				// update number of copied bytes 
				bytesCopied += bytesToCopy;
			}
			
			this->attachments.push_back(endLine);

			string boundary(email_template[BOUNDARY]);
			this->attachments.push_back(boundary);

		}

		else {
			removeAllAttachments();
			cout << "Failed to extract filename!" << endl;
		}


		// close the file
		fclose(fp);
	}

	else {
		cout << "Unable to open file." << endl;
	}
}

void Email::constructEmail(void)
{
	int i = 0;
	char buffer[MAX_LEN];
	string boundary(email_template[BOUNDARY]);

	
	time_t rawtime;
	struct tm * timeinfo;

	// store all email content in a vector
	// TO:
	snprintf(buffer, MAX_LEN, "To: %s\r\n", this->to.c_str());
	string line1(buffer);
	this->email_contents.push_back(line1);
		
	// FROM:
	memset(buffer, 0, MAX_LEN);
	snprintf(buffer, MAX_LEN, "From: %s\r\n", this->from.c_str());
	string line2(buffer);
	this->email_contents.push_back(line2);

	// Subject:
	memset(buffer, 0, MAX_LEN);
	snprintf(buffer, MAX_LEN, "Subject: %s\r\n", this->subject.c_str());
	string line4(buffer);
	this->email_contents.push_back(line4);

	// time:
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	memset(buffer, 0, MAX_LEN);
	strftime(buffer, sizeof(buffer), "%d/%m/%Y %I:%M:%S +1100\r\n", timeinfo);
	string time_str(buffer);

	this->email_contents.push_back(time_str);

	

	for (i = 0; i < END_LINE; i++) { //  user-agent and other
		string line(email_template[i]);
		this->email_contents.push_back(line);
	}

	// add in the body
	string endLine(email_template[END_LINE]);
	this->email_contents.push_back(endLine);

	memset(buffer, 0, MAX_LEN);
	snprintf(buffer, MAX_LEN, "%s", this->body.c_str()); // Body:
	string line5(buffer);

	this->email_contents.push_back(line5); // body
	this->email_contents.push_back(endLine); // \r\n
	this->email_contents.push_back(boundary); // boundary

											  // add in the attachments
	for (i = 0; i < attachments.size(); i++) {
		this->email_contents.push_back(this->attachments[i]);
	}

	// add last boundary with --
	string lastBoundary(email_template[END_OF_TRANSMISSION_BOUNDARY]);
	this->email_contents.push_back(lastBoundary);

	//no more data to send
	string endTransmission(email_template[END_OF_TRANSMISSION]);
	this->email_contents.push_back(endTransmission);
}


int Email::send() const
{
	CURL *curl;
	CURLcode res = CURLE_OK;
	struct curl_slist *reciever = NULL; 
	struct upload_status upload_ctx;

	upload_ctx.lines_read = 0;

	curl = curl_easy_init();

	global_vec = this->email_contents;

	if (curl) {
		/* Set username and password */
		curl_easy_setopt(curl, CURLOPT_USERNAME, this->smtp_user.c_str());
		curl_easy_setopt(curl, CURLOPT_PASSWORD, this->smtp_password.c_str());
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); // allows emails to be sent 
		curl_easy_setopt(curl, CURLOPT_URL, this->smtp_host.c_str());
		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, this->from.c_str());

		//Add reciever
		
		reciever = curl_slist_append(reciever, this->to.c_str());

		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, reciever);

		//specify the payload
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
		curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		/* Send the message */
		res = curl_easy_perform(curl);

		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		/* Free the list of reciever */
		curl_slist_free_all(reciever);

		/* Always cleanup */
		curl_easy_cleanup(curl);
	}

	return (int)res;
}

void Email::removeAllAttachments()
{
	this->attachments.clear();
}


static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
	struct upload_status *upload_ctx = (struct upload_status *)userp;
	const char *data;

	if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
		return 0;
	}

	if (upload_ctx->lines_read >= 0 && upload_ctx->lines_read < global_vec.size())
	{
		data = global_vec[upload_ctx->lines_read].c_str();

		if (data) {
			size_t len = strlen(data);
			memcpy(ptr, data, len);
			upload_ctx->lines_read++;

			return len;
		}
	}

	return 0;
}