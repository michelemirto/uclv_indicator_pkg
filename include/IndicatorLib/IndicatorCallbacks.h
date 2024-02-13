#pragma once

#include "std_srvs/Trigger.h"
#include "Indicator.h"

namespace uclv
{
    // Service callback for indicator reset
    bool zero_cb(std_srvs::Trigger::Request &req, std_srvs::Trigger::Response &resp, uclv::Indicator &ind)
    {
        ind.zero_indicator();
        resp.success = true;
        return true;
    }
}