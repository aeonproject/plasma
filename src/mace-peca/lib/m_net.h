/* 
 * m_net.h : part of the Mace toolkit for building distributed systems
 * 
 * Copyright (c) 2011, Charles Killian, Dejan Kostic, Ryan Braud, James W. Anderson, John Fisher-Ogden, Calvin Hubble, Duy Nguyen, Justin Burke, David Oppenheimer, Amin Vahdat, Adolfo Rodriguez, Sooraj Bhat
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of the contributors, nor their associated universities 
 *      or organizations may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * ----END-OF-LEGAL-STUFF---- */
#ifndef _m_net_h
#define _m_net_h

/**
 * \file m_net.h
 * \brief file to include for networking includes for portability
 */

#include "maceConfig.h"

#ifdef HAVE_NETINET_IN_H
#include "netinet/in.h"
#endif

#ifdef HAVE_NETINET_TCP_H
#include "netinet/tcp.h"
#endif

#ifdef HAVE_NETDB_H
#include "netdb.h"
#endif

#ifdef HAVE_SYS_SELECT_H
#include "sys/select.h"
#endif

#ifdef HAVE_SYS_SOCKET_H
#include "sys/socket.h"
#endif

#ifdef HAVE_ARPA_INET_H
#include "arpa/inet.h"
#endif

#if ! ( defined(HAVE_ARPA_INET_H) || defined(HAVE_SYS_SOCKET_H) || defined(HAVE_SYS_SELECT_H) || defined(HAVE_NETDB_H) || defined(HAVE_NETINET_IN_H) )
#include "windows.h"
#include "winsock2.h"
#include "ws2tcpip.h"

#if defined(WSAENOTCONN) && ! defined(ENOTCONN)
//#define EINPROGRESS WSAEINPROGRESS
//#define ENOBUFS WSAENOBUFS
//#define EPROTO WSAEPROTONOSUPPORT
//#define ECONNABORTED WSAECONNABORTED
//#define EWOULDBLOCK WSAEWOULDBLOCK
//#define ENOTCONN WSAENOTCONN
#define NO_SIGPIPE
#endif

#endif

#endif //_m_net_h
