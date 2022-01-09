/*
 * Google Cloud Certificates
 * Copyright (c) 2019-2020, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef AZURE_CLOUD_CREDENTIALS_H
#define AZURE_CLOUD_CREDENTIALS_H

namespace azure_cloud {
    namespace credentials {
        /*
        * Primary Connecion String
        */

        // Use https://dpsgen.z8.web.core.windows.net/ to obtain YOUR connection string
        // This one will not work, but I guess you knew that ;)
        const char iothub_connection_string[] = "HostName=iotc-0d73069e-b57a-40ca-b02e-da97b9557178.azure-devices.net;DeviceId=aniplscfvz;SharedAccessKey=eTRPIoHESop9g1gsPqfgvoJAuirqBpby2gFJQFvoQ5k=";
    }
}
#endif
