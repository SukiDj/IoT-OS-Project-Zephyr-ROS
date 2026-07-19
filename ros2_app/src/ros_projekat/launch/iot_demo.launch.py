from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='ros_projekat',
            executable='sensor_node',
            name='sensor_node',
            parameters=[{'publish_period_sec': 1.5, 'start_temp_c': 22.0, 'noise_std': 0.8}]
        ),
        Node(
            package='ros_projekat',
            executable='actuator_node',
            name='actuator_node',
            parameters=[{'threshold_c': 28.0}]
        ),
    ])