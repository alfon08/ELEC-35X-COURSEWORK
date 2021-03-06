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
        const char iothub_connection_string[] = "HostName=iotc-3e7caa92-161e-4bba-8ae9-b5ff970534fa.azure-devices.net;DeviceId=16ae64elodr;SharedAccessKey=pdobpLlJagqmmTGXGxL+qauh7YmFhXNyvClE/6guVsk=";
    }
}
#endif
