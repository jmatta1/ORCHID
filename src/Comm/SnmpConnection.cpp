/***************************************************************************//**
********************************************************************************
**
** @file SnmpConnection.cpp
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
** @details Holds the class implementation that wraps the net snmp socket with a
**   boost socket the structure and most of the code of this class comes from:
**       http://blog-moshe.blogspot.com/2012/10/boost-asio-with-net-snmp.html
**
********************************************************************************
*******************************************************************************/

#include"SnmpConnection.h"
// includes for C system headers
// includes for C++ system headers
#include<iostream>
using std::cout;
using std::endl;
// includes from other libraries
// includes from ORCHID


int asynch_response(int operation, struct snmp_session *sp, int reqid, struct snmp_pdu *pdu, void *magic)
{
    if (operation == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE)
    {
        char buf[1024];

        if (pdu->errstat == SNMP_ERR_NOERROR) 
        {
            cout << "SNMP_ERR_NOERROR" << endl;
            if(pdu->variables)
            {
                //print_variable(pdu->variables->name, pdu->variables->name_length,  pdu->variables);
                snprint_variable(buf, sizeof(buf), pdu->variables->name, pdu->variables->name_length, pdu->variables);
                //snmp_connection::response_ = buf;
            }
        }

    }
    return 1;
}

void SnmpConnection::connect(long version, const string& peername, const string& community)
{
    struct snmp_session sess;
    snmp_sess_init(&sess);   /* initialize session */
    sess.version = SNMP_VERSION_2c;
    sess.peername = strdup(peername.c_str());
    sess.community = reinterpret_cast<u_char*>(strdup(community.c_str()));
    sess.community_len = community.size();
    sess.callback = asynch_response;
    snmpHandle_ = snmp_sess_open(&sess);
    free(sess.peername);
    free(sess.community);
    netsnmp_transport *transport  = snmp_sess_transport( snmpHandle_ );
    snmpSocket_.assign(boost::asio::ip::udp::v4(), transport->sock);
    // Put the socket into non-blocking mode.
    boost::asio::ip::udp::socket::non_blocking_io non_blocking_io(true);
    snmpSocket_.io_control(non_blocking_io);    
}

void SnmpConnection::asyncSnmpGet(const string& snmp_oid, SNMPHandler handler)
{
    struct snmp_pdu *pdu = NULL;
    oid anOID[MAX_OID_LEN];
    size_t anOID_len = MAX_OID_LEN;

    pdu = snmp_pdu_create(SNMP_MSG_GET);   
    if (!snmp_parse_oid(snmp_oid.c_str(), anOID, &anOID_len)) {
        snmp_perror(snmp_oid.c_str());
        cout << "error snmp_parse_oid" << endl;
    }    

    snmp_add_null_var(pdu, anOID, anOID_len);
    snmpSocket_.async_send(boost::asio::null_buffers(),
                           boost::bind(&SnmpConnection::handleSnmpReq, this,
                                       boost::asio::placeholders::error,
                                       boost::asio::placeholders::bytes_transferred,
                                       pdu, handler));
}

void SnmpConnection::handleSnmpReq(const boost::system::error_code &ec, std::size_t bytes_transferred, struct snmp_pdu *pdu, SNMPHandler handler)
{
    if (this->snmpSocket_.is_open()){
        if (ec)
        {
            cout << "error snmp_connection::handle_snmp_req" << endl;
        }
        else
        {
            // Notify net-snmp library that it can perform a write. 
            snmp_sess_send(snmpHandle_, pdu);
            snmpSocket_.async_receive(boost::asio::null_buffers(),
                                      boost::bind(&SnmpConnection::handleSnmpRes, this, 
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred,
                                      handler));
        }
    }    
}
void SnmpConnection::handleSnmpRes(const boost::system::error_code &ec,
                                     std::size_t bytes_transferred,
                                     SNMPHandler handler)
{
    if (this->snmpSocket_.is_open())
    {
        if (ec)
        {
            cout << "error snmp_connection::handle_snmp_res" << endl;
        }
        else
        {      
            fd_set snmp_fds;
            FD_ZERO(&snmp_fds);
            FD_SET(snmpSocket_.native(), &snmp_fds);
            snmp_sess_read(snmpHandle_, &snmp_fds);
        }
    }
}
/*
void SnmpConnection::handleSnmpRes(const boost::system::error_code &ec, std::size_t bytesTransferred, SNMPHandler handler) 
{
    if (this->snmpSocket_.is_open())
    {
        if (ec)
        {
            cout << "error snmp_connection::handle_snmp_res" << endl;
        }
        else
        {      
            fd_set snmp_fds;
            FD_ZERO(&snmp_fds);
            FD_SET(snmp_socket_.native(), &snmp_fds);
            snmp_sess_read(snmpHandle_, &snmp_fds);
            handler(snmp_response);
        }
    }
}*/
