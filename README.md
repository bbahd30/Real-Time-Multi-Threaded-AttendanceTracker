# Overview
The Real-Time, Multi-Threaded Attendance Tracker is a cutting-edge software solution designed to streamline and enhance attendance management for educational institutions, corporate organizations, and events. This innovative system leverages advanced technologies to offer real-time tracking, multi-threaded processing, and comprehensive attendance analytics.

# Key Features
- Real-Time Tracking: The system offers real-time attendance tracking, ensuring that attendance records are updated instantly as participants check in or out. This feature enhances accuracy and eliminates the need for manual data entry.
- Multi-Threaded Processing: The attendance tracker employs a multi-threaded architecture to efficiently handle concurrent attendance updates from multiple sources. This ensures high performance and scalability, even in large organizations or events.
- Customizable Attendance Rules: Users can define and customize attendance rules based on their specific needs. Whether it's class attendance, employee attendance, or event participation, the system can adapt to various scenarios.
- Integration Capabilities: The system can seamlessly integrate with existing databases, student information systems, HR software, or event management tools, ensuring data consistency and reducing manual data transfer.

# Usage
- Run Server.cpp
```console
$ g++ server.cpp -o server -pthread
$ ./server
```
- Run Client
```console
$ g++ client.cpp -o client -pthread
$ ./client 127.0.0.1
```
