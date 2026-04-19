#pragma once

#include <Arduino.h>
#include <ctype.h>

//
// SerialConsole<32> console(processCommand);
// void loop() {
// console.handleInput();
// }

/*

void processCommand(const char* input) {
    char cmd = input[0];

    if (cmd == 't' || cmd == 'T') {
        int value = atoi(input + 1);

        value = normalizeAngle(value);

        target_angle = value;
        Serial.printf("target set: %d\n", target_angle);
    }
}

*/

/*

void processCommand(const char* input) {
    if (!input || input[0] == '\0') return;

    if (strcasecmp(input, "stop") == 0) {
        moving         = false;
        wheel_target_1 = wheel_target_2 = wheel_target_3 = 0.0;
        position_pid_x.reset(x_mm);
        position_pid_y.reset(y_mm);
        position_pid_theta.reset(theta);
        Serial.println("stopped");
        return;
    }

    const double raw  = atof(input + 1);
    const double dist = fabs(raw);

    switch (input[0]) {
    case 'r':
    case 'R':
        if (dist <= 0.0) {
            Serial.println("distance must be > 0");
            return;
        }
        target_x_mm  = x_mm + dist;
        target_y_mm  = y_mm;
        target_theta = theta;

        position_pid_x.reset(x_mm);
        position_pid_y.reset(y_mm);
        position_pid_theta.reset(theta);
        moving = true;
        Serial.printf("move right %.1f mm (target_x=%.1f)\n", dist, target_x_mm);
        break;

    case 'l':
    case 'L':
        if (dist <= 0.0) {
            Serial.println("distance must be > 0");
            return;
        }
        target_x_mm  = x_mm - dist;
        target_y_mm  = y_mm;
        target_theta = theta;

        position_pid_x.reset(x_mm);
        position_pid_y.reset(y_mm);
        position_pid_theta.reset(theta);
        moving = true;
        Serial.printf("move left %.1f mm (target_x=%.1f)\n", dist, target_x_mm);
        break;

    case 'f':
    case 'F':
        if (dist <= 0.0) {
            Serial.println("distance must be > 0");
            return;
        }
        target_x_mm  = x_mm;
        target_y_mm  = y_mm + dist;
        target_theta = theta;

        position_pid_x.reset(x_mm);
        position_pid_y.reset(y_mm);
        position_pid_theta.reset(theta);
        moving = true;
        Serial.printf("move front %.1f mm (target_y=%.1f)\n", dist, target_y_mm);
        break;

    case 'b':
    case 'B':
        if (dist <= 0.0) {
            Serial.println("distance must be > 0");
            return;
        }
        target_x_mm  = x_mm;
        target_y_mm  = y_mm - dist;
        target_theta = theta;

        position_pid_x.reset(x_mm);
        position_pid_y.reset(y_mm);
        position_pid_theta.reset(theta);
        moving = true;
        Serial.printf("move back %.1f mm (target_y=%.1f)\n", dist, target_y_mm);
        break;

    case 't':
    case 'T': {
        // t<deg> 例: t90, t-45 （相対回転）
        const double deg = raw;
        if (deg == 0.0) {
            Serial.println("angle must be != 0");
            return;
        }
        target_x_mm  = x_mm;
        target_y_mm  = y_mm;
        target_theta = theta + deg * DEG2RAD;

        position_pid_x.reset(x_mm);
        position_pid_y.reset(y_mm);
        position_pid_theta.reset(theta);
        moving = true;
        Serial.printf("turn %.1f deg (target_theta=%.3f rad)\n", deg, target_theta);
        break;
    }

    default: Serial.println("unknown command: use r/l/f/b/t + number, or stop"); break;
    }
}

*/

template <size_t INPUT_BUFFER_SIZE = 32> class SerialConsole {
    public:
        using CommandHandler = void (*)(const char*);

        explicit SerialConsole(CommandHandler handler) : handler_(handler) {}

        void handleInput() {
            bool updated = false;

            while (Serial.available()) {
                char c = static_cast<char>(Serial.read());

                if (c == '\r' || c == '\n') {
                    Serial.println();
                    if (input_length_ > 0 && handler_) {
                        handler_(input_buffer_);
                        input_length_    = 0;
                        input_buffer_[0] = '\0';
                    }
                    updated = true;
                } else if (c == 0x08 || c == 0x7F) { // BS, DEL
                    if (input_length_ > 0) {
                        input_buffer_[--input_length_] = '\0';
                        updated                        = true;
                    }
                } else if (isprint(static_cast<unsigned char>(c))) {
                    if (input_length_ < INPUT_BUFFER_SIZE - 1) {
                        input_buffer_[input_length_++] = c;
                        input_buffer_[input_length_]   = '\0';
                        updated                        = true;
                    }
                }
            }

            if (updated) {
                refreshPrompt();
            }
        }

        void clearCurrentLine() {
            Serial.print('\r');
            Serial.print("                                                                                ");
            Serial.print('\r');
        }

        void refreshPrompt() {
            Serial.print('\r');
            Serial.print("> ");
            Serial.print(input_buffer_);

            const size_t now_len = 2 + input_length_; // "> " + 入力文字列
            if (last_prompt_len_ > now_len) {
                for (size_t i = 0; i < (last_prompt_len_ - now_len); i++) {
                    Serial.print(' ');
                }
                Serial.print('\r');
                Serial.print("> ");
                Serial.print(input_buffer_);
            }
            last_prompt_len_ = now_len;
        }

    private:
        CommandHandler handler_                         = nullptr;
        char           input_buffer_[INPUT_BUFFER_SIZE] = {0};
        size_t         input_length_                    = 0;
        size_t         last_prompt_len_                 = 0;
};