# Advanced Operating Systems - IoT OS Project

This repository contains the implementation of Project 2 for the Advanced Operating Systems course. The project demonstrates core Internet of Things (IoT) and Real-Time Operating System (RTOS) concepts using **Zephyr OS** and **ROS2 (Robot Operating System)**.

## Project Structure

The project is divided into two main parts. Both applications simulate a simple sensor-actuator logic: reading temperature data and triggering a warning (actuation) when the temperature exceeds a defined threshold.

* `/zephyr_app`: Contains the C implementation for Zephyr RTOS. It utilizes the Zephyr native simulator (`native_sim`) to run the RTOS environment and execute tasks without physical hardware.
* `/ros2_app`: Contains the ROS2 Python package implementing a Publisher (sensor node) and Subscriber (actuator node) architecture communicating via topics.

## Environment & Execution

To ensure an isolated and reproducible development environment, both applications were developed, compiled, and tested using **Docker**.

* **Zephyr OS:** Built using the official `zephyrprojectrtos/zephyr-build:latest` Docker image.
* **ROS2:** Executed using the `osrf/ros:humble-desktop` Docker image.

## Author

**Aleksandar Djordjevic (AD)**
