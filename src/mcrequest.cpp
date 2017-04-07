#include "mcrequest.hpp"

/*
 * address ---------------------------------------------------
 */
MCAddress::MCAddress()
	: address("0.0.0.0"),
	  port(0)
{}

MCAddress::MCAddress(
		QHostAddress address,
		int port)
	: address(address),
	  port(port)
{}

MCAddress::~MCAddress()
{}

QHostAddress MCAddress::getAddress()
{
	return this->address;
}

int MCAddress::getPort()
{
	return this->port;
}

void MCAddress::setAddress(QHostAddress& addr)
{
	this->address = addr;
}

void MCAddress::setPort(int port)
{
	this->port = port;
}

bool MCAddress::read(const QJsonObject &json)
{
	this->address	= QHostAddress(json["ip"].toString());
	this->port	= json["port"].toInt();
	
	return true;
}

bool MCAddress::write(QJsonObject &json)
{
	json["ip"]	= this->address.toString();
	json["port"]	= this->port;
	
	return true;
}

/*
 * request -------------------------------------------------
 */
const QString MCRequest::JSON_TYPE = "request";

MCRequest::MCRequest()
{}

MCRequest::~MCRequest()
{}

bool MCRequest::read(const QJsonObject& json)
{
	if(json["type"].toString() != JSON_TYPE)
		return false;

	return true;
}

void MCRequest::setPeer(QHostAddress& h, int port)
{
	peer.setAddress(h);
	peer.setPort(port);
}

bool MCRequest::write(QJsonObject& json)
{
	json["type"] = JSON_TYPE;

	QJsonObject peer;
	if(!this->peer.write(peer))
		return false;

	json["peer"] = peer;

	return true;
}

/*
 * request_db ---------------------------------------------
 */
const QString MCRequestDB::method = "request_db";

MCRequestDB::MCRequestDB()
{}

MCRequestDB::~MCRequestDB()
{}

bool MCRequestDB::read(const QJsonObject &json)
{
	if(!MCRequest::read(json))
		return false;

	if(json["method"].toString() != method)
		return false;

	if(!responseAddress.read(json["responseAddress"].toObject()))
		return false;

	return true;
}

bool MCRequestDB::write(QJsonObject &json)
{
	if(!MCRequest::write(json))
		return false;

	json["method"]	= method;

	QJsonObject responseAddr;
	responseAddress.write(responseAddr);
	json["responseAddress"] = responseAddr;

	return true;
}

void MCRequestDB::setListeningAddress(QHostAddress& ip)
{
	this->responseAddress.setAddress(ip);
}

void MCRequestDB::setListeningPort(int port)
{
	this->responseAddress.setPort(port);
}

MCAddress MCRequestDB::getResponseAddress() const
{
	return responseAddress;
}

/*
 * response -----------------------------------------------
 */
const QString MCResponse::JSON_TYPE = "response";

MCResponse::MCResponse()
{}

MCResponse::~MCResponse()
{}

bool MCResponse::read(const QJsonObject& json)
{
	if(json["type"].toString() != JSON_TYPE)
		return false;
	return true;
}

bool MCResponse::write(QJsonObject& json)
{
	json["type"] = JSON_TYPE;

	return true;
}

/*
 * response_db --------------------------------------------
 */
const QString MCResponseDB::method = "response_db";

MCResponseDB::MCResponseDB()
{}

MCResponseDB::MCResponseDB(MCRequestDB* request)
	: request(request)
{}

MCResponseDB::~MCResponseDB()
{
	if(request)
		delete request;
}

bool MCResponseDB::read(const QJsonObject &json)
{
	qDebug() << "MCResponseDB";
	if(!MCResponse::read(json))
		return false;

	if(json["method"].toString() != this->method)
		return false;

	this->dbData = json["dbData"].toString();

	return true;
}

bool MCResponseDB::write(QJsonObject &json)
{
	if(!MCResponse::write(json))
		return false;

	json["method"] = this->method;
	json["dbData"] = this->dbData;

	return true;
}

QString MCResponseDB::getDbData()
{
	return this->dbData;
}

void MCResponseDB::setDbData(QString s)
{
	this->dbData = s;
}

MCRequestDB* MCResponseDB::getRequest()
{
	return this->request;
}

/*
 * miner_request
 */
const QString MCRequestMiner::method = "miner_request";

MCRequestMiner::MCRequestMiner()
{
}

MCRequestMiner::MCRequestMiner(
		const MCTransaction& t,
		const MCSignature& s)
{
	this->transaction = t;
	this->signature = s;
}

MCRequestMiner::~MCRequestMiner()
{
}

MCSignature MCRequestMiner::getSignature() const
{
	return signature;
}

void MCRequestMiner::setSignature(const MCSignature &value)
{
	signature = value;
}

MCTransaction MCRequestMiner::getTransaction() const
{
	return transaction;
}

void MCRequestMiner::setTransaction(const MCTransaction &value)
{
	transaction = value;
}

bool MCRequestMiner::read(const QJsonObject& json)
{
	if(!MCRequest::read(json))
		return false;

	if(json["method"] != this->method)
		return false;

	if(!this->signature.read(json["signature"].toObject()))
		return false;

	if(!this->transaction.read(json["transaction"].toObject()))
		return false;

	return true;
}

bool MCRequestMiner::write(QJsonObject& json)
{
	if(!MCRequest::write(json))
		return false;
	json["method"]		= this->method;

	QJsonObject jSignature;
	if(!this->signature.write(jSignature))
		return false;
	json["signature"] = jSignature;

	QJsonObject jTransaction;
	if(!this->transaction.write(jTransaction))
		return false;
	json["transaction"] = jTransaction;

	return true;
}