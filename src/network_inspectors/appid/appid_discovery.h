//--------------------------------------------------------------------------
// Copyright (C) 2014-2017 Cisco and/or its affiliates. All rights reserved.
// Copyright (C) 2005-2013 Sourcefire, Inc.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 2 as published
// by the Free Software Foundation.  You may not use, modify or distribute
// this program under any other version of the GNU General Public License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//--------------------------------------------------------------------------

// appid_discovery.h author Sourcefire Inc.

#ifndef APPID_DISCOVERY_H
#define APPID_DISCOVERY_H

#include <map>
#include <string>
#include <vector>

#include "application_ids.h"

#include "protocols/protocol_ids.h"
#include "search_engines/search_tool.h"
#include "flow/flow.h"
#include "utils/util.h"

class AppIdSession;
class AppIdDetector;
class ServiceDetector;
struct ServiceDetectorPort;
struct Packet;

#define SCAN_HTTP_VIA_FLAG          (1<<0)
#define SCAN_HTTP_USER_AGENT_FLAG   (1<<1)
#define SCAN_HTTP_HOST_URL_FLAG     (1<<2)
#define SCAN_SSL_HOST_FLAG          (1<<4)
#define SCAN_HOST_PORT_FLAG         (1<<5)
#define SCAN_HTTP_VENDOR_FLAG       (1<<6)
#define SCAN_HTTP_XWORKINGWITH_FLAG (1<<7)
#define SCAN_HTTP_CONTENT_TYPE_FLAG (1<<8)

class AppIdPatternMatchNode
{
public:
    AppIdPatternMatchNode(AppIdDetector* detector, int start, unsigned len)
        : service(detector), pattern_start_pos(start), size(len)
    {}

    bool valid_match(int end_position)
    {
        if ( pattern_start_pos >= 0 && pattern_start_pos != (end_position - (int)size) )
            return false;
        else
            return true;
    }

    AppIdDetector* service;
    int pattern_start_pos;
    unsigned size;
};

struct ServiceMatch
{
    struct ServiceMatch* next;
    unsigned count;
    unsigned size;
    ServiceDetector* service = nullptr;
};

typedef std::map<std::string, AppIdDetector*> AppIdDetectors;
typedef AppIdDetectors::iterator AppIdDetectorsIterator;

class AppIdDiscovery
{
public:
    AppIdDiscovery();
    virtual ~AppIdDiscovery();
    static void initialize_plugins();
    static void finalize_plugins();
    static void release_plugins();

    virtual void initialize() = 0;
    virtual void register_detector(const std::string&, AppIdDetector*,  IpProtocol);
    virtual void add_pattern_data(AppIdDetector*, SearchTool*, int position,
        const uint8_t* const pattern, unsigned size, unsigned nocase);
    virtual void register_tcp_pattern(AppIdDetector*, const uint8_t* const pattern, unsigned size,
        int position, unsigned nocase);
    virtual void register_udp_pattern(AppIdDetector*, const uint8_t* const pattern, unsigned size,
        int position, unsigned nocase);
    virtual int add_service_port(AppIdDetector*, const ServiceDetectorPort&);

    static void do_application_discovery(Packet* p);

    AppIdDetectors* get_tcp_detectors()
    {
        return &tcp_detectors;
    }

    AppIdDetectors* get_udp_detectors()
    {
        return &udp_detectors;
    }

protected:
    AppIdDetectors tcp_detectors;
    AppIdDetectors udp_detectors;
    SearchTool* tcp_patterns = nullptr;
    int tcp_pattern_count = 0;
    SearchTool* udp_patterns = nullptr;
    int udp_pattern_count = 0;
    std::vector<AppIdPatternMatchNode*> pattern_data;
};
#endif

