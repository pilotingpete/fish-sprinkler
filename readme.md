# Ryan's Water Sculpture Controller

## About

This is code for an arduino to control an art installation project for Ryan.

## Operation Overview

High level overview of sculpture functionality is as follows:

1. Initial state: Pneumatic Solenoid is closed (LOW), Water solenoid is closed (LOW)
2. At the beginning of a cycle the Water Solenoid opens (HIGH) for some duration of time so that just enough water can fill cylinder, after which the Water Solenoid closes (LOW).
3. Wait for the Motion Sensor to be tripped
4. If the Motion Sensor is tripped the Pneumatic Solenoid is opened (High) for just enough time to push the piston up, but not long enough to launch it.
5. GOTO step 2.

- The Sculpture should go into a sleep for some period of time so that it cannot be triggered over and over.
- There should also be a counter to track the number of times it detects motion.
- There should be a hardware switch to force a cycle to start.
