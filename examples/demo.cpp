// Copyright 2025 Enactic, Inc.
// ... (License header) ...

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <openarm/can/socket/openarm.hpp>
#include <openarm/damiao_motor/dm_motor_constants.hpp>
#include <thread>

int main() {
    try {
        std::cout << "=== OpenArm CAN Example ===" << std::endl;
        std::cout << "This example demonstrates the OpenArm API functionality" << std::endl;

        // ===================================================================
        // 1. 已修改: 设置为 CAN 2.0 (false)
        // 假设您使用 "can0"。如果您连接在 "can1"，请将 "can0" 改为 "can1"。
        // ===================================================================
        std::cout << "Initializing OpenArm CAN..." << std::endl;
        openarm::can::socket::OpenArm openarm("can0", false);  // 使用 CAN 2.0

        // ===================================================================
        // 2. 已修改: 配置 7 个臂部电机 (电机 1-7)
        // ===================================================================
        std::vector<openarm::damiao_motor::MotorType> motor_types = {
            openarm::damiao_motor::MotorType::DM10010, // 电机 1 (10010)
            openarm::damiao_motor::MotorType::DM10010, // 电机 2 (10010)
            
            // 已更新为 DM6248
            openarm::damiao_motor::MotorType::DM6248,  // 电机 3 (6248)
            openarm::damiao_motor::MotorType::DM6248,  // 电机 4 (6248)
            
            openarm::damiao_motor::MotorType::DM4340,  // 电机 5 (4340P)
            openarm::damiao_motor::MotorType::DM4340,  // 电机 6 (4340P)
            openarm::damiao_motor::MotorType::DM4340   // 电机 7 (4340P)
        };
        
        // 3. !! 必须修改 !!: 配置 7 个臂部电机的 CAN ID
        // (这里假设它们是 0x01 到 0x07，请改为您的实际 ID)
        std::vector<uint32_t> send_can_ids = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
        // (接收 ID 通常是 发送ID + 0x10)
        std::vector<uint32_t> recv_can_ids = {0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
        openarm.init_arm_motors(motor_types, send_can_ids, recv_can_ids);

        // ===================================================================
        // 4. 已修改: 配置 1 个夹爪电机 (电机 8)
        // ===================================================================
        std::cout << "Initializing gripper..." << std::endl;
        // !! 必须修改 !!: 假设夹爪 ID 是 0x08，请改为您的实际 ID
        openarm.init_gripper_motor(openarm::damiao_motor::MotorType::DM4310, 0x08, 0x18); //

        // Set callback mode to ignore and enable all motors
        openarm.set_callback_mode_all(openarm::damiao_motor::CallbackMode::IGNORE);

        // Enable all motors
        std::cout << "\n=== Enabling Motors ===" << std::endl;
        openarm.enable_all();
        openarm.recv_all(2000);

        // ... (后续代码与之前相同，保持不变) ...

        // Set device mode to param and query motor id
        std::cout << "\n=== Querying Motor Recv IDs ===" << std::endl;
        openarm.set_callback_mode_all(openarm::damiao_motor::CallbackMode::PARAM);
        openarm.query_param_all(static_cast<int>(openarm::damiao_motor::RID::MST_ID));
        openarm.recv_all(2000);

        // Access motors through components
        for (const auto& motor : openarm.get_arm().get_motors()) {
            std::cout << "Arm Motor: " << motor.get_send_can_id() << " ID: "
                      << motor.get_param(static_cast<int>(openarm::damiao_motor::RID::MST_ID))
                      << std::endl;
        }
        for (const auto& motor : openarm.get_gripper().get_motors()) {
            std::cout << "Gripper Motor: " << motor.get_send_can_id() << " ID: "
                      << motor.get_param(static_cast<int>(openarm::damiao_motor::RID::MST_ID))
                      << std::endl;
        }

        // Set device mode to state and control motor
        std::cout << "\n=== Controlling Motors ===" << std::endl;
        openarm.set_callback_mode_all(openarm::damiao_motor::CallbackMode::STATE);

        // ===================================================================
        // 5. 已修改: 控制参数 (mit_control_all)
        // 列表长度必须与您的臂部电机数量 (7) 一致！
        // ===================================================================
        openarm.get_arm().mit_control_all({
            openarm::damiao_motor::MITParam{2, 1, 0, 0, 0}, // 电机 1
            openarm::damiao_motor::MITParam{2, 1, 0, 0, 0}, // 电机 2
            openarm::damiao_motor::MITParam{2, 1, 0, 0, 0}, // 电机 3
            openarm::damiao_motor::MITParam{2, 1, 0, 0, 0}, // 电机 4
            openarm::damiao_motor::MITParam{2, 1, 0, 0, 0}, // 电机 5
            openarm::damiao_motor::MITParam{2, 1, 0, 0, 0}, // 电机 6
            openarm::damiao_motor::MITParam{2, 1, 0, 0, 0}  // 电机 7
        });
        openarm.recv_all(500);

        // (同样，这里也要改成 7 个)
        openarm.get_arm().mit_control_all({
            openarm::damiao_motor::MITParam{0, 0, 0, 0, 0.1}, // 电机 1
            openarm::damiao_motor::MITParam{0, 0, 0, 0, 0.1}, // 电机 2
            openarm::damiao_motor::MITParam{0, 0, 0, 0, 0.1}, // 电机 3
            openarm::damiao_motor::MITParam{0, 0, 0, 0, 0.1}, // 电机 4
            openarm::damiao_motor::MITParam{0, 0, 0, 0, 0.1}, // 电机 5
            openarm::damiao_motor::MITParam{0, 0, 0, 0, 0.1}, // 电机 6
            openarm::damiao_motor::MITParam{0, 0, 0, 0, 0.1}  // 电机 7
        });
        openarm.recv_all(500);

        // Control gripper
        std::cout << "Closing gripper..." << std::endl;
        openarm.get_gripper().close();
        openarm.recv_all(1000);

        for (int i = 0; i < 10; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            openarm.refresh_all();
            openarm.recv_all(300);

            // Display arm motor states
            for (const auto& motor : openarm.get_arm().get_motors()) {
                std::cout << "Arm Motor: " << motor.get_send_can_id()
                          << " position: " << motor.get_position() << std::endl;
            }
            // Display gripper state
            for (const auto& motor : openarm.get_gripper().get_motors()) {
                std::cout << "Gripper Motor: " << motor.get_send_can_id()
                          << " position: " << motor.get_position() << std::endl;
            }
        }

        openarm.disable_all();
        openarm.recv_all(1000);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}


// // Copyright 2025 Enactic, Inc.
// //
// // Licensed under the Apache License, Version 2.0 (the "License");
// // you may not use this file except in compliance with the License.
// // You may obtain a copy of the License at
// //
// //     http://www.apache.org/licenses/LICENSE-2.0
// //
// // Unless required by applicable law or agreed to in writing, software
// // distributed under the License is distributed on an "AS IS" BASIS,
// // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// // See the License for the specific language governing permissions and
// // limitations under the License.

// #include <atomic>
// #include <chrono>
// #include <csignal>
// #include <iostream>
// #include <openarm/can/socket/openarm.hpp>
// #include <openarm/damiao_motor/dm_motor_constants.hpp>
// #include <thread>

// int main() {
//     try {
//         std::cout << "=== OpenArm CAN Example ===" << std::endl;
//         std::cout << "This example demonstrates the OpenArm API functionality" << std::endl;

//         // Initialize OpenArm with CAN interface and enable CAN-FD
//         std::cout << "Initializing OpenArm CAN..." << std::endl;
//         openarm::can::socket::OpenArm openarm("can0", true);  // Use CAN-FD on can0 interface

//         // Initialize arm motors
//         std::vector<openarm::damiao_motor::MotorType> motor_types = {
//             openarm::damiao_motor::MotorType::DM4310, openarm::damiao_motor::MotorType::DM4310};
//         std::vector<uint32_t> send_can_ids = {0x01, 0x02};
//         std::vector<uint32_t> recv_can_ids = {0x11, 0x12};
//         openarm.init_arm_motors(motor_types, send_can_ids, recv_can_ids);

//         // Initialize gripper
//         std::cout << "Initializing gripper..." << std::endl;
//         openarm.init_gripper_motor(openarm::damiao_motor::MotorType::DM4310, 0x08, 0x18);

//         // Set callback mode to ignore and enable all motors
//         openarm.set_callback_mode_all(openarm::damiao_motor::CallbackMode::IGNORE);

//         // Enable all motors
//         std::cout << "\n=== Enabling Motors ===" << std::endl;
//         openarm.enable_all();
//         // Allow time (2ms) for the motors to respond for slow operations like enabling
//         openarm.recv_all(2000);

//         // Set device mode to param and query motor id
//         std::cout << "\n=== Querying Motor Recv IDs ===" << std::endl;
//         openarm.set_callback_mode_all(openarm::damiao_motor::CallbackMode::PARAM);
//         openarm.query_param_all(static_cast<int>(openarm::damiao_motor::RID::MST_ID));
//         // Allow time (2ms) for the motors to respond for slow operations like querying
//         // parameter from register
//         openarm.recv_all(2000);

//         // Access motors through components
//         for (const auto& motor : openarm.get_arm().get_motors()) {
//             std::cout << "Arm Motor: " << motor.get_send_can_id() << " ID: "
//                       << motor.get_param(static_cast<int>(openarm::damiao_motor::RID::MST_ID))
//                       << std::endl;
//         }
//         for (const auto& motor : openarm.get_gripper().get_motors()) {
//             std::cout << "Gripper Motor: " << motor.get_send_can_id() << " ID: "
//                       << motor.get_param(static_cast<int>(openarm::damiao_motor::RID::MST_ID))
//                       << std::endl;
//         }

//         // Set device mode to state and control motor
//         std::cout << "\n=== Controlling Motors ===" << std::endl;
//         openarm.set_callback_mode_all(openarm::damiao_motor::CallbackMode::STATE);

//         // Control arm motors with position control
//         openarm.get_arm().mit_control_all({openarm::damiao_motor::MITParam{2, 1, 0, 0, 0},
//                                            openarm::damiao_motor::MITParam{2, 1, 0, 0, 0}});
//         openarm.recv_all(500);

//         // Control arm motors with torque control
//         openarm.get_arm().mit_control_all({openarm::damiao_motor::MITParam{0, 0, 0, 0, 0.1},
//                                            openarm::damiao_motor::MITParam{0, 0, 0, 0, 0.1}});
//         openarm.recv_all(500);

//         // Control gripper
//         std::cout << "Closing gripper..." << std::endl;
//         openarm.get_gripper().close();
//         openarm.recv_all(1000);

//         for (int i = 0; i < 10; i++) {
//             std::this_thread::sleep_for(std::chrono::milliseconds(100));

//             openarm.refresh_all();
//             openarm.recv_all(300);

//             // Display arm motor states
//             for (const auto& motor : openarm.get_arm().get_motors()) {
//                 std::cout << "Arm Motor: " << motor.get_send_can_id()
//                           << " position: " << motor.get_position() << std::endl;
//             }
//             // Display gripper state
//             for (const auto& motor : openarm.get_gripper().get_motors()) {
//                 std::cout << "Gripper Motor: " << motor.get_send_can_id()
//                           << " position: " << motor.get_position() << std::endl;
//             }
//         }

//         openarm.disable_all();
//         openarm.recv_all(1000);

//     } catch (const std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return -1;
//     }

//     return 0;
// }
