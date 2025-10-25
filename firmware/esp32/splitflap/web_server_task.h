/*
   Copyright 2024 Scott Bezek and the splitflap contributors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#include "../core/logger.h"
#include "../core/splitflap_task.h"
#include "../core/task.h"

class WebServerTask : public Task<WebServerTask> {
    friend class Task<WebServerTask>; // Allow base Task to invoke protected run()

    public:
        WebServerTask(SplitflapTask& splitflap_task, Logger& logger, const uint8_t task_core, const uint16_t port = 80);

    protected:
        void run();

    private:
        void setupWebServer();
        String getAlphabetString();
        String getStatusJSON();
        String getHistoryJSON();
        String getAdminStatusJSON();
        void addToHistory(const String& text);
        bool checkAuthentication();
        bool checkAdminAuthentication();

        SplitflapTask& splitflap_task_;
        Logger& logger_;
        WebServer server_;
        bool server_started_ = false;
        
        // History storage
        static const int MAX_HISTORY = 10;
        String history_[MAX_HISTORY];
        int history_count_ = 0;
        
        // Display state
        bool display_enabled_ = true;
        
        // Embedded frontend files
        static const char HTML_CONTENT[];
};
