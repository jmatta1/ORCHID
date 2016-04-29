/***************************************************************************//**
********************************************************************************
**
** @file SnmpConnection.h
** @author James Till Matta
** @date 29 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the class that wraps the net snmp socket with a boost socket
**   the structure and most of the code of this class comes from the internet
**       http://blog-moshe.blogspot.com/2012/10/boost-asio-with-net-snmp.html
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_COMM_SNMPCONNECTION_H
#define ORCHID_SRC_COMM_SNMPCONNECTION_H

// includes for C system headers
// includes for C++ system headers
#include<string>
using std::string;
// includes from other libraries
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <boost/function.hpp>
// includes from ORCHID

typedef boost::function<void(string)> SNMPHandler;

class SnmpConnection
{
    public:                                  
    //construction and destruction
	SnmpConnection(boost::asio::io_service& io_service) :
	    snmpSocket_(io_service), snmpHandle_(NULL) {}
	~SnmpConnection()
	    {snmp_sess_close(this->snmpHandle_);}
    
    //All the necessary functions
	void connect(long version, const string& peername, const string& community);
	void asyncSnmpGet(const string& snmp_oid, SNMPHandler handler);
	void asyncSnmpSet(const string& snmp_oid, const int type, const string& value, SNMPHandler handler);
private:
	void handleSnmpReq(const boost::system::error_code &ec, std::size_t bytes_transferred, struct snmp_pdu *pdu, SNMPHandler handler);
	void handleSnmpRes(const boost::system::error_code &ec, std::size_t bytes_transferred, SNMPHandler handler);

	boost::asio::ip::udp::socket snmpSocket_;
	void *snmpHandle_;
};



#endif //ORCHID_SRC_COMM_SNMPCONNECTION_H
